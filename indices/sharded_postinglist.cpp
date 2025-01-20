#include "sharded_postinglist.h"


void ShardedPostingList::print() const
{
    for (size_t i = 0; i < size(); i++)
    {
        cout << " Shard " << i+1 << " of " << size() << ":" << endl;
        (*this)[i].print();
        cout << endl;
    }
}


size_t ShardedPostingList::getSize()
{
    size_t size = 0;
    for (auto& s : *this) size += s.getSize();
    return size;
}


void Shard::print() const
{
    cout << "  start:" << this->sstart << "..end:" << this->send << endl;

    cout << "  impact list:" << endl;
    for (const auto& entry : this->impactList)
    {
        cout << "    t:" << entry.first << " -> i:" << entry.second << endl;
    }
        
    cout << "  record entries:" << endl;
    auto i = 0;
    for (const auto& entry : *this)
    {
        cout << "   [i:" << i++ << "]";
        entry.print(' ');
    }
}


size_t Shard::getSize()
{
    size_t size 
        // shard start and end
        = (2 * sizeof(Timestamp)) 
        // records
        + (this->size() * (sizeof(RecordId) + 2 * sizeof(Timestamp)))
        // impact lists
        + (this->impactList.size() * (sizeof(Timestamp) + sizeof(size_t)));

    return size;
}


void ShardedPostingList::addRecord(const RecordId rid, const Timestamp rstart, const Timestamp rend, const RecordId impact_list_gap, const Timestamp relaxation)
{
    // Algorithm for idealized sharding (η = 0) without wasted reads
    // Assumption: records-to-add are ordered by start

    // Remark (see Anand (2013), p. 40):
    // When using idealized sharding, a large number of shards can be generated
    // depending on the distribution of time intervals. The performance of 
    // query processing may suffer as each shard necessitates a costly 
    // random-seek operation. In some cases, it might be advantageous to 
    // decrease the number of shards produced even if in incurs wasted reads.

    // Ensure there is a shard with s.end <= r.end
    if (find_if(
            (*this).begin(), 
            (*this).end(), 
            [&rend,&rstart,&relaxation](const auto& s)
            {
                 return s.sstart_last < rstart && s.send < (rend + relaxation); 
            })
        == (*this).end())
    {
        auto s = Shard();
        s.sstart = rstart;
        s.sstart_last = rstart;
        s.send = rend;
        s.emplace_back(rid, rstart, rend);

        (*this).push_back(s);

        return;
    }

    // Find the shard with argmin(r.end - s.end), i.e., smallest gap
    auto shard_idx = 0;
    auto min_gap = numeric_limits<size_t>::max();
    for (size_t i = 0; i < this->size(); ++i)
    {
        const auto& s = (*this)[i];

        if (s.sstart_last > rstart) continue;

        if ((s.send - relaxation) > rend) continue;

        if (abs(rend - s.send) < min_gap)
        {
            min_gap = abs(rend - s.send);
            shard_idx = i;
        }
    }

    auto& shard = (*this)[shard_idx]; 

    // If there is a gap between old s.end r.start, add offset to impact list
    if (rstart > (shard.send))
    {
        shard.impactList[rstart] = shard.size(); 
    }
    
    if (impact_list_gap != 0 && shard.size() % impact_list_gap == 0)
    {
        // Only add an entry if it differs from the last
        auto new_timespamp = shard.send+1;
        auto create_entry = true;
        if (!shard.impactList.empty())
        {
            auto& last_timestamp = (--shard.impactList.end())->first;

            // Update last entry
            if (new_timespamp == last_timestamp)
            {
                (--shard.impactList.end())->second = shard.size();
                create_entry = false;
            }
        }

        if (create_entry) shard.impactList[new_timespamp] = shard.size(); 
    }

    shard.emplace_back(rid, rstart, rend);

    shard.sstart_last = rstart;
    shard.send = max(rend, shard.send);
}


void ShardedPostingList::getCandidates(const Timestamp& qstart, const Timestamp& qend, RelationId& candidates) const
{
    for (const auto& shard : *this)
    {
        shard.getCandidates(qstart, qend, candidates);
    }
}


void Shard::getCandidates(const Timestamp& qstart, const Timestamp& qend, RelationId& candidates) const
{
    // Query does not overlap with shard
    if (this->sstart > qend || this->send < qstart) return;

    // Get the offset from the impact list
    auto offset = 0;
    auto impact_iter = this->impactList.upper_bound(qstart);
    
    if (impact_iter != this->impactList.begin())
    {
        --impact_iter;
        offset = (*impact_iter).second;
    };

    auto shard_iter = this->begin() + offset;

    // Iterate over all entries starting at the offset
    for (;shard_iter != this->end(); ++shard_iter)
    {
        // Bineary-search was not benefical 
        // for Wikipedia[2020-2024) and ECOM.
        if (shard_iter->end < qstart) continue;

        // Bineary-search was not benefical 
        // for Wikipedia[2020-2024) and ECOM.
        if (shard_iter->start > qend) break;

        candidates.push_back(shard_iter->id);
    }
}


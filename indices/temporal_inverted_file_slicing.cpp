#include "temporal_inverted_files.h"



TemporalInvertedFile_Slicing::TemporalInvertedFile_Slicing()
{
}


TemporalInvertedFile_Slicing::TemporalInvertedFile_Slicing(const IRelation &R, const PartitionId numPartitions) : InvertedFileTemplate()
{
    unordered_map<TermId, Relation> tmplists;
    this->R = &R;

#ifdef CONSTRUCTION_TWO_PASSES
    TermId *lsizes = new TermId[R.dictionarySize]; // Counters

    // Step 1: one pass to count the contents inside each partition.
    memset(lsizes, 0, R.dictionarySize*sizeof(TermId));
    for (auto &r : R)
    {
        for (auto &tid : r.terms)
            lsizes[tid]++;
    }

    // Step 2: allocate necessary memory
    for (TermId tid = 0; tid < R.dictionarySize; tid++)
    {
        if (lsizes[tid] > 0)
            tmplists[tid].reserve(lsizes[tid]);
    }
    delete[] lsizes;
#endif

    // Step 3: fill temporary postings lists
    for (const auto &r : R)
    {
        for (const auto &tid : r.terms)
        {
            auto &list = tmplists[tid];

            if (list.gstart > r.start)
                list.gstart = r.start;
            if (list.gend < r.end)
                list.gend = r.end;
            list.emplace_back(r.id, r.start, r.end);
        }
    }
    
    // Step 4: construct grids
    for (auto iterL = tmplists.begin(); iterL != tmplists.end(); iterL++)
    {
//        iterL->second.gstart = R.gstart;
//        iterL->second.gend = R.gend;
        this->lists.insert(make_pair(iterL->first, new OneDimensionalGrid(iterL->second, numPartitions)));
    }
//        this->lists[iterL->first] = new OneDimensionalGrid(iterL->second, numPartitions);
//        cout<<"ok for "<<iterL->first<<endl;
//    TermId t = this->lists.begin()->first;
//    this->lists[t].print('r');
//    this->dlists[t]->print('r');
//    cout<<this->dlists.size()<<endl;

    tmplists.clear();
}


// Always two passes, no need otherwise
TemporalInvertedFile_Slicing::TemporalInvertedFile_Slicing(const IRelation &R, const IRelation &U, const TermId dictionarySize, const PartitionId numPartitions) : InvertedFileTemplate()
{
    unordered_map<TermId, Relation> tmplists;
    unordered_map<TermId, Relation> tmpUlists;
    this->R = &R;

    TermId *lsizes = new TermId[dictionarySize]; // Counters

    // Step 1: one pass to count the contents inside each partition.
    memset(lsizes, 0, dictionarySize*sizeof(TermId));
    for (auto &r : R)
    {
        for (auto &tid : r.terms)
            lsizes[tid]++;
    }

    // Step 2: allocate necessary memory
    for (TermId tid = 0; tid < dictionarySize; tid++)
    {
        if (lsizes[tid] > 0)
            tmplists[tid].reserve(lsizes[tid]);
    }
    delete[] lsizes;

    // Step 3: fill temporary postings lists
    for (const auto &r : R)
    {
        for (const auto &tid : r.terms)
        {
            auto &list = tmplists[tid];

            if (list.gstart > r.start)
                list.gstart = r.start;
            if (list.gend < r.end)
                list.gend = r.end;
            list.emplace_back(r.id, r.start, r.end);
        }
    }
    for (const auto &r : U)
    {
        for (const auto &tid : r.terms)
        {
            auto &list = tmpUlists[tid];

            if (list.gstart > r.start)
                list.gstart = r.start;
            if (list.gend < r.end)
                list.gend = r.end;
            list.emplace_back(r.id, r.start, r.end);
        }
    }
    
    // Step 4: construct grids
    for (auto iterL = tmplists.begin(); iterL != tmplists.end(); iterL++)
    {
        auto i = tmpUlists.find(iterL->first);

        if (i == tmpUlists.end())   // No updates will take place in this postings list
            this->lists.insert(make_pair(iterL->first, new OneDimensionalGrid(iterL->second, numPartitions)));
        else
            this->lists.insert(make_pair(iterL->first, new OneDimensionalGrid(iterL->second, i->second, numPartitions)));
    }
    
    for (auto iterL = tmpUlists.begin(); iterL != tmpUlists.end(); iterL++)
    {
        auto i = tmplists.find(iterL->first);
        if (i == tmplists.end())
            this->lists.insert(make_pair(iterL->first, new OneDimensionalGrid(Relation(), iterL->second, numPartitions)));
    }

    tmplists.clear();
    tmpUlists.clear();
}


void TemporalInvertedFile_Slicing::getStats()
{
//    size_t sum = 0;
//
//    this->numEmptyLists = this->R->dictionarySize - this->lists.size();
//    for (auto iter = this->lists.begin(); iter != this->lists.end(); iter++)
//        sum += iter->second.size();
//    this->avgListSize = (float)sum/this->lists.size();
}


size_t TemporalInvertedFile_Slicing::getSize()
{
    size_t size = 0;
    
    for (auto iterL = this->lists.begin(); iterL != this->lists.end(); iterL++)
        size += iterL->second->getSize();
    
    return size;
}


TemporalInvertedFile_Slicing::~TemporalInvertedFile_Slicing()
{
    for (auto iterL = this->lists.begin(); iterL != this->lists.end(); iterL++)
        delete iterL->second;
}


void TemporalInvertedFile_Slicing::print(char c)
{
//    for (auto iter = this->dlists.begin(); iter != this->dlists.end(); iter++)
//    {
//        cout << "t" << iter->first << " (" << iter->second.size() << "):";
//        for (auto &r : iter->second)
//            cout << " " << c << r.id;
//        cout << endl;
//    }
}



// Updating
void TemporalInvertedFile_Slicing::insert(IRelation &U)
{
    for (auto &r : U)
    {
        for (auto &tid : r.terms)
            this->lists[tid]->insert(Record(r.id, r.start, r.end));
    }
}



// Querying
bool TemporalInvertedFile_Slicing::moveOut_gOverlaps_checkBoth(const RangeIRQuery &q, RelationId &candidates)
{
    auto iterL = this->lists.find(q.terms[0]);
    
    // If the inverted file does not contain the term then result must be empty
    if (iterL == this->lists.end())
        return false;
    else
    {
        iterL->second->moveOut_checkBoth_gOverlaps(RangeQuery(q.id, q.start, q.end), candidates);
                
        return (!candidates.empty());
    }
}


bool TemporalInvertedFile_Slicing::intersect_gOverlaps(const RangeIRQuery &q, const unsigned int off, RelationId &candidates)
{
    auto iterL = this->lists.find(q.terms[off]);
    
    // If the inverted file does not contain the term then result must be empty
    if (iterL == this->lists.end())
        return false;
    else
    {
        iterL->second->interesect_gOverlaps(RangeQuery(q.id, q.start, q.end), candidates);
                
        return (!candidates.empty());
    }
}


bool TemporalInvertedFile_Slicing::intersectAndOutput_gOverlaps(const RangeIRQuery &q, const unsigned int off, RelationId &candidates, RelationId &result)
{
    auto iterL = this->lists.find(q.terms[off]);
    
    // If the inverted file does not contain the term then result must be empty
    if (iterL == this->lists.end())
        return false;
    else
    {
        iterL->second->interesectAndOutput_gOverlaps(RangeQuery(q.id, q.start, q.end), candidates, result);
                
        return (!result.empty());
    }
}


void TemporalInvertedFile_Slicing::executeContainment(const RangeIRQuery &q, RelationId &result)
{
#ifdef PROFILING
    Timer tim;
    double secsMO = 0, secsS = 0, secsI = 0, secsIO = 0;
    cout<<endl;
#endif
    
    RelationId candidates;
    auto numQueryTerms = q.terms.size();
    
    if (numQueryTerms == 1)
        this->moveOut_gOverlaps_checkBoth(q, result);
    else
    {
#ifdef PROFILING
        tim.start();
#endif
        if (!this->moveOut_gOverlaps_checkBoth(q, candidates))
            return;
#ifdef PROFILING
        secsMO += tim.stop();
        cout<<"q" << q.id << ": t = " << q.terms[0] << "\t|C| = " << candidates.size()<<endl;
#endif

        for (auto i = 1; i < numQueryTerms-1; i++)
        {
#ifdef PROFILING
            tim.start();
#endif
            sort(candidates.begin(), candidates.end());
#ifdef PROFILING
            secsS += tim.stop();
            tim.start();
#endif
            if (!this->intersect_gOverlaps(q, i, candidates))
                return;
#ifdef PROFILING
            secsI += tim.stop();
            cout<<"q" << q.id << ": t = " << q.terms[i] << "\t|C| = " << candidates.size()<<endl;
#endif
        }
        
#ifdef PROFILING
        tim.start();
#endif
        sort(candidates.begin(), candidates.end());
#ifdef PROFILING
        secsS += tim.stop();
        tim.start();
#endif
        this->intersectAndOutput_gOverlaps(q, numQueryTerms-1, candidates, result);
#ifdef PROFILING
        secsIO += tim.stop();
        
        printf("moveOut          : %f secs\n", secsMO);
        printf("sort             : %f secs\n", secsS);
        printf("intersect        : %f secs\n", secsI);
        printf("intersect and out: %f secs\n", secsIO);
#endif
    }
}

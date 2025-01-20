#include "temporal_inverted_files.h"



TemporalInvertedFile::TemporalInvertedFile() : InvertedFileTemplate()
{
}


// Undecided if we should use two passes
TemporalInvertedFile::TemporalInvertedFile(const IRelation &R) : InvertedFileTemplate()
{
    this->R = &R;
    this->numIndexedRecords = R.size();

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
            this->lists[tid].reserve(lsizes[tid]);
    }
    delete[] lsizes;
#endif

    // Step 3: fill postings lists
    for (auto &r : R)
        this->index(r);
}


// Has to be always two passes
TemporalInvertedFile::TemporalInvertedFile(const IRelation &R, const IRelation &U, const TermId dictionarySize) : InvertedFileTemplate()
{
    this->R = &R;
    this->numIndexedRecords = R.size();
    
    TermId *lsizes = new TermId[dictionarySize]; // Counters

    // Step 1: one pass to count the contents inside each partition.
    memset(lsizes, 0, dictionarySize*sizeof(TermId));
    for (auto &r : R)
    {
        for (auto &tid : r.terms)
            lsizes[tid]++;
    }
    for (auto &r : U)
    {
        for (auto &tid : r.terms)
            lsizes[tid]++;
    }

    // Step 2: allocate necessary memory
    for (TermId tid = 0; tid < dictionarySize; tid++)
    {
        if (lsizes[tid] > 0)
            this->lists[tid].reserve(lsizes[tid]);
    }
    delete[] lsizes;

    // Step 3: fill postings lists
    for (auto &r : R)
        this->index(r);
}


void TemporalInvertedFile::index(const IRecord &r)
{
    for (auto &tid : r.terms)
    {
        Relation &list = this->lists[tid];
        list.emplace_back(r.id, r.start, r.end);
        if (list.gstart > r.start)
            list.gstart = r.start;
        if (list.gend < r.end)
            list.gend = r.end;
    }
}


void TemporalInvertedFile::getStats()
{
    size_t sum = 0;

    this->numEmptyLists = this->R->dictionarySize - this->lists.size();
    for (auto iter = this->lists.begin(); iter != this->lists.end(); iter++)
        sum += iter->second.size();
    this->avgListSize = (float)sum/this->lists.size();
}


size_t TemporalInvertedFile::getSize()
{
    size_t size = 0;
    
    for (auto iter = this->lists.begin(); iter != this->lists.end(); iter++)
        size += iter->second.size() * sizeof(Record);
    
    return size;
}


TemporalInvertedFile::~TemporalInvertedFile()
{
}


void TemporalInvertedFile::print(char c)
{
    for (auto iter = this->lists.begin(); iter != this->lists.end(); iter++)
    {
        cout << "t" << iter->first << " (" << iter->second.size() << "):";
        for (auto &r : iter->second)
            cout << " " << c << r.id;
        cout << endl;
    }
}



// Updating
void TemporalInvertedFile::insert(IRelation &U)
{
    for (auto &r : U)
        this->index(r);
}



// Querying
bool TemporalInvertedFile::moveOut_CheckBoth_gOverlaps(const RangeIRQuery &q, RelationId &candidates)
{
    unordered_map<TermId, Relation>::iterator iterE = this->lists.find(q.terms[0]);
    
    // If the inverted file does not contain the term then result must be empty
    if (iterE == this->lists.end())
        return false;
    else
    {
        for (const Record &r: iterE->second)
        {
            if ((r.start <= q.end) && (q.start <= r.end))
                candidates.push_back(r.id);
        }
        
        return (!candidates.empty());
    }
}


bool TemporalInvertedFile::moveOut_CheckStart_gOverlaps(const RangeIRQuery &q, RelationId &candidates)
{
    unordered_map<TermId, Relation>::iterator iterE = this->lists.find(q.terms[0]);
    
    // If the inverted file does not contain the term then result must be empty
    if (iterE == this->lists.end())
        return false;
    else
    {
        for (const Record &r: iterE->second)
        {
            if (r.start <= q.end)
                candidates.push_back(r.id);
        }
        
        return (!candidates.empty());
    }
}


bool TemporalInvertedFile::moveOut_CheckEnd_gOverlaps(const RangeIRQuery &q, RelationId &candidates)
{
    unordered_map<TermId, Relation>::iterator iterE = this->lists.find(q.terms[0]);
    
    // If the inverted file does not contain the term then result must be empty
    if (iterE == this->lists.end())
        return false;
    else
    {
        for (const Record &r: iterE->second)
        {
            if (q.start <= r.end)
            {
                candidates.push_back(r.id);
            }
        }
        
        return (!candidates.empty());
    }
}


bool TemporalInvertedFile::moveOut_NoChecks_gOverlaps(const RangeIRQuery &q, RelationId &candidates)
{
    unordered_map<TermId, Relation>::iterator iterE = this->lists.find(q.terms[0]);
    
    // If the inverted file does not contain the term then result must be empty
    if (iterE == this->lists.end())
        return false;
    else
    {
        for (const Record &r: iterE->second)
            candidates.push_back(r.id);

        return (!candidates.empty());
    }
}


bool TemporalInvertedFile::intersect_gOverlaps(const RangeIRQuery &q, const unsigned int off, RelationId &candidates)
{
    auto iterE = this->lists.find(q.terms[off]);
    auto counter = 0;
    
    
    if (iterE == this->lists.end())
        return false;
    else
    {
        auto iterC    = candidates.begin();
        auto iterCEnd = candidates.end();
        auto iterL    = iterE->second.begin();
        auto iterLEnd = iterE->second.end();
        RelationId tmp;

        tmp.reserve(candidates.size());
        while ((iterC != iterCEnd) && (iterL != iterLEnd))
        {
            if (iterL->id < *iterC)
                iterL++;
            else if (iterL->id > *iterC)
                iterC++;
            else
            {
                tmp.push_back(iterL->id);
                iterL++;
            }
        }
        candidates.swap(tmp);
        
        return (!candidates.empty());
    }
}


bool TemporalInvertedFile::intersectAndOutput_gOverlaps(const RangeIRQuery &q, const unsigned int off, RelationId &candidates, RelationId &result)
{
    auto iterE = this->lists.find(q.terms[off]);
    auto counter = 0;
    
    
    if (iterE == this->lists.end())
        return false;
    else
    {
        auto iterC    = candidates.begin();
        auto iterCEnd = candidates.end();
        auto iterL    = iterE->second.begin();
        auto iterLEnd = iterE->second.end();

        while ((iterC != iterCEnd) && (iterL != iterLEnd))
        {
            if (iterL->id < *iterC)
                iterL++;
            else if (iterL->id > *iterC)
                iterC++;
            else
            {
                result.push_back(iterL->id);
                iterL++;
            }
        }
        
        return (!result.empty());
    }
}


void TemporalInvertedFile::executeContainment(const RangeIRQuery &q, RelationId &result)
{
    RelationId candidates;
    auto numQueryTerms = q.terms.size();
//    auto rsize = this-R->getFrequency(q.terms[0]);
    
//    candidates.reserve(rsize);
    if (numQueryTerms == 1)
        this->moveOut_CheckBoth_gOverlaps(q, result);
    else
    {
        if (!this->moveOut_CheckBoth_gOverlaps(q, candidates))
            return;
        
        for (auto i = 1; i < numQueryTerms-1; i++)
        {
            if (!this->intersect_gOverlaps(q, i, candidates))
                return;
        }
        
        this->intersectAndOutput_gOverlaps(q, numQueryTerms-1, candidates, result);
    }
}


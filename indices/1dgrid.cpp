#include "1dgrid.h"


//#define INTERSECT_WITH_BINARYSEARCH
//#define INTERSECT_WITH_BINARYSEARCH_AND_TEMPORALCHECKS
#define INTERSECT_WITH_MERGESORT

//#define INTERSECT_WITH_SHRINKING


inline void OneDimensionalGrid::updateCounters(const Record &r)
{
    auto s_pid = (r.start == this->gend)? this->numPartitionsMinus1: (max(r.start,gstart)-gstart)/this->partitionExtent;
    auto e_pid = (r.end   == this->gend)? this->numPartitionsMinus1: (min(r.end,gend)-gstart)/this->partitionExtent;
//    auto s_pid = (r.start == this->gend) ? this->numPartitionsMinus1 : (r.start-this->gstart)/this->partitionExtent;
//    auto e_pid = (r.end   == this->gend) ? this->numPartitionsMinus1 : (r.end-this->gstart)/this->partitionExtent;
    
    this->pRecs_sizes[s_pid]++;
    while (s_pid != e_pid)
    {
        s_pid++;
        this->pRecs_sizes[s_pid]++;
    }
}


inline void OneDimensionalGrid::updatePartitions(const Record &r)
{
    auto s_pid = (r.start == this->gend)? this->numPartitionsMinus1: (max(r.start,gstart)-gstart)/this->partitionExtent;
    auto e_pid = (r.end   == this->gend)? this->numPartitionsMinus1: (min(r.end,gend)-gstart)/this->partitionExtent;
//    auto s_pid = (r.start == this->gend) ? this->numPartitionsMinus1 : (r.start-this->gstart)/this->partitionExtent;
//    auto e_pid = (r.end   == this->gend) ? this->numPartitionsMinus1 : (r.end-this->gstart)/this->partitionExtent;

//    this->pRecs[s_pid][this->pRecs_sizes[s_pid]] = r;
//    this->pRecs_sizes[s_pid]++;
    this->pRecs[s_pid].push_back(r);
    while (s_pid != e_pid)
    {
        s_pid++;
//        this->pRecs[s_pid][this->pRecs_sizes[s_pid]] = r;
//        this->pRecs_sizes[s_pid]++;
        this->pRecs[s_pid].push_back(r);
    }
}


OneDimensionalGrid::OneDimensionalGrid()
{
    
}


OneDimensionalGrid::OneDimensionalGrid(const Relation &R, const PartitionId numPartitions)
{
    // Initialize statistics.
    this->numIndexedRecords   = R.size();
    this->numPartitions       = numPartitions;
    this->numPartitionsMinus1 = this->numPartitions-1;
    this->numEmptyPartitions  = 0;
    this->avgPartitionSize    = 0;
    this->gstart              = R.gstart;
    this->gend                = R.gend;
    this->partitionExtent     = (Timestamp)ceil((double)(this->gend-this->gstart)/this->numPartitions);
    this->numReplicas = 0;
    

    // Step 1: one pass to count the contents inside each partition.
    this->pRecs_sizes = (size_t*)calloc(this->numPartitions, sizeof(size_t));

    for (const Record &r : R)
        this->updateCounters(r);

    // Step 2: allocate necessary memory.
    this->pRecs = new Relation[this->numPartitions];
    for (auto pId = 0; pId < this->numPartitions; pId++)
    {
        this->pRecs[pId].gstart = this->gstart            + pId*this->partitionExtent;
        this->pRecs[pId].gend   = this->pRecs[pId].gstart + this->partitionExtent;
//        this->pRecs[pId].resize(this->pRecs_sizes[pId]);
        this->pRecs[pId].reserve(this->pRecs_sizes[pId]);
    }
//    memset(this->pRecs_sizes, 0, this->numPartitions*sizeof(size_t));

    // Step 3: fill partitions.
    for (const Record &r : R)
        this->updatePartitions(r);

    // Free auxiliary memory.
    free(pRecs_sizes);
}


OneDimensionalGrid::OneDimensionalGrid(const Relation &R, const Relation &U, const PartitionId numPartitions)
{
    // Initialize statistics.
    this->numIndexedRecords   = R.size();
    this->numPartitions       = numPartitions;
    this->numPartitionsMinus1 = this->numPartitions-1;
    this->numEmptyPartitions  = 0;
    this->avgPartitionSize    = 0;
    this->gstart              = min(R.gstart, U.gstart);
    this->gend                = max(R.gend, U.gend);
    this->partitionExtent     = (Timestamp)ceil((double)(this->gend-this->gstart)/this->numPartitions);
    this->numReplicas = 0;
    

    // Step 1: one pass to count the contents inside each partition.
    this->pRecs_sizes = (size_t*)calloc(this->numPartitions, sizeof(size_t));

    for (const Record &r : R)
        this->updateCounters(r);
    for (const Record &r : U)
        this->updateCounters(r);

    // Step 2: allocate necessary memory.
    this->pRecs = new Relation[this->numPartitions];
    for (auto pId = 0; pId < this->numPartitions; pId++)
    {
        this->pRecs[pId].gstart = this->gstart            + pId*this->partitionExtent;
        this->pRecs[pId].gend   = this->pRecs[pId].gstart + this->partitionExtent;
//        this->pRecs[pId].resize(this->pRecs_sizes[pId]);
        this->pRecs[pId].reserve(this->pRecs_sizes[pId]);
    }
//    memset(this->pRecs_sizes, 0, this->numPartitions*sizeof(size_t));

    // Step 3: fill partitions.
    for (const Record &r : R)
        this->updatePartitions(r);

    // Free auxiliary memory.
    free(pRecs_sizes);
}


void OneDimensionalGrid::print(char c)
{
    for (auto pId = 0; pId < this->numPartitions; pId++)
    {
        Relation &p = this->pRecs[pId];
        
        cout << "Partition " << pId << " [" << p.gstart << ".." << p.gend << "] (" << p.size() << "):";
        for (size_t i = 0; i < p.size(); i++)
            cout << " r" << p[i].id;
        cout << endl;
    }
}


OneDimensionalGrid::~OneDimensionalGrid()
{
    delete[] this->pRecs;
}


void OneDimensionalGrid::getStats()
{
    for (auto pId = 0; pId < this->numPartitions; pId++)
    {
        this->numReplicas += this->pRecs[pId].size();

        if (this->pRecs[pId].empty())
            this->numEmptyPartitions++;
    }
    
    this->avgPartitionSize = (float)(this->numReplicas)/this->numPartitions;
    this->numReplicas -= this->numIndexedRecords;
}


size_t OneDimensionalGrid::getSize()
{
    size_t size = 0;
    
    for (auto pId = 0; pId < this->numPartitions; pId++)
        size += this->pRecs[pId].size() * (sizeof(Record));
    
    return size;
}



// Updating
void OneDimensionalGrid::insert(const Record &r)
{
    auto s_pid = (r.start == this->gend)? this->numPartitionsMinus1: (max(r.start,gstart)-gstart)/this->partitionExtent;
    auto e_pid = (r.end   == this->gend)? this->numPartitionsMinus1: (min(r.end,gend)-gstart)/this->partitionExtent;
//    auto s_pid = (r.start == this->gend) ? this->numPartitionsMinus1 : (r.start-this->gstart)/this->partitionExtent;
//    auto e_pid = (r.end   == this->gend) ? this->numPartitionsMinus1 : (r.end-this->gstart)/this->partitionExtent;
//    cout<<"\t\ts_pid = " << s_pid<<endl;
//    cout<<"\t\te_pid = " << e_pid<<endl;

    this->pRecs[s_pid].push_back(r);
    while (s_pid != e_pid)
    {
        s_pid++;
        this->pRecs[s_pid].push_back(r);
    }
}



// Querying
void OneDimensionalGrid::moveOut_checkBoth_gOverlaps(const RangeQuery &q, RelationId &candidates)
{
    RelationIterator iter, iterBegin, iterEnd;
    auto s_pid = (max(q.start,gstart) == this->gend)? this->numPartitionsMinus1: (max(q.start,gstart)-gstart)/this->partitionExtent;
    auto e_pid = (min(q.end,gend)     == this->gend)? this->numPartitionsMinus1: (min(q.end,gend)-gstart)/this->partitionExtent;
    
//    cout<<this->gstart<<endl;
//    cout<<this->gend<<endl;
//    cout<<q.start<<endl;
//    cout<<q.end<<endl;
//    cout << "s_pid = "<<s_pid<<"\te_pid = "<<e_pid<<endl;
    //    this->pRecs[s_pid].print('r');
    // Handle the first partition.
    iterBegin = this->pRecs[s_pid].begin();
    iterEnd   = this->pRecs[s_pid].end();
//    cout<<"|P["<<s_pid<<"]| = " << this->pRecs[s_pid].size()<<endl;
    for (RelationIterator iter = iterBegin; iter != iterEnd; iter++)
    {
        if ((iter->start <= q.end) && (q.start <= iter->end))
            candidates.push_back(iter->id);
    }
//    cout<<s_pid << " ok"<<endl;

    // Handle partitions completely contained inside the query range.
    for (auto pid = s_pid+1; pid < e_pid; pid++)
    {
        Relation &p = this->pRecs[pid];
        iterBegin = p.begin();
        iterEnd = p.end();
        for (iter = p.begin(); iter != iterEnd; iter++)
        {
            // Perform de-duplication test.
//            if (max(Q.start, iter->start) >= p.gstart)
            if (iter->start >= p.gstart)
                candidates.push_back(iter->id);
        }
//        cout<<pid << " ok"<<endl;
    }

    // Handle the last partition.
    if (e_pid != s_pid)
    {
        iterBegin = this->pRecs[e_pid].begin();
        iterEnd = this->pRecs[e_pid].end();
        for (iter = iterBegin; iter != iterEnd; iter++)
        {
//            if ((max(q.start, iter->start) >= this->pRecs[e_pId].gstart) && (iter->start <= q.end && q.start <= iter->end))
            if ((iter->start >= this->pRecs[e_pid].gstart) && (iter->start <= q.end) && (q.start <= iter->end))
                candidates.push_back(iter->id);
        }
    }
//    cout<<e_pid << " ok"<<endl;
}


inline void mergeSort_withoutDeduplication(Relation &partition, RelationId &candidates, RelationId &result)
{
    auto iterP    = partition.begin();
    auto iterPEnd = partition.end();
    auto iterC    = candidates.begin();
    auto iterCEnd = candidates.end();

//    tmp.reserve(candidates.size());
#ifdef INTERSECT_WITH_SHRINKING
    while ((iterP != iterPEnd) && (iterC != candidates.end()))
#else
    while ((iterP != iterPEnd) && (iterC != iterCEnd))
#endif
    {
        if (iterP->id < *iterC)
            iterP++;
        else if (iterP->id > *iterC)
            iterC++;
        else
        {
            result.push_back(iterP->id);
            iterP++;
            
#ifdef INTERSECT_WITH_SHRINKING
            iterC = candidates.erase(iterC);
#else
            iterC++;
#endif
        }
    }
//    candidates.swap(tmp);
}

inline void mergeSort_withDeduplication(Relation &partition, RelationId &candidates, RelationId &result)
{
    auto iterP    = partition.begin();
    auto iterPEnd = partition.end();
    auto iterC    = candidates.begin();
    auto iterCEnd = candidates.end();

//    tmp.reserve(candidates.size());
#ifdef INTERSECT_WITH_SHRINKING
    while ((iterP != iterPEnd) && (iterC != candidates.end()))
#else
    while ((iterP != iterPEnd) && (iterC != iterCEnd))
#endif
    {
        if (iterP->id < *iterC)
            iterP++;
        else if (iterP->id > *iterC)
            iterC++;
        else
        {
#ifdef INTERSECT_WITH_SHRINKING
            result.push_back(iterP->id);
            iterP++;
            iterC = candidates.erase(iterC);
#else
            if (iterP->start >= partition.gstart)
                result.push_back(iterP->id);
            iterP++;
            iterC++;
#endif
        }
    }
//    candidates.swap(tmp);
}


//bool myfunction (const Record &i, const Record &j)
//{
//    return (i.id < j.id);
//}


void OneDimensionalGrid::interesect_gOverlaps(const RangeQuery &q, RelationId &candidates)
{
    RelationId result;
    auto s_pid = (max(q.start,gstart) == this->gend)? this->numPartitionsMinus1: (max(q.start,gstart)-gstart)/this->partitionExtent;
    auto e_pid = (min(q.end,gend)     == this->gend)? this->numPartitionsMinus1: (min(q.end,gend)-gstart)/this->partitionExtent;


//    cout << "s_pid = "<<s_pid<<"\te_pid = "<<e_pid<<endl;
#if defined(INTERSECT_WITH_BINARYSEARCH)
    // Alternative version
#ifdef INTERSECT_WITH_SHRINKING
    for (auto iterC = candidates.begin(); iterC != candidates.end(); iterC++)
    {
        for (auto pid = s_pid; pid <= e_pid; pid++)
        {
            if (binary_search(this->pRecs[pid].begin(), this->pRecs[pid].end(), Record(*iterC,0,0), compareRecordsById))
//            if (binary_search(this->pRecs[pid].begin(), this->pRecs[pid].end(), cid))
            {
                result.push_back(cid);
                break;
            }
        }
    }
#else
    for (auto &cid : candidates)
    {
        for (auto pid = s_pid; pid <= e_pid; pid++)
        {
            if (binary_search(this->pRecs[pid].begin(), this->pRecs[pid].end(), Record(cid,0,0), compareRecordsById))
//            if (binary_search(this->pRecs[pid].begin(), this->pRecs[pid].end(), cid))
            {
                result.push_back(cid);
                break;
            }
        }
    }
#endif
#elif defined(INTERSECT_WITH_BINARYSEARCH_AND_TEMPORALCHECKS)
    RelationIterator iter, iterBegin, iterEnd;

    // Handle the first partition.
    iterBegin = this->pRecs[s_pid].begin();
    iterEnd   = this->pRecs[s_pid].end();
//    cout<<"|P["<<s_pid<<"]| = " << this->pRecs[s_pid].size()<<endl;
    for (RelationIterator iter = iterBegin; iter != iterEnd; iter++)
    {
        if ((iter->start <= q.end) && (q.start <= iter->end) && (binary_search(candidates.begin(), candidates.end(), iter->id)))
            result.push_back(iter->id);
    }
//    cout<<s_pid << " ok"<<endl;

    // Handle partitions completely contained inside the query range.
    for (auto pid = s_pid+1; pid < e_pid; pid++)
    {
        Relation &p = this->pRecs[pid];
        iterBegin = p.begin();
        iterEnd = p.end();
        for (iter = p.begin(); iter != iterEnd; iter++)
        {
            // Perform de-duplication test.
//            if (max(Q.start, iter->start) >= p.gstart)
            if ((iter->start >= p.gstart) && (binary_search(candidates.begin(), candidates.end(), iter->id)))
                result.push_back(iter->id);
        }
//        cout<<pid << " ok"<<endl;
    }

    // Handle the last partition.
    if (e_pid != s_pid)
    {
        iterBegin = this->pRecs[e_pid].begin();
        iterEnd = this->pRecs[e_pid].end();
        for (iter = iterBegin; iter != iterEnd; iter++)
        {
//            if ((max(q.start, iter->start) >= this->pRecs[e_pId].gstart) && (iter->start <= q.end && q.start <= iter->end))
            if ((iter->start >= this->pRecs[e_pid].gstart) && (iter->start <= q.end) && (q.start <= iter->end) && (binary_search(candidates.begin(), candidates.end(), iter->id)))
                result.push_back(iter->id);
        }
    }
#elif defined(INTERSECT_WITH_MERGESORT)
    mergeSort_withoutDeduplication(this->pRecs[s_pid], candidates, result);
#ifdef PROFILING
    cout << "\t|C| = " << candidates.size() << "\tINTERSECT |P[" << s_pid << "]| = " << this->pRecs[s_pid].size() << endl;
#endif

    for (auto pid = s_pid+1; pid <= e_pid; pid++)
    {
        mergeSort_withDeduplication(this->pRecs[pid], candidates, result);
#ifdef PROFILING
        cout << "\t|C| = " << candidates.size() << "\tINTERSECT |P[" << pid << "]| = " << this->pRecs[s_pid].size() << endl;
#endif
    }
#endif

    candidates.swap(result);
}


void OneDimensionalGrid::interesectAndOutput_gOverlaps(const RangeQuery &q, RelationId &candidates, RelationId &result)
{
    auto s_pid = (max(q.start,gstart) == this->gend)? this->numPartitionsMinus1: (max(q.start,gstart)-gstart)/this->partitionExtent;
    auto e_pid = (min(q.end,gend)     == this->gend)? this->numPartitionsMinus1: (min(q.end,gend)-gstart)/this->partitionExtent;


//    cout << "s_pid = "<<s_pid<<"\te_pid = "<<e_pid<<endl;
#if defined(INTERSECT_WITH_BINARYSEARCH)
    // Alternative version
    for (auto &cid : candidates)
    {
        for (auto pid = s_pid; pid <= e_pid; pid++)
        {
            if (binary_search(this->pRecs[pid].begin(), this->pRecs[pid].end(), Record(cid,0,0), myfunction))
//            if (binary_search(this->pRecs[pid].begin(), this->pRecs[pid].end(), cid))
            {
                result.push_back(cid);
                break;
            }
        }
    }
#elif defined(INTERSECT_WITH_BINARYSEARCH_AND_TEMPORALCHECKS)
    RelationIterator iter, iterBegin, iterEnd;

    // Handle the first partition.
    iterBegin = this->pRecs[s_pid].begin();
    iterEnd   = this->pRecs[s_pid].end();
//    cout<<"|P["<<s_pid<<"]| = " << this->pRecs[s_pid].size()<<endl;
    for (RelationIterator iter = iterBegin; iter != iterEnd; iter++)
    {
        if ((iter->start <= q.end) && (q.start <= iter->end) && (binary_search(candidates.begin(), candidates.end(), iter->id)))
            result.push_back(iter->id);
    }
//    cout<<s_pid << " ok"<<endl;

    // Handle partitions completely contained inside the query range.
    for (auto pid = s_pid+1; pid < e_pid; pid++)
    {
        Relation &p = this->pRecs[pid];
        iterBegin = p.begin();
        iterEnd = p.end();
        for (iter = p.begin(); iter != iterEnd; iter++)
        {
            // Perform de-duplication test.
//            if (max(Q.start, iter->start) >= p.gstart)
            if ((iter->start >= p.gstart) && (binary_search(candidates.begin(), candidates.end(), iter->id)))
                result.push_back(iter->id);
        }
//        cout<<pid << " ok"<<endl;
    }

    // Handle the last partition.
    if (e_pid != s_pid)
    {
        iterBegin = this->pRecs[e_pid].begin();
        iterEnd = this->pRecs[e_pid].end();
        for (iter = iterBegin; iter != iterEnd; iter++)
        {
//            if ((max(q.start, iter->start) >= this->pRecs[e_pId].gstart) && (iter->start <= q.end && q.start <= iter->end))
            if ((iter->start >= this->pRecs[e_pid].gstart) && (iter->start <= q.end) && (q.start <= iter->end) && (binary_search(candidates.begin(), candidates.end(), iter->id)))
                result.push_back(iter->id);
        }
    }
#elif defined(INTERSECT_WITH_MERGESORT)
    mergeSort_withoutDeduplication(this->pRecs[s_pid], candidates, result);
#ifdef PROFILING
    cout << "\t|C| = " << candidates.size() << "\tINTERSECT |P[" << s_pid << "]| = " << this->pRecs[s_pid].size() << endl;
#endif

    for (auto pid = s_pid+1; pid <= e_pid; pid++)
    {
        mergeSort_withDeduplication(this->pRecs[pid], candidates, result);
#ifdef PROFILING
        cout << "\t|C| = " << candidates.size() << "\tINTERSECT |P[" << pid << "]| = " << this->pRecs[s_pid].size() << endl;
#endif
    }
#endif
}



inline void OneDimensionalGrid_RecordStart::updateCounters(const Record &r)
{
    auto s_pid = (r.start == this->gend)? this->numPartitionsMinus1: (max(r.start,gstart)-gstart)/this->partitionExtent;
    auto e_pid = (r.end   == this->gend)? this->numPartitionsMinus1: (min(r.end,gend)-gstart)/this->partitionExtent;
//    auto s_pid = (r.start == this->gend) ? this->numPartitionsMinus1 : (r.start-this->gstart)/this->partitionExtent;
//    auto e_pid = (r.end   == this->gend) ? this->numPartitionsMinus1 : (r.end-this->gstart)/this->partitionExtent;
    
    this->pRecs_sizes[s_pid]++;
    while (s_pid != e_pid)
    {
        s_pid++;
        this->pRecs_sizes[s_pid]++;
    }
}


inline void OneDimensionalGrid_RecordStart::updatePartitions(const Record &r)
{
    auto s_pid = (r.start == this->gend)? this->numPartitionsMinus1: (max(r.start,gstart)-gstart)/this->partitionExtent;
    auto e_pid = (r.end   == this->gend)? this->numPartitionsMinus1: (min(r.end,gend)-gstart)/this->partitionExtent;
//    auto s_pid = (r.start == this->gend) ? this->numPartitionsMinus1 : (r.start-this->gstart)/this->partitionExtent;
//    auto e_pid = (r.end   == this->gend) ? this->numPartitionsMinus1 : (r.end-this->gstart)/this->partitionExtent;

    this->pRecs[s_pid].emplace_back(r.id, r.start);
//    this->pRecs[s_pid][this->pRecs_sizes[s_pid]] = RecordStart(r.id, r.start);
//    this->pRecs_sizes[s_pid]++;
    while (s_pid != e_pid)
    {
        s_pid++;
        this->pRecs[s_pid].emplace_back(r.id, r.start);
//        this->pRecs[s_pid][this->pRecs_sizes[s_pid]] = RecordStart(r.id, r.start);
//        this->pRecs_sizes[s_pid]++;
    }
}


OneDimensionalGrid_RecordStart::OneDimensionalGrid_RecordStart()
{
    
}


OneDimensionalGrid_RecordStart::OneDimensionalGrid_RecordStart(const Relation &R, const PartitionId numPartitions)
{
    // Initialize statistics.
    this->numIndexedRecords   = R.size();
    this->numPartitions       = numPartitions;
    this->numPartitionsMinus1 = this->numPartitions-1;
    this->numEmptyPartitions  = 0;
    this->avgPartitionSize    = 0;
    this->gstart              = R.gstart;
    this->gend                = R.gend;
    this->partitionExtent     = (Timestamp)ceil((double)(this->gend-this->gstart)/this->numPartitions);
    this->numReplicas = 0;
    

    // Step 1: one pass to count the contents inside each partition.
    this->pRecs_sizes = (size_t*)calloc(this->numPartitions, sizeof(size_t));

    for (const Record &r : R)
        this->updateCounters(r);

    // Step 2: allocate necessary memory.
    this->pRecs = new RelationStart[this->numPartitions];
    for (auto pId = 0; pId < this->numPartitions; pId++)
    {
        this->pRecs[pId].gstart = this->gstart            + pId*this->partitionExtent;
//        this->pRecs[pId].gend   = this->pRecs[pId].gstart + this->partitionExtent;
        this->pRecs[pId].reserve(this->pRecs_sizes[pId]);
    }
//    memset(this->pRecs_sizes, 0, this->numPartitions*sizeof(size_t));

    // Step 3: fill partitions.
    for (const Record &r : R)
        this->updatePartitions(r);

    // Free auxiliary memory.
    free(pRecs_sizes);
}


OneDimensionalGrid_RecordStart::OneDimensionalGrid_RecordStart(const Relation &R, const Relation &U, const PartitionId numPartitions)
{
    // Initialize statistics.
    this->numIndexedRecords   = R.size();
    this->numPartitions       = numPartitions;
    this->numPartitionsMinus1 = this->numPartitions-1;
    this->numEmptyPartitions  = 0;
    this->avgPartitionSize    = 0;
    this->gstart              = min(R.gstart, U.gstart);
    this->gend                = max(R.gend, U.gend);
    this->partitionExtent     = (Timestamp)ceil((double)(this->gend-this->gstart)/this->numPartitions);
    this->numReplicas = 0;
    

    // Step 1: one pass to count the contents inside each partition.
    this->pRecs_sizes = (size_t*)calloc(this->numPartitions, sizeof(size_t));

    for (const Record &r : R)
        this->updateCounters(r);
    for (const Record &r : U)
        this->updateCounters(r);

    // Step 2: allocate necessary memory.
    this->pRecs = new RelationStart[this->numPartitions];
    for (auto pId = 0; pId < this->numPartitions; pId++)
    {
        this->pRecs[pId].gstart = this->gstart            + pId*this->partitionExtent;
//        this->pRecs[pId].gend   = this->pRecs[pId].gstart + this->partitionExtent;
        this->pRecs[pId].reserve(this->pRecs_sizes[pId]);
    }
//    memset(this->pRecs_sizes, 0, this->numPartitions*sizeof(size_t));

    // Step 3: fill partitions.
    for (const Record &r : R)
        this->updatePartitions(r);

    // Free auxiliary memory.
    free(pRecs_sizes);
}


void OneDimensionalGrid_RecordStart::print(char c)
{
    for (auto pId = 0; pId < this->numPartitions; pId++)
    {
        RelationStart &p = this->pRecs[pId];
        
        cout << "Partition " << pId << " " << p.gstart << " (" << p.size() << "):";
        for (size_t i = 0; i < p.size(); i++)
            cout << " r" << p[i].id;
        cout << endl;
    }
}


OneDimensionalGrid_RecordStart::~OneDimensionalGrid_RecordStart()
{
    delete[] this->pRecs;
}


void OneDimensionalGrid_RecordStart::getStats()
{
    for (auto pId = 0; pId < this->numPartitions; pId++)
    {
        this->numReplicas += this->pRecs[pId].size();

        if (this->pRecs[pId].empty())
            this->numEmptyPartitions++;
    }
    
    this->avgPartitionSize = (float)(this->numReplicas)/this->numPartitions;
    this->numReplicas -= this->numIndexedRecords;
}


size_t OneDimensionalGrid_RecordStart::getSize()
{
    size_t size = 0;
    
    for (auto pId = 0; pId < this->numPartitions; pId++)
        size += this->pRecs[pId].size() * (sizeof(RecordStart));
    
    return size;
}



// Updating
void OneDimensionalGrid_RecordStart::insert(const Record &r)
{
    auto s_pid = (r.start == this->gend)? this->numPartitionsMinus1: (max(r.start,gstart)-gstart)/this->partitionExtent;
    auto e_pid = (r.end   == this->gend)? this->numPartitionsMinus1: (min(r.end,gend)-gstart)/this->partitionExtent;
//    auto s_pid = (r.start == this->gend) ? this->numPartitionsMinus1 : (r.start-this->gstart)/this->partitionExtent;
//    auto e_pid = (r.end   == this->gend) ? this->numPartitionsMinus1 : (r.end-this->gstart)/this->partitionExtent;
//    cout<<"\t\ts_pid = " << s_pid<<endl;
//    cout<<"\t\te_pid = " << e_pid<<endl;

    this->pRecs[s_pid].emplace_back(r.id, r.start);
    while (s_pid != e_pid)
    {
        s_pid++;
        this->pRecs[s_pid].emplace_back(r.id, r.start);
    }
}



// Querying
inline void mergeSort_withoutDeduplication(RelationStart &partition, RelationId &candidates, RelationId &result)
{
    auto iterP    = partition.begin();
    auto iterPEnd = partition.end();
    auto iterC    = candidates.begin();
    auto iterCEnd = candidates.end();

//    tmp.reserve(candidates.size());
#ifdef INTERSECT_WITH_SHRINKING
    while ((iterP != iterPEnd) && (iterC != candidates.end()))
#else
    while ((iterP != iterPEnd) && (iterC != iterCEnd))
#endif
    {
        if (iterP->id < *iterC)
            iterP++;
        else if (iterP->id > *iterC)
            iterC++;
        else
        {
            result.push_back(iterP->id);
            iterP++;
            
#ifdef INTERSECT_WITH_SHRINKING
            iterC = candidates.erase(iterC);
#else
            iterC++;
#endif
        }
    }
//    candidates.swap(tmp);
}

inline void mergeSort_withDeduplication(RelationStart &partition, RelationId &candidates, RelationId &result)
{
    auto iterP    = partition.begin();
    auto iterPEnd = partition.end();
    auto iterC    = candidates.begin();
    auto iterCEnd = candidates.end();

//    tmp.reserve(candidates.size());
#ifdef INTERSECT_WITH_SHRINKING
    while ((iterP != iterPEnd) && (iterC != candidates.end()))
#else
    while ((iterP != iterPEnd) && (iterC != iterCEnd))
#endif
    {
        if (iterP->id < *iterC)
            iterP++;
        else if (iterP->id > *iterC)
            iterC++;
        else
        {
#ifdef INTERSECT_WITH_SHRINKING
            result.push_back(iterP->id);
            iterP++;
            iterC = candidates.erase(iterC);
#else
            if (iterP->start >= partition.gstart)
                result.push_back(iterP->id);
            iterP++;
            iterC++;
#endif
        }
    }
//    candidates.swap(tmp);
}


//bool myfunction (const Record &i, const Record &j)
//{
//    return (i.id < j.id);
//}


void OneDimensionalGrid_RecordStart::interesect_gOverlaps(const RangeQuery &q, RelationId &candidates)
{
    RelationId result;
    auto s_pid = (max(q.start,gstart) == this->gend)? this->numPartitionsMinus1: (max(q.start,gstart)-gstart)/this->partitionExtent;
    auto e_pid = (min(q.end,gend)     == this->gend)? this->numPartitionsMinus1: (min(q.end,gend)-gstart)/this->partitionExtent;


//    cout << "s_pid = "<<s_pid<<"\te_pid = "<<e_pid<<endl;
#if defined(INTERSECT_WITH_BINARYSEARCH)
    // Alternative version
#ifdef INTERSECT_WITH_SHRINKING
    for (auto iterC = candidates.begin(); iterC != candidates.end(); iterC++)
    {
        for (auto pid = s_pid; pid <= e_pid; pid++)
        {
            if (binary_search(this->pRecs[pid].begin(), this->pRecs[pid].end(), Record(*iterC,0,0), compareRecordsById))
//            if (binary_search(this->pRecs[pid].begin(), this->pRecs[pid].end(), cid))
            {
                result.push_back(cid);
                break;
            }
        }
    }
#else
    for (auto &cid : candidates)
    {
        for (auto pid = s_pid; pid <= e_pid; pid++)
        {
            if (binary_search(this->pRecs[pid].begin(), this->pRecs[pid].end(), Record(cid,0,0), compareRecordsById))
//            if (binary_search(this->pRecs[pid].begin(), this->pRecs[pid].end(), cid))
            {
                result.push_back(cid);
                break;
            }
        }
    }
#endif
#elif defined(INTERSECT_WITH_BINARYSEARCH_AND_TEMPORALCHECKS)
    RelationIterator iter, iterBegin, iterEnd;

    // Handle the first partition.
    iterBegin = this->pRecs[s_pid].begin();
    iterEnd   = this->pRecs[s_pid].end();
//    cout<<"|P["<<s_pid<<"]| = " << this->pRecs[s_pid].size()<<endl;
    for (RelationIterator iter = iterBegin; iter != iterEnd; iter++)
    {
        if ((iter->start <= q.end) && (q.start <= iter->end) && (binary_search(candidates.begin(), candidates.end(), iter->id)))
            result.push_back(iter->id);
    }
//    cout<<s_pid << " ok"<<endl;

    // Handle partitions completely contained inside the query range.
    for (auto pid = s_pid+1; pid < e_pid; pid++)
    {
        RelationStart &p = this->pRecs[pid];
        iterBegin = p.begin();
        iterEnd = p.end();
        for (iter = p.begin(); iter != iterEnd; iter++)
        {
            // Perform de-duplication test.
//            if (max(Q.start, iter->start) >= p.gstart)
            if ((iter->start >= p.gstart) && (binary_search(candidates.begin(), candidates.end(), iter->id)))
                result.push_back(iter->id);
        }
//        cout<<pid << " ok"<<endl;
    }

    // Handle the last partition.
    if (e_pid != s_pid)
    {
        iterBegin = this->pRecs[e_pid].begin();
        iterEnd = this->pRecs[e_pid].end();
        for (iter = iterBegin; iter != iterEnd; iter++)
        {
//            if ((max(q.start, iter->start) >= this->pRecs[e_pId].gstart) && (iter->start <= q.end && q.start <= iter->end))
            if ((iter->start >= this->pRecs[e_pid].gstart) && (iter->start <= q.end) && (q.start <= iter->end) && (binary_search(candidates.begin(), candidates.end(), iter->id)))
                result.push_back(iter->id);
        }
    }
#elif defined(INTERSECT_WITH_MERGESORT)
    mergeSort_withoutDeduplication(this->pRecs[s_pid], candidates, result);
#ifdef PROFILING
    cout << "\t|C| = " << candidates.size() << "\tINTERSECT |P[" << s_pid << "]| = " << this->pRecs[s_pid].size() << endl;
#endif

    for (auto pid = s_pid+1; pid <= e_pid; pid++)
    {
        mergeSort_withDeduplication(this->pRecs[pid], candidates, result);
#ifdef PROFILING
        cout << "\t|C| = " << candidates.size() << "\tINTERSECT |P[" << pid << "]| = " << this->pRecs[s_pid].size() << endl;
#endif
    }
#endif

    candidates.swap(result);
}


void OneDimensionalGrid_RecordStart::interesectAndOutput_gOverlaps(const RangeQuery &q, RelationId &candidates, RelationId &result)
{
    auto s_pid = (max(q.start,gstart) == this->gend)? this->numPartitionsMinus1: (max(q.start,gstart)-gstart)/this->partitionExtent;
    auto e_pid = (min(q.end,gend)     == this->gend)? this->numPartitionsMinus1: (min(q.end,gend)-gstart)/this->partitionExtent;


//    cout << "s_pid = "<<s_pid<<"\te_pid = "<<e_pid<<endl;
#if defined(INTERSECT_WITH_BINARYSEARCH)
    // Alternative version
    for (auto &cid : candidates)
    {
        for (auto pid = s_pid; pid <= e_pid; pid++)
        {
            if (binary_search(this->pRecs[pid].begin(), this->pRecs[pid].end(), Record(cid,0,0), myfunction))
//            if (binary_search(this->pRecs[pid].begin(), this->pRecs[pid].end(), cid))
            {
                result.push_back(cid);
                break;
            }
        }
    }
#elif defined(INTERSECT_WITH_BINARYSEARCH_AND_TEMPORALCHECKS)
    RelationIterator iter, iterBegin, iterEnd;

    // Handle the first partition.
    iterBegin = this->pRecs[s_pid].begin();
    iterEnd   = this->pRecs[s_pid].end();
//    cout<<"|P["<<s_pid<<"]| = " << this->pRecs[s_pid].size()<<endl;
    for (RelationIterator iter = iterBegin; iter != iterEnd; iter++)
    {
        if ((iter->start <= q.end) && (q.start <= iter->end) && (binary_search(candidates.begin(), candidates.end(), iter->id)))
            result.push_back(iter->id);
    }
//    cout<<s_pid << " ok"<<endl;

    // Handle partitions completely contained inside the query range.
    for (auto pid = s_pid+1; pid < e_pid; pid++)
    {
        Relation &p = this->pRecs[pid];
        iterBegin = p.begin();
        iterEnd = p.end();
        for (iter = p.begin(); iter != iterEnd; iter++)
        {
            // Perform de-duplication test.
//            if (max(Q.start, iter->start) >= p.gstart)
            if ((iter->start >= p.gstart) && (binary_search(candidates.begin(), candidates.end(), iter->id)))
                result.push_back(iter->id);
        }
//        cout<<pid << " ok"<<endl;
    }

    // Handle the last partition.
    if (e_pid != s_pid)
    {
        iterBegin = this->pRecs[e_pid].begin();
        iterEnd = this->pRecs[e_pid].end();
        for (iter = iterBegin; iter != iterEnd; iter++)
        {
//            if ((max(q.start, iter->start) >= this->pRecs[e_pId].gstart) && (iter->start <= q.end && q.start <= iter->end))
            if ((iter->start >= this->pRecs[e_pid].gstart) && (iter->start <= q.end) && (q.start <= iter->end) && (binary_search(candidates.begin(), candidates.end(), iter->id)))
                result.push_back(iter->id);
        }
    }
#elif defined(INTERSECT_WITH_MERGESORT)
    mergeSort_withoutDeduplication(this->pRecs[s_pid], candidates, result);
#ifdef PROFILING
    cout << "\t|C| = " << candidates.size() << "\tINTERSECT |P[" << s_pid << "]| = " << this->pRecs[s_pid].size() << endl;
#endif

    for (auto pid = s_pid+1; pid <= e_pid; pid++)
    {
        mergeSort_withDeduplication(this->pRecs[pid], candidates, result);
#ifdef PROFILING
        cout << "\t|C| = " << candidates.size() << "\tINTERSECT |P[" << pid << "]| = " << this->pRecs[s_pid].size() << endl;
#endif
    }
#endif
}

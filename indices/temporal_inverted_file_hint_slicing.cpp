#include "temporal_inverted_files.h"



//unsigned int determineOptimalNumBitsForHINT_M(const RecordId numRecords, const Timestamp domainSize, Timestamp avgRecordExtent, const float qe_precentage);


TemporalInvertedFile_HINTSlicing::TemporalInvertedFile_HINTSlicing()
{
}


TemporalInvertedFile_HINTSlicing::TemporalInvertedFile_HINTSlicing(const IRelation &R, const unsigned numPartitions, const unsigned int numBits = 0) : InvertedFileTemplate()
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

    // Step 4: construct HINTs
    for (auto iterL = tmplists.begin(); iterL != tmplists.end(); iterL++)
    {
//        iterL->second.gstart = R.gstart;
//        iterL->second.gend = R.gend;
//        this->glists.insert(make_pair(iterL->first, new OneDimensionalGrid(iterL->second, numPartitions)));
        this->glists.insert(make_pair(iterL->first, new OneDimensionalGrid_RecordStart(iterL->second, numPartitions)));

        iterL->second.domainSize = iterL->second.gend-iterL->second.gstart;
//        iterL->second.gstart = R.gstart;
//        iterL->second.gend = R.gend;
//        cout << "t" << iterL->first << ": " << iterL->second.size() << " interval(s) << m = " << determineOptimalNumBitsForHINT_M(iterL->second.size(), iterL->second.domainSize, iterL->second.avgRecordExtent, 0.1) << endl;
#ifdef USE_SS
        this->hlists.insert(make_pair(iterL->first, new HINT_M_SubsSortByRecordId_SS_CM(iterL->second, numBits)));
#else
        this->hlists.insert(make_pair(iterL->first, new HINT_M_SubsSortByRecordId_CM(iterL->second, numBits)));
#endif
    }

    tmplists.clear();
}


TemporalInvertedFile_HINTSlicing::TemporalInvertedFile_HINTSlicing(const IRelation &R, const IRelation &U, const TermId dictionarySize, const unsigned numPartitions, const unsigned int numBits = 0) : InvertedFileTemplate()
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

    // Step 4: construct HINTs
    for (auto iterL = tmplists.begin(); iterL != tmplists.end(); iterL++)
    {
        auto i = tmpUlists.find(iterL->first);

        iterL->second.domainSize = iterL->second.gend-iterL->second.gstart;
        
        if (i == tmpUlists.end())   // No updates will take place in this postings list
        {
            this->glists.insert(make_pair(iterL->first, new OneDimensionalGrid_RecordStart(iterL->second, numPartitions)));

#ifdef USE_SS
            this->hlists.insert(make_pair(iterL->first, new HINT_M_SubsSortByRecordId_SS_CM(iterL->second, numBits)));
#else
            this->hlists.insert(make_pair(iterL->first, new HINT_M_SubsSortByRecordId_CM(iterL->second, numBits)));
#endif
        }
        else
        {
            this->glists.insert(make_pair(iterL->first, new OneDimensionalGrid_RecordStart(iterL->second, i->second, numPartitions)));

#ifdef USE_SS
            this->hlists.insert(make_pair(iterL->first, new HINT_M_SubsSortByRecordId_SS_CM(iterL->second, i->second, numBits)));
#else
            this->hlists.insert(make_pair(iterL->first, new HINT_M_SubsSortByRecordId_CM(iterL->second, i->second, numBits)));
#endif
        }
    }

    for (auto iterL = tmpUlists.begin(); iterL != tmpUlists.end(); iterL++)
    {
        auto i = tmplists.find(iterL->first);
        if (i == tmplists.end())
        {
            this->glists.insert(make_pair(iterL->first, new OneDimensionalGrid_RecordStart(Relation(), iterL->second, numPartitions)));

#ifdef USE_SS
            this->hlists.insert(make_pair(iterL->first, new HINT_M_SubsSortByRecordId_SS_CM(Relation(), iterL->second, numBits)));
#else
            this->hlists.insert(make_pair(iterL->first, new HINT_M_SubsSortByRecordId_CM(Relation(), iterL->second, numBits)));
#endif
        }
    }

    tmplists.clear();
    tmpUlists.clear();
}


void TemporalInvertedFile_HINTSlicing::getStats()
{
//    size_t sum = 0;
//
//    this->numEmptyLists = this->R->dictionarySize - this->hlists.size();
//    for (auto iter = this->hlists.begin(); iter != this->hlists.end(); iter++)
//        sum += iter->second.size();
//    this->avgListSize = (float)sum/this->hlists.size();
}


size_t TemporalInvertedFile_HINTSlicing::getSize()
{
    size_t size = 0;

    for (auto iterL = this->glists.begin(); iterL != this->glists.end(); iterL++)
        size += iterL->second->getSize();

    for (auto iterL = this->hlists.begin(); iterL != this->hlists.end(); iterL++)
        size += iterL->second->getSize();
    
    return size;
}


TemporalInvertedFile_HINTSlicing::~TemporalInvertedFile_HINTSlicing()
{
    for (auto iterL = this->glists.begin(); iterL != this->glists.end(); iterL++)
        delete iterL->second;
    
    for (auto iterL = this->hlists.begin(); iterL != this->hlists.end(); iterL++)
        delete iterL->second;
}


void TemporalInvertedFile_HINTSlicing::print(char c)
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
void TemporalInvertedFile_HINTSlicing::insert(IRelation &U)
{
    for (auto &r : U)
    {
        for (auto &tid : r.terms)
        {
            this->glists[tid]->insert(Record(r.id, r.start, r.end));
            this->hlists[tid]->insert(Record(r.id, r.start, r.end));
        }
    }
}



// Querying
bool TemporalInvertedFile_HINTSlicing::moveOut_gOverlaps(const RangeIRQuery &q, RelationId &candidates)
{
    auto iterL = this->hlists.find(q.terms[0]);
    
    // If the inverted file does not contain the term then result must be empty
    if (iterL == this->hlists.end())
        return false;
    else
    {
        iterL->second->moveOut_gOverlaps(RangeQuery(q.id, q.start, q.end), candidates);

        return (!candidates.empty());
    }
}


bool TemporalInvertedFile_HINTSlicing::intersect_gOverlaps(const RangeIRQuery &q, const unsigned int off, RelationId &candidates)
{
    auto iterL = this->glists.find(q.terms[off]);
    
    // If the inverted file does not contain the term then result must be empty
    if (iterL == this->glists.end())
        return false;
    else
    {
        iterL->second->interesect_gOverlaps(RangeQuery(q.id, q.start, q.end), candidates);
                
        return (!candidates.empty());
    }
}


bool TemporalInvertedFile_HINTSlicing::intersectAndOutput_gOverlaps(const RangeIRQuery &q, const unsigned int off, RelationId &candidates, RelationId &result)
{
    auto iterL = this->glists.find(q.terms[off]);
    
    // If the inverted file does not contain the term then result must be empty
    if (iterL == this->glists.end())
        return false;
    else
    {
        iterL->second->interesectAndOutput_gOverlaps(RangeQuery(q.id, q.start, q.end), candidates, result);
                
        return (!result.empty());
    }
}


bool TemporalInvertedFile_HINTSlicing::moveOut_gOverlaps(const RangeIRQuery &q, vector<RelationId> &vec_candidates)
{
    auto iterL = this->hlists.find(q.terms[0]);
    
    // If the inverted file does not contain the term then result must be empty
    if (iterL == this->hlists.end())
        return false;
    else
    {
        iterL->second->moveOut_gOverlaps(RangeQuery(q.id, q.start, q.end), vec_candidates);

        return (!vec_candidates.empty());
    }
}


//#define MWAY_MERGESORT_STL
#define MWAY_MERGESORT_STL_INPLACE
//#define MWAY_MERGESORT_PQUEUE
class compareQueueElements
{
public:
  bool operator() (const pair<RelationIdIterator,RelationIdIterator> &lhs, const pair<RelationIdIterator,RelationIdIterator> &rhs) const
  {
      return (*(lhs.first) > *(rhs.first));
  }
};
inline void mwayMergeSort(vector<RelationId> &vec_candidates, RelationId &result)
{
#if defined(MWAY_MERGESORT_STL)
    result.swap(vec_candidates[0]);
    for (auto i = 1; i < vec_candidates.size(); i++)
    {
        RelationId res2(result.size()+vec_candidates[i].size());
        merge(result.begin(), result.end(), vec_candidates[i].begin(), vec_candidates[i].end(), res2.begin());
        result.swap(res2);
    }
#elif defined(MWAY_MERGESORT_STL_INPLACE)
    result.swap(vec_candidates[0]);
    for (auto i = 1; i < vec_candidates.size(); i++)
    {
        result.insert(result.begin(), vec_candidates[i].begin(), vec_candidates[i].end());
        inplace_merge(result.begin(), result.begin()+vec_candidates[i].size(), result.end());
//        RelationId res2(result.size()+vec_candidates[i].size());
//        merge(result.begin(), result.end(), vec_candidates[i].begin(), vec_candidates[i].end(), res2.begin());
//        result.swap(res2);
    }
#elif defined(MWAY_MERGESORT_PQUEUE)
    priority_queue<pair<RelationIdIterator,RelationIdIterator>, vector<pair<RelationIdIterator,RelationIdIterator>>, compareQueueElements> Q;

//    auto sum = 0;
    result.clear();
    for (RelationId &cand: vec_candidates)
    {
        Q.push(make_pair(cand.begin(), cand.end()));
//        cout << "push " << *cand.begin() << "\t(" << cand.size() << ")"<<endl;
//        sum+=cand.size();
    }
//    cout<<sum<<endl;

    while (!Q.empty())
    {
        auto curr = Q.top();
        Q.pop();
        
//        cout << "pop: " << *curr.first << endl;
        
        if (Q.empty())
        {
            result.insert(result.end(), curr.first, curr.second);
//            while (curr.first != curr.second)
//            {
//                result.push_back(*curr.first);
//                curr.first++;
//            }
            return;
        }
        else
        {
            auto cid_newtop = *(Q.top().first);
            while ((curr.first != curr.second) && (*curr.first < cid_newtop))
            {
                result.push_back(*curr.first);
                curr.first++;
            }
//            result.push_back(*curr.first);
//            curr.first++;
            if (curr.first != curr.second)
                Q.push(curr);
        }
    }
#endif
}


#define V2

#if defined(V1)
void TemporalInvertedFile_HINTSlicing::executeContainment(const RangeIRQuery &q, RelationId &result)
{
#ifdef PROFILING
    Timer tim;
    double secsMO = 0, secsS = 0, secsI = 0, secsIO = 0;
    cout<<endl;
#endif
    
    RelationId candidates;
    auto numQueryTerms = q.terms.size();
    
    if (numQueryTerms == 1)
        this->moveOut_gOverlaps(q, result);
    else
    {
#ifdef PROFILING
        tim.start();
#endif
        if (!this->moveOut_gOverlaps(q, candidates))
            return;
#ifdef PROFILING
        secsMO = tim.stop();
        tim.start();
#endif
        sort(candidates.begin(), candidates.end());
#ifdef PROFILING
        secsS += tim.stop();
        cout<<"q" << q.id << ": t = " << q.terms[0] << "\t|C| = " << candidates.size() << endl;
#endif
        for (auto i = 1; i < numQueryTerms-1; i++)
        {
#ifdef PROFILING
            tim.start();
#endif
            if (!this->intersect_gOverlaps(q, i, candidates))
                return;
#ifdef PROFILING
            secsI += tim.stop();
            tim.start();
#endif
            sort(candidates.begin(), candidates.end());
#ifdef PROFILING
            secsS += tim.stop();
            cout<<"q" << q.id << ": t = " << q.terms[i] << "\t|C| = " << candidates.size() << endl;
#endif
        }
#ifdef PROFILING
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
#elif defined(V2)
void TemporalInvertedFile_HINTSlicing::executeContainment(const RangeIRQuery &q, RelationId &result)
{
    //    cout<<"V2"<<endl;
#ifdef PROFILING
    Timer tim;
    double secsMO = 0, secsS = 0, secsI = 0, secsIO = 0;
    cout<<endl;
#endif
    
    RelationId candidates;
    vector<RelationId> vec_candidates;
    auto numQueryTerms = q.terms.size();
    
    if (numQueryTerms == 1)
        this->moveOut_gOverlaps(q, result);
    else
    {
#ifdef PROFILING
        tim.start();
#endif
        if (!this->moveOut_gOverlaps(q, vec_candidates))
            return;
#ifdef PROFILING
        secsMO = tim.stop();
        tim.start();
#endif
        mwayMergeSort(vec_candidates, candidates);
#ifdef PROFILING
        secsS += tim.stop();
        cout<<"q" << q.id << ": t = " << q.terms[0] << "\t|C| = " << candidates.size() << " from " << vec_candidates.size() << " subsets" << endl;
#endif
        for (auto i = 1; i < numQueryTerms-1; i++)
        {
#ifdef PROFILING
            tim.start();
#endif
            if (!this->intersect_gOverlaps(q, i, candidates))
                return;
#ifdef PROFILING
            secsI += tim.stop();
            tim.start();
#endif
            sort(candidates.begin(), candidates.end());
#ifdef PROFILING
            secsS += tim.stop();
            cout<<"q" << q.id << ": t = " << q.terms[i] << "\t|C| = " << candidates.size() << endl;
#endif
        }
#ifdef PROFILING
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
#endif

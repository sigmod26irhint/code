#include "temporal_inverted_files.h"



//unsigned int determineOptimalNumBitsForHINT_M(const RecordId numRecords, const Timestamp domainSize, Timestamp avgRecordExtent, const float qe_precentage);


TemporalInvertedFile_HINTGamma::TemporalInvertedFile_HINTGamma()
{
}


TemporalInvertedFile_HINTGamma::TemporalInvertedFile_HINTGamma(const IRelation &R, const unsigned int numBits = 0) : InvertedFileTemplate()
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
        iterL->second.domainSize = iterL->second.gend-iterL->second.gstart;
//        iterL->second.gstart = R.gstart;
//        iterL->second.gend = R.gend;
//        cout << "t" << iterL->first << ": " << iterL->second.size() << " interval(s) << m = " << ((numBits != 0)? numBits: determineOptimalNumBitsForHINT_M(iterL->second.size(), iterL->second.domainSize, iterL->second.avgRecordExtent, 0.1)) << endl;
#ifdef USE_SS
        this->hlists.insert(make_pair(iterL->first, new HINT_M_SubsSortByRecordId_SS_CM(iterL->second, numBits)));
#else
        this->hlists.insert(make_pair(iterL->first, new HINT_M_SubsSortByRecordId_CM(iterL->second, numBits)));
#endif
//        cout << "\tHINT constructed" << endl;
    }
//    cout<<"done"<<endl;

    tmplists.clear();
}


TemporalInvertedFile_HINTGamma::TemporalInvertedFile_HINTGamma(const IRelation &R, const IRelation &U, const TermId dictionarySize, const unsigned int numBits = 0) : InvertedFileTemplate()
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
#ifdef USE_SS
            this->hlists.insert(make_pair(iterL->first, new HINT_M_SubsSortByRecordId_SS_CM(iterL->second, numBits)));
#else
            this->hlists.insert(make_pair(iterL->first, new HINT_M_SubsSortByRecordId_CM(iterL->second, numBits)));
#endif
        }
        else
        {
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


void TemporalInvertedFile_HINTGamma::getStats()
{
//    size_t sum = 0;
//
//    this->numEmptyLists = this->R->dictionarySize - this->hlists.size();
//    for (auto iter = this->hlists.begin(); iter != this->hlists.end(); iter++)
//        sum += iter->second.size();
//    this->avgListSize = (float)sum/this->hlists.size();
}


size_t TemporalInvertedFile_HINTGamma::getSize()
{
    size_t size = 0;
    
    for (auto iterL = this->hlists.begin(); iterL != this->hlists.end(); iterL++)
        size += iterL->second->getSize();
    
    return size;
}


TemporalInvertedFile_HINTGamma::~TemporalInvertedFile_HINTGamma()
{
    for (auto iterL = this->hlists.begin(); iterL != this->hlists.end(); iterL++)
        delete iterL->second;
}


void TemporalInvertedFile_HINTGamma::print(char c)
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
void TemporalInvertedFile_HINTGamma::insert(IRelation &U)
{
    for (auto &r : U)
    {
        for (auto &tid : r.terms)
            this->hlists[tid]->insert(Record(r.id, r.start, r.end));
    }
}



// Querying
bool TemporalInvertedFile_HINTGamma::moveOut_gOverlaps(const RangeIRQuery &q, RelationId &candidates)
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


bool TemporalInvertedFile_HINTGamma::intersect_gOverlaps(const RangeIRQuery &q, const unsigned int off, RelationId &candidates)
{
    auto iterL = this->hlists.find(q.terms[off]);
    
    // If the inverted file does not contain the term then result must be empty
    if (iterL == this->hlists.end())
        return false;
    else
    {
        iterL->second->intersect_gOverlaps(RangeQuery(q.id, q.start, q.end), candidates);

        return (!candidates.empty());
    }
}


bool TemporalInvertedFile_HINTGamma::intersectAndOutput_gOverlaps(const RangeIRQuery &q, const unsigned int off, RelationId &candidates, RelationId &result)
{
    auto iterL = this->hlists.find(q.terms[off]);
    
    // If the inverted file does not contain the term then result must be empty
    if (iterL == this->hlists.end())
        return false;
    else
    {
        iterL->second->intersectAndOutput_gOverlaps(RangeQuery(q.id, q.start, q.end), candidates, result);

        return (!result.empty());
    }
}


bool TemporalInvertedFile_HINTGamma::moveOut_gOverlaps(const RangeIRQuery &q, vector<RelationId> &vec_candidates)
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


bool TemporalInvertedFile_HINTGamma::moveOut_NoChecks_gOverlaps(const RangeIRQuery &q, const unsigned int off, vector<RelationId> &vec_candidates)
{
    auto iterL = this->hlists.find(q.terms[off]);
    
    // If the inverted file does not contain the term then result must be empty
    if (iterL == this->hlists.end())
        return false;
    else
    {
        iterL->second->moveOut_NoChecks_gOverlaps(RangeQuery(q.id, q.start, q.end), vec_candidates);

        return (!vec_candidates.empty());
    }
}

bool TemporalInvertedFile_HINTGamma::intersect_gOverlaps(const RangeIRQuery &q, const unsigned int off, RelationId &candidates, vector<RelationId> &vec_candidates)
{
    auto iterL = this->hlists.find(q.terms[off]);
    
    // If the inverted file does not contain the term then result must be empty
    if (iterL == this->hlists.end())
        return false;
    else
    {
        iterL->second->intersect_gOverlaps(RangeQuery(q.id, q.start, q.end), candidates, vec_candidates);

        return (!candidates.empty());
    }
}


//bool TemporalInvertedFile_HINTGamma::intersect_mway_gOverlaps(const RangeIRQuery &q, const unsigned int off, vector<RelationId> &vec_candidates)
//{
//    auto iterL = this->hlists.find(q.terms[off]);
//    
//    // If the inverted file does not contain the term then result must be empty
//    if (iterL == this->hlists.end())
//        return false;
//    else
//    {
//        iterL->second->intersect_mway_gOverlaps(RangeQuery(q.id, q.start, q.end), vec_candidates);
//
//        return (!vec_candidates.empty());
//    }
//}
//
//
//bool TemporalInvertedFile_HINTGamma::intersectAndOutput_mway_gOverlaps(const RangeIRQuery &q, const unsigned int off, vector<RelationId> &vec_candidates, vector<RelationId> &vec_result)
//{
//    auto iterL = this->hlists.find(q.terms[off]);
//    
//    // If the inverted file does not contain the term then result must be empty
//    if (iterL == this->hlists.end())
//        return false;
//    else
//    {
//        iterL->second->intersectAndOutput_mway_gOverlaps(RangeQuery(q.id, q.start, q.end), vec_candidates, vec_result);
//
//        return (!vec_result.empty());
//    }
//}


//bool TemporalInvertedFile_HINTGamma::verify_gOverlaps(const RangeIRQuery &q, const unsigned int off, RelationId &candidates, RelationId &result)
//{
//    for (auto cid : candidates)
//    {
//        const IRecord &r = this->R.at(cid);
//        if (r.containsTerms(q.terms, off))
//            result.push_back(cid);
//    }
//    
//    return (!result.empty());
//}

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



inline void mergeSort(RelationId &candidates, RelationId &divisions)
{
    auto iterD = divisions.begin();
    auto iterDEnd = divisions.end();
    auto iterC    = candidates.begin();
    auto iterCEnd = candidates.end();
    RelationId result;

    while ((iterD != iterDEnd) && (iterC != iterCEnd))
    {
        if (*iterD < *iterC)
            iterD++;
        else if (*iterD > *iterC)
            iterC++;
        else
        {
            result.push_back(*iterD);
            iterD++;
            
#ifdef INTERSECT_WITH_SHRINKING
            iterC = candidates.erase(iterC);
#else
            iterC++;
#endif
        }
    }
    
    candidates.swap(result);
}




#define V2

#if defined(V1)
void TemporalInvertedFile_HINTGamma::executeContainment(const RangeIRQuery &q, RelationId &result)
{
//    cout<<"V1"<<endl;
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
//        for (const RecordId &c: candidates)
//            cout << "\t"<<c<<endl;
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
//            for (const RecordId &c: candidates)
//                cout << "\t"<<c<<endl;
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
//
#elif defined(V2)
void TemporalInvertedFile_HINTGamma::executeContainment(const RangeIRQuery &q, RelationId &result)
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
//        for (const RecordId &c: candidates)
//            cout << "\t"<<c<<endl;
#endif
        for (auto i = 1; i < numQueryTerms-1; i++)
        {
#ifdef PROFILING
            tim.start();
#endif
            vec_candidates.clear();
            if (!this->intersect_gOverlaps(q, i, candidates, vec_candidates))
                return;
#ifdef PROFILING
            secsI += tim.stop();
            tim.start();
#endif
            mwayMergeSort(vec_candidates, candidates);
#ifdef PROFILING
            secsS += tim.stop();
            cout<<"q" << q.id << ": t = " << q.terms[i] << "\t|C| = " << candidates.size() << " from " << vec_candidates.size() << " subsets" << endl;;
//            for (const RecordId &c: candidates)
//                cout << "\t"<<c<<endl;
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
//
#elif defined(V3)
void TemporalInvertedFile_HINTGamma::executeContainment(const RangeIRQuery &q, RelationId &result)
{
//    cout<<"V3"<<endl;
#ifdef PROFILING
    Timer tim;
    double secsMO = 0, secsS = 0, secsI = 0, secsIO = 0;
    cout<<endl;
#endif
    
    RelationId candidates, divisions;
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
#endif

//        for (const RelationId &cands: vec_candidates)
//        {
//            if (cands.empty())
//                cout << "Oops" << endl;
//        }

#ifdef PROFILING
        tim.start();
#endif
        mwayMergeSort(vec_candidates, result);
#ifdef PROFILING
        secsS += tim.stop();
        cout<<"q" << q.id << ": t = " << q.terms[0] << "\t|C| = " << result.size() << " from " << vec_candidates.size() << " subsets" << endl;
#endif
        for (auto i = 1; i < numQueryTerms-1; i++)
        {
#ifdef PROFILING
            tim.start();
#endif
            vec_candidates.clear();
            if (!this->moveOut_NoChecks_gOverlaps(q, i, vec_candidates))
                return;
#ifdef PROFILING
            secsI += tim.stop();
            tim.start();
#endif
            mwayMergeSort(vec_candidates, divisions);
#ifdef PROFILING
//            secsS += tim.stop();
//            cout<<"Candidates ("<<candidates.size()<<"):";
//            for (auto &id:candidates)
//                cout << " "<<id;
//            cout<<endl<<endl;
//            cout<<"Divisions ("<<divisions.size()<<"):";
//            for (auto &id:divisions)
//                cout << " "<<id;
//            cout<<endl;
#endif
//            for (const RelationId &cands: vec_candidates)
//            {
//                if (cands.empty())
//                    cout << "Oops" << endl;
//            }
#ifdef PROFILING
            tim.start();
            cout << "\t|C| = " << result.size() << "\tINTERSECT |P| = " << divisions.size() << endl;
#endif
            mergeSort(result, divisions);
#ifdef PROFILING
            secsI += tim.stop();
            cout<<"q" << q.id << ": t = " << q.terms[i] << "\t|C| = " << result.size() << " from " << vec_candidates.size() << " subsets" << endl;;
#endif
        }
//        cout<<"q" << q.id << ": t = " << q.terms[numQueryTerms-2] << "\t|C| = " << candidates.size()<<endl;
//        tim.start();
//        sort(candidates.begin(), candidates.end());
//        secsS += tim.stop();
#ifdef PROFILING
        tim.start();
#endif
        vec_candidates.clear();
        if (!this->moveOut_NoChecks_gOverlaps(q, numQueryTerms-1, vec_candidates))
            return;
#ifdef PROFILING
        secsIO += tim.stop();
        tim.start();
#endif
        mwayMergeSort(vec_candidates, divisions);
#ifdef PROFILING
        secsS += tim.stop();
        cout << "\t|C| = " << result.size() << "\tINTERSECT |P| = " << divisions.size() << endl;

        tim.start();
#endif
        mergeSort(result, divisions);
#ifdef PROFILING
        secsI += tim.stop();

        printf("moveOut          : %f secs\n", secsMO);
        printf("sort             : %f secs\n", secsS);
        printf("intersect        : %f secs\n", secsI);
        printf("intersect and out: %f secs\n", secsIO);
#endif
    }
}
//
#elif defined(V4)
void TemporalInvertedFile_HINTGamma::executeContainment(const RangeIRQuery &q, RelationId &result)
{
//    cout<<"V4"<<endl;
#ifdef PROFILING
    Timer tim;
    double secsMO = 0, secsS = 0, secsI = 0, secsIO = 0, secsV = 0;
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
//        if (candidates.size() < THRESHOLD_VERIFY)
//        {
//#ifdef PROFILING
//            tim.start();
//#endif
//            this->verify_gOverlaps(q, 1, candidates, result);
//            
//#ifdef PROFILING
//            secsV = tim.stop();
//
//            printf("moveOut          : %f secs\n", secsMO);
//            printf("sort             : %f secs\n", secsS);
//            printf("intersect        : %f secs\n", secsI);
//            printf("intersect and out: %f secs\n", secsIO);
//            printf("verify           : %f secs\n", secsV);
//#endif
//
//            return;
//        }

        mwayMergeSort(vec_candidates, candidates);
#ifdef PROFILING
        secsS += tim.stop();
        cout<<"q" << q.id << ": t = " << q.terms[0] << "\t|C| = " << candidates.size() << " from " << vec_candidates.size() << " subsets" << endl << endl;
#endif
        for (auto i = 1; i < numQueryTerms-1; i++)
        {
#ifdef PROFILING
            tim.start();
#endif
            if (!this->intersect2_gOverlaps(q, i, candidates))
                return;
#ifdef PROFILING
            secsI += tim.stop();
#endif
//            for (const RelationId &cands: vec_candidates)
//            {
//                if (cands.empty())
//                    cout << "Oops" << endl;
//            }
//            if (candidates.size() < THRESHOLD_VERIFY)
//            {
//#ifdef PROFILING
//                tim.start();
//#endif
//                this->verify_gOverlaps(q, i+1, candidates, result);
//                
//#ifdef PROFILING
//                secsV = tim.stop();
//
//                printf("moveOut          : %f secs\n", secsMO);
//                printf("sort             : %f secs\n", secsS);
//                printf("intersect        : %f secs\n", secsI);
//                printf("intersect and out: %f secs\n", secsIO);
//                printf("verify           : %f secs\n", secsV);
//#endif
//
//                return;
//            }

#ifdef PROFILING
            tim.start();
#endif
//            candidates.clear();
#ifdef PROFILING
            secsS += tim.stop();
            cout<<"q" << q.id << ": t = " << q.terms[i] << "\t|C| = " << candidates.size() << " from " << vec_candidates.size() << " subsets" << endl;
#endif
        }
//        cout<<"q" << q.id << ": t = " << q.terms[numQueryTerms-2] << "\t|C| = " << candidates.size()<<endl;
//        tim.start();
//        sort(candidates.begin(), candidates.end());
//        secsS += tim.stop();
        
#ifdef PROFILING
        tim.start();
#endif
        this->intersect2AndOutput_mway_gOverlaps(q, numQueryTerms-1, candidates, result);
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

/*
void TemporalInvertedFile_HINTGamma::executeContainment(const RangeIRQuery &q, vector<RelationId> &vec_result)
{
#ifdef PROFILING
    cout<<"Version fragmented result"<<endl;
    Timer tim;
    double secsMO = 0, secsS = 0, secsI = 0, secsIO = 0;
#endif
    
    RelationId candidates;
    vector<RelationId> vec_candidates;
    auto numQueryTerms = q.terms.size();
    
    if (numQueryTerms == 1)
        this->moveOut_gOverlaps(q, vec_result[0]);
    else
    {
#ifdef PROFILING
        tim.start();
#endif
        if (!this->moveOut_gOverlaps(q, vec_candidates))
            return;
        
#ifdef PROFILING
        secsMO = tim.stop();
        auto sum = 0;
        for (const RelationId c: vec_candidates)
        {
            sum += c.size();
        }
        cout<<"q" << q.id << ": t = " << q.terms[0] << "\t|C| = " << sum << " from " << vec_candidates.size() << " subsets" << endl;
#endif

        for (auto i = 1; i < numQueryTerms-1; i++)
        {
#ifdef PROFILING
            tim.start();
#endif
            if (!this->intersect_mway_gOverlaps(q, i, vec_candidates))
                return;
#ifdef PROFILING
            secsI += tim.stop();
            sum = 0;
            for (const RelationId c: vec_candidates)
            {
                sum += c.size();
            }
            cout<<"q" << q.id << ": t = " << q.terms[i] << "\t|C| = " << sum << " from " << vec_candidates.size() << " subsets" << endl;
#endif
        }
#ifdef PROFILING
        tim.start();
#endif
        this->intersectAndOutput_mway_gOverlaps(q, numQueryTerms-1, vec_candidates, vec_result);
#ifdef PROFILING
        secsIO += tim.stop();
        
        cout<<endl;
        printf("moveOut          : %f secs\n", secsMO);
        printf("sort             : %f secs\n", secsS);
        printf("intersect        : %f secs\n", secsI);
        printf("intersect and out: %f secs\n", secsIO);
#endif
    }
}

bool TemporalInvertedFile_HINTGamma::intersect2_gOverlaps(const RangeIRQuery &q, const unsigned int termoff, RelationId &candidates)
{
    auto iterL = this->hlists.find(q.terms[termoff]);
    
    // If the inverted file does not contain the term then result must be empty
    if (iterL == this->hlists.end())
        return false;
    else
    {
        iterL->second->intersect2_gOverlaps(RangeQuery(q.id, q.start, q.end), candidates);

        return (!candidates.empty());
    }
}

bool TemporalInvertedFile_HINTGamma::intersect2AndOutput_mway_gOverlaps(const RangeIRQuery &q, const unsigned int termoff, RelationId &candidates, RelationId &result)
{
    auto iterL = this->hlists.find(q.terms[termoff]);
    
    // If the inverted file does not contain the term then result must be empty
    if (iterL == this->hlists.end())
        return false;
    else
    {
        iterL->second->intersect2AndOutput_gOverlaps(RangeQuery(q.id, q.start, q.end), candidates, result);

        return (!candidates.empty());
    }

}*/

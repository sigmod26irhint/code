#include "temporal_inverted_files.h"



TemporalInvertedFile_HINTGammaH::TemporalInvertedFile_HINTGammaH()
{
}
unsigned int determineOptimalNumBitsForHINT_M(const RecordId numRecords, const Timestamp domainSize, Timestamp avgRecordExtent, const float qe_precentage);

// Undecided if we should use two passes
TemporalInvertedFile_HINTGammaH::TemporalInvertedFile_HINTGammaH(const IRelation &R, const unsigned int numBits = 0) : InvertedFileTemplate()
{
    this->R = &R;

    this->tif = new TemporalInvertedFile(R);

    auto iterL = this->tif->lists.begin();
    while (iterL != this->tif->lists.end())
    {
        iterL->second.domainSize = iterL->second.gend-iterL->second.gstart;
//        cout << "t" << iterL->first << ": " << iterL->second.size() << " interval(s) << m = " << determineOptimalNumBitsForHINT_M(iterL->second.size(), iterL->second.domainSize, iterL->second.avgRecordExtent, 0.1) << endl;
        if (iterL->second.size() > 10000)
        {
            this->hints.insert(make_pair(iterL->first, new HINT_M_SubsSortByRecordId_SS_CM(iterL->second, numBits)));
            iterL = this->tif->lists.erase(iterL);
        }
        else
            iterL++;
    }
}


void TemporalInvertedFile_HINTGammaH::getStats()
{
//    size_t sum = 0;
//
//    this->numEmptyLists = this->R->dictionarySize - this->hints.size();
//    for (auto iter = this->hints.begin(); iter != this->hints.end(); iter++)
//        sum += iter->second.size();
//    this->avgListSize = (float)sum/this->hints.size();
}


size_t TemporalInvertedFile_HINTGammaH::getSize()
{
    size_t size = 0;
    
    size += this->tif->getSize();
    
    for (auto iterL = this->hints.begin(); iterL != this->hints.end(); iterL++)
        size += iterL->second->getSize();
    
    return size;
}


TemporalInvertedFile_HINTGammaH::~TemporalInvertedFile_HINTGammaH()
{
    delete this->tif;

    for (auto iterL = this->hints.begin(); iterL != this->hints.end(); iterL++)
        delete iterL->second;
}


void TemporalInvertedFile_HINTGammaH::print(char c)
{
//    for (auto iter = this->dlists.begin(); iter != this->dlists.end(); iter++)
//    {
//        cout << "t" << iter->first << " (" << iter->second.size() << "):";
//        for (auto &r : iter->second)
//            cout << " " << c << r.id;
//        cout << endl;
//    }
}


bool TemporalInvertedFile_HINTGammaH::moveOut_gOverlaps(const RangeIRQuery &q, RelationId &candidates)
{
    auto iterHL = this->hints.find(q.terms[0]);
    
    // If the inverted file does not contain the term then result must be empty
    if (iterHL == this->hints.end())
    {
        return this->tif->moveOut_CheckBoth_gOverlaps(q, candidates);
    }
    else
    {
        iterHL->second->moveOut_gOverlaps(RangeQuery(q.id, q.start, q.end), candidates);

        return (!candidates.empty());
    }
}


bool TemporalInvertedFile_HINTGammaH::intersect_gOverlaps(const RangeIRQuery &q, const unsigned int off, RelationId &candidates)
{
    auto iterHL = this->hints.find(q.terms[off]);
    
    // If the inverted file does not contain the term then result must be empty
    if (iterHL == this->hints.end())
    {
        return this->tif->intersect_gOverlaps(q, off, candidates);
    }
    else
    {
        iterHL->second->intersect_gOverlaps(RangeQuery(q.id, q.start, q.end), candidates);

        return (!candidates.empty());
    }
}


bool TemporalInvertedFile_HINTGammaH::intersectAndOutput_gOverlaps(const RangeIRQuery &q, const unsigned int off, RelationId &candidates, RelationId &result)
{
    auto iterHL = this->hints.find(q.terms[off]);
    
    // If the inverted file does not contain the term then result must be empty
    if (iterHL == this->hints.end())
    {
        return this->tif->intersectAndOutput_gOverlaps(q, off, candidates, result);
    }
    else
    {
        iterHL->second->intersectAndOutput_gOverlaps(RangeQuery(q.id, q.start, q.end), candidates, result);

        return (!result.empty());
    }
}


bool TemporalInvertedFile_HINTGammaH::moveOut_gOverlaps(const RangeIRQuery &q, vector<RelationId> &vec_candidates)
{
    auto iterHL = this->hints.find(q.terms[0]);
    
    // If the inverted file does not contain the term then result must be empty
    if (iterHL == this->hints.end())
    {
        return this->tif->moveOut_CheckBoth_gOverlaps(q, vec_candidates[0]);
    }
    else
    {
        iterHL->second->moveOut_gOverlaps(RangeQuery(q.id, q.start, q.end), vec_candidates);

        return (!vec_candidates.empty());
    }
}


bool TemporalInvertedFile_HINTGammaH::moveOut_NoChecks_gOverlaps(const RangeIRQuery &q, const unsigned int off, vector<RelationId> &vec_candidates)
{
    auto iterL = this->hints.find(q.terms[off]);
    
    // If the inverted file does not contain the term then result must be empty
    if (iterL == this->hints.end())
    {
        return this->tif->moveOut_NoChecks_gOverlaps(q, vec_candidates[0]);
    }
    else
    {
        iterL->second->moveOut_NoChecks_gOverlaps(RangeQuery(q.id, q.start, q.end), vec_candidates);

        return (!vec_candidates.empty());
    }
}

bool TemporalInvertedFile_HINTGammaH::intersect_gOverlaps(const RangeIRQuery &q, const unsigned int off, RelationId &candidates, vector<RelationId> &vec_candidates)
{
    auto iterHL = this->hints.find(q.terms[off]);
    
    // If the inverted file does not contain the term then result must be empty
    if (iterHL == this->hints.end())
    {
        return this->tif->intersect_gOverlaps(q, off, vec_candidates[0]);
    }
    else
    {
        iterHL->second->intersect_gOverlaps(RangeQuery(q.id, q.start, q.end), candidates, vec_candidates);

        return (!candidates.empty());
    }
}


bool TemporalInvertedFile_HINTGammaH::intersect_mway_gOverlaps(const RangeIRQuery &q, const unsigned int off, vector<RelationId> &vec_candidates)
{
    auto iterL = this->hints.find(q.terms[off]);
    
    // If the inverted file does not contain the term then result must be empty
    if (iterL == this->hints.end())
    {
        return false;
    }
    else
    {
        iterL->second->intersect_mway_gOverlaps(RangeQuery(q.id, q.start, q.end), vec_candidates);

        return (!vec_candidates.empty());
    }
}


bool TemporalInvertedFile_HINTGammaH::intersectAndOutput_mway_gOverlaps(const RangeIRQuery &q, const unsigned int off, vector<RelationId> &vec_candidates, vector<RelationId> &vec_result)
{
    auto iterL = this->hints.find(q.terms[off]);
    
    // If the inverted file does not contain the term then result must be empty
    if (iterL == this->hints.end())
        return false;
    else
    {
        iterL->second->intersectAndOutput_mway_gOverlaps(RangeQuery(q.id, q.start, q.end), vec_candidates, vec_result);

        return (!vec_result.empty());
    }
}


bool TemporalInvertedFile_HINTGammaH::verify_gOverlaps(const RangeIRQuery &q, const unsigned int off, RelationId &candidates, RelationId &result)
{
    for (auto cid : candidates)
    {
        const IRecord &r = this->R->at(cid);
        if (r.containsTerms(q.terms, off))
            result.push_back(cid);
    }
    
    return (!result.empty());
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




#define V1

#if defined(V1)
void TemporalInvertedFile_HINTGammaH::executeContainment(const RangeIRQuery &q, RelationId &result)
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
//
#elif defined(V2)
void TemporalInvertedFile_HINTGammaH::executeContainment(const RangeIRQuery &q, RelationId &result)
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
void TemporalInvertedFile_HINTGammaH::executeContainment(const RangeIRQuery &q, RelationId &result)
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
void TemporalInvertedFile_HINTGammaH::executeContainment(const RangeIRQuery &q, RelationId &result)
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

void TemporalInvertedFile_HINTGammaH::executeContainment(const RangeIRQuery &q, vector<RelationId> &vec_result)
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

bool TemporalInvertedFile_HINTGammaH::intersect2_gOverlaps(const RangeIRQuery &q, const unsigned int termoff, RelationId &candidates)
{
    auto iterL = this->hints.find(q.terms[termoff]);
    
    // If the inverted file does not contain the term then result must be empty
    if (iterL == this->hints.end())
        return false;
    else
    {
        iterL->second->intersect2_gOverlaps(RangeQuery(q.id, q.start, q.end), candidates);

        return (!candidates.empty());
    }
}

bool TemporalInvertedFile_HINTGammaH::intersect2AndOutput_mway_gOverlaps(const RangeIRQuery &q, const unsigned int termoff, RelationId &candidates, RelationId &result)
{
    auto iterL = this->hints.find(q.terms[termoff]);
    
    // If the inverted file does not contain the term then result must be empty
    if (iterL == this->hints.end())
        return false;
    else
    {
        iterL->second->intersect2AndOutput_gOverlaps(RangeQuery(q.id, q.start, q.end), candidates, result);

        return (!candidates.empty());
    }

}

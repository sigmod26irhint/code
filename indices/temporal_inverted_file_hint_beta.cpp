#include "temporal_inverted_files.h"



TemporalInvertedFile_HINTBeta::TemporalInvertedFile_HINTBeta()
{
}


TemporalInvertedFile_HINTBeta::TemporalInvertedFile_HINTBeta(const IRelation &R, const unsigned int numBits = 0) : InvertedFileTemplate()
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
//        cout << "t" << iterL->first << ": " << iterL->second.size() << " interval(s)" << endl;
#ifdef USE_SS
        this->hlists.insert(make_pair(iterL->first, new HINT_M_SubsSort_SS_CM(iterL->second, numBits)));
#else
        this->hlists.insert(make_pair(iterL->first, new HINT_M_SubsSort_CM(iterL->second, numBits)));
#endif
//        cout << "\tHINT constructed" << endl;
    }
//    cout<<"done"<<endl;

    tmplists.clear();
}


TemporalInvertedFile_HINTBeta::TemporalInvertedFile_HINTBeta(const IRelation &R, const IRelation &U, const TermId dictionarySize, const unsigned int numBits = 0) : InvertedFileTemplate()
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
            this->hlists.insert(make_pair(iterL->first, new HINT_M_SubsSort_SS_CM(iterL->second, numBits)));
#else
            this->hlists.insert(make_pair(iterL->first, new HINT_M_SubsSort_CM(iterL->second, numBits)));
#endif
        }
        else
        {
#ifdef USE_SS
            this->hlists.insert(make_pair(iterL->first, new HINT_M_SubsSort_SS_CM(iterL->second, i->second, numBits)));
#else
            this->hlists.insert(make_pair(iterL->first, new HINT_M_SubsSort_CM(iterL->second, i->second, numBits)));
#endif
        }
    }

    for (auto iterL = tmpUlists.begin(); iterL != tmpUlists.end(); iterL++)
    {
        auto i = tmplists.find(iterL->first);
        if (i == tmplists.end())
        {
#ifdef USE_SS
            this->hlists.insert(make_pair(iterL->first, new HINT_M_SubsSort_SS_CM(Relation(), iterL->second, numBits)));
#else
            this->hlists.insert(make_pair(iterL->first, new HINT_M_SubsSort_CM(Relation(), iterL->second, numBits)));
#endif
        }
    }
    
    tmplists.clear();
    tmpUlists.clear();
}


void TemporalInvertedFile_HINTBeta::getStats()
{
//    size_t sum = 0;
//
//    this->numEmptyLists = this->R->dictionarySize - this->hlists.size();
//    for (auto iter = this->hlists.begin(); iter != this->hlists.end(); iter++)
//        sum += iter->second.size();
//    this->avgListSize = (float)sum/this->hlists.size();
}


size_t TemporalInvertedFile_HINTBeta::getSize()
{
    size_t size = 0;
    
    for (auto iterL = this->hlists.begin(); iterL != this->hlists.end(); iterL++)
        size += iterL->second->getSize();
    
    return size;
}


TemporalInvertedFile_HINTBeta::~TemporalInvertedFile_HINTBeta()
{
    for (auto iterL = this->hlists.begin(); iterL != this->hlists.end(); iterL++)
        delete iterL->second;
}


void TemporalInvertedFile_HINTBeta::print(char c)
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
void TemporalInvertedFile_HINTBeta::insert(IRelation &U)
{
    for (auto &r : U)
    {
        for (auto &tid : r.terms)
            this->hlists[tid]->insert(Record(r.id, r.start, r.end));
    }
}



// Querying
bool TemporalInvertedFile_HINTBeta::moveOut_gOverlaps(const RangeIRQuery &q, RelationId &candidates)
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


bool TemporalInvertedFile_HINTBeta::intersect_gOverlaps(const RangeIRQuery &q, const unsigned int off, RelationId &candidates)
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


bool TemporalInvertedFile_HINTBeta::intersectAndOutput_gOverlaps(const RangeIRQuery &q, const unsigned int off, RelationId &candidates, RelationId &result)
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


//bool TemporalInvertedFile_HINTBeta::verify_gOverlaps(const RangeIRQuery &q, const unsigned int off, RelationId &candidates, RelationId &result)
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


void TemporalInvertedFile_HINTBeta::executeContainment(const RangeIRQuery &q, RelationId &result)
{
#ifdef PROFILING
    Timer tim;
    double secsMO = 0, secsS = 0, secsI = 0, secsIO = 0;
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
        secsMO += tim.stop();
        cout<<"q" << q.id << ": t = " << q.terms[0] << "\t|C| = " << candidates.size() << endl;
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
        
        sort(candidates.begin(), candidates.end());
        
#ifdef PROFILING
        secsS += tim.stop();
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
            cout<<"q" << q.id << ": t = " << q.terms[i] << "\t|C| = " << candidates.size() << endl;
            tim.start();
#endif
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
            
            sort(candidates.begin(), candidates.end());
            
#ifdef PROFILING
            secsI += tim.stop();
#endif
        }
        
#ifdef PROFILING
        tim.start();
#endif
        this->intersectAndOutput_gOverlaps(q, numQueryTerms-1, candidates, result);
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


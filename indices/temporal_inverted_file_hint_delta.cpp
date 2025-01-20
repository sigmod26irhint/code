#include "temporal_inverted_files.h"



TemporalInvertedFile_HINTDelta::TemporalInvertedFile_HINTDelta()
{
}
unsigned int determineOptimalNumBitsForHINT_M(const RecordId numRecords, const Timestamp domainSize, Timestamp avgRecordExtent, const float qe_precentage);

// Undecided if we should use two passes
TemporalInvertedFile_HINTDelta::TemporalInvertedFile_HINTDelta(const IRelation &R, const unsigned int numBits = 0) : InvertedFileTemplate()
{
    unordered_map<TermId, Relation>  tmplists;


    this->R = &R;
    this->iidx = new InvertedFile(R);
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


    for (auto iterL = tmplists.begin(); iterL != tmplists.end(); iterL++)
    {
        iterL->second.domainSize = iterL->second.gend-iterL->second.gstart;
//        iterL->second.gstart = R.gstart;
//        iterL->second.gend = R.gend;
//        cout << "t" << iterL->first << ": " << iterL->second.size() << " interval(s) << m = " << determineOptimalNumBitsForHINT_M(iterL->second.size(), iterL->second.domainSize, iterL->second.avgRecordExtent, 0.1) << endl;
        this->hidxs.insert(make_pair(iterL->first, new HINT_M_SubsSort_SS_CM(iterL->second, numBits)));
//        cout << "\tHINT constructed" << endl;
    }
//    cout<<"done"<<endl;

    tmplists.clear();
}


void TemporalInvertedFile_HINTDelta::getStats()
{
//    size_t sum = 0;
//
//    this->numEmptyLists = this->R->dictionarySize - this->hlists.size();
//    for (auto iter = this->hlists.begin(); iter != this->hlists.end(); iter++)
//        sum += iter->second.size();
//    this->avgListSize = (float)sum/this->hlists.size();
}


size_t TemporalInvertedFile_HINTDelta::getSize()
{
    size_t size = 0;

    size += this->iidx->getSize();
    for (auto iterL = this->hidxs.begin(); iterL != this->hidxs.end(); iterL++)
        size += iterL->second->getSize();
    
    return size;
}


TemporalInvertedFile_HINTDelta::~TemporalInvertedFile_HINTDelta()
{
    delete this->iidx;
    
    for (auto iterL = this->hidxs.begin(); iterL != this->hidxs.end(); iterL++)
        delete iterL->second;
}


void TemporalInvertedFile_HINTDelta::print(char c)
{
//    for (auto iter = this->dlists.begin(); iter != this->dlists.end(); iter++)
//    {
//        cout << "t" << iter->first << " (" << iter->second.size() << "):";
//        for (auto &r : iter->second)
//            cout << " " << c << r.id;
//        cout << endl;
//    }
}


bool TemporalInvertedFile_HINTDelta::moveOut_gOverlaps(const RangeIRQuery &q, RelationId &candidates)
{
    auto iterL = this->hidxs.find(q.terms[0]);
    
    // If the inverted file does not contain the term then result must be empty
    if (iterL == this->hidxs.end())
        return false;
    else
    {
        iterL->second->moveOut_gOverlaps(RangeQuery(q.id, q.start, q.end), candidates);

        return (!candidates.empty());
    }
}


bool TemporalInvertedFile_HINTDelta::intersect_gOverlaps(const RangeIRQuery &q, const unsigned int off, RelationId &candidates)
{
    return this->iidx->intersect(q, off, candidates);
}


void TemporalInvertedFile_HINTDelta::intersectAndOutput_gOverlaps(const RangeIRQuery &q, const unsigned int off, RelationId &candidates, RelationId &result)
{
    this->iidx->intersectAndOutput(q, off, candidates, result);
}


void TemporalInvertedFile_HINTDelta::executeContainment(const RangeIRQuery &q, RelationId &result)
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

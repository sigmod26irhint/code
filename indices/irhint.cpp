#include "irhint.h"



void irHINTa::updateCounters(const IRecord &r)
{
    int level = 0;
    Timestamp a = r.start >> (this->maxBits-this->numBits);
    Timestamp b = r.end   >> (this->maxBits-this->numBits);
    Timestamp prevb;
    int firstfound = 0, lastfound = 0;
    

    while (level < this->height && a <= b)
    {
        if (a%2)
        { //last bit of a is 1
            if (firstfound)
            {
                if ((a == b) && (!lastfound))
                {
                    for (auto &tid : r.terms)
                        this->pRepsIn_lsizes[level][a][tid]++;
                    lastfound = 1;
                }
                else
                {
                    for (auto &tid : r.terms)
                        this->pRepsAft_lsizes[level][a][tid]++;
                }
            }
            else
            {
                if ((a == b) && (!lastfound))
                {
                    for (auto &tid : r.terms)
                        this->pOrgsIn_lsizes[level][a][tid]++;
                }
                else
                {
                    for (auto &tid : r.terms)
                        this->pOrgsAft_lsizes[level][a][tid]++;
                }
                firstfound = 1;
            }
            a++;
        }
        if (!(b%2))
        { //last bit of b is 0
            prevb = b;
            b--;
            if ((!firstfound) && b < a)
            {
                if (!lastfound)
                {
                    for (auto &tid : r.terms)
                        this->pOrgsIn_lsizes[level][prevb][tid]++;
                }
                else
                {
                    for (auto &tid : r.terms)
                        this->pOrgsAft_lsizes[level][prevb][tid]++;
                }
            }
            else
            {
                if (!lastfound)
                {
                    for (auto &tid : r.terms)
                        this->pRepsIn_lsizes[level][prevb][tid]++;
                    lastfound = 1;
                }
                else
                {
                    for (auto &tid : r.terms)
                        this->pRepsAft_lsizes[level][prevb][tid]++;
                }
            }
        }
        a >>= 1; // a = a div 2
        b >>= 1; // b = b div 2
        level++;
    }
}


void irHINTa::updatePartitions(const IRecord &r)
{
    int level = 0;
    Timestamp a = r.start >> (this->maxBits-this->numBits);
    Timestamp b = r.end   >> (this->maxBits-this->numBits);
    Timestamp prevb;
    int firstfound = 0, lastfound = 0;
    

    while (level < this->height && a <= b)
    {
        if (a%2)
        { //last bit of a is 1
            if (firstfound)
            {
                if ((a == b) && (!lastfound))
                {
                    this->pRepsIn[level][a].index(r);
                    lastfound = 1;
                }
                else
                {
                    this->pRepsAft[level][a].index(r);
                }
            }
            else
            {
                if ((a == b) && (!lastfound))
                {
                    this->pOrgsIn[level][a].index(r);
                }
                else
                {
                    this->pOrgsAft[level][a].index(r);
                }
                firstfound = 1;
            }
            a++;
        }
        if (!(b%2))
        { //last bit of b is 0
            prevb = b;
            b--;
            if ((!firstfound) && b < a)
            {
                if (!lastfound)
                {
                    this->pOrgsIn[level][prevb].index(r);
                }
                else
                {
                    this->pOrgsAft[level][prevb].index(r);
                }
            }
            else
            {
                if (!lastfound)
                {
                    this->pRepsIn[level][prevb].index(r);
                    lastfound = 1;
                }
                else
                {
                    this->pRepsAft[level][prevb].index(r);
                }
            }
        }
        a >>= 1; // a = a div 2
        b >>= 1; // b = b div 2
        level++;
    }
}


irHINTa::irHINTa(const IRelation &R, const unsigned int numBits = 0) : HierarchicalIRIndex(R, numBits)
{
#ifdef CONSTRUCTION_TWO_PASSES
    // Step 1: one pass to count the contents inside each partition.
    this->pOrgsIn_lsizes  = new TermId**[this->height];
    this->pOrgsAft_lsizes = new TermId**[this->height];
    this->pRepsIn_lsizes  = new TermId**[this->height];
    this->pRepsAft_lsizes = new TermId**[this->height];

    for (auto l = 0; l < this->height; l++)
    {
        auto cnt = (int)(pow(2, this->numBits-l));
        
        TermId *lsizes = new TermId[R.dictionarySize];
        this->pOrgsIn_lsizes[l]  = new TermId*[cnt];
        this->pOrgsAft_lsizes[l] = new TermId*[cnt];
        this->pRepsIn_lsizes[l]  = new TermId*[cnt];
        this->pRepsAft_lsizes[l] = new TermId*[cnt];
        
        for (auto p = 0; p < cnt; p++)
        {
            //calloc allocates memory and sets each counter to 0
            this->pOrgsIn_lsizes[l][p]  = (TermId *)calloc(R.dictionarySize, sizeof(TermId));
            this->pOrgsAft_lsizes[l][p] = (TermId *)calloc(R.dictionarySize, sizeof(TermId));
            this->pRepsIn_lsizes[l][p]  = (TermId *)calloc(R.dictionarySize, sizeof(TermId));
            this->pRepsAft_lsizes[l][p] = (TermId *)calloc(R.dictionarySize, sizeof(TermId));
        }
    }

    for (const IRecord &r : R)
        this->updateCounters(r);
#endif

    // Step 2: allocate necessary memory
    this->pOrgsIn  = new TemporalInvertedFile*[this->height];
    this->pOrgsAft = new TemporalInvertedFile*[this->height];
    this->pRepsIn  = new TemporalInvertedFile*[this->height];
    this->pRepsAft = new TemporalInvertedFile*[this->height];
    for (auto l = 0; l < this->height; l++)
    {
        auto cnt = (int)(pow(2, this->numBits-l));
        
        this->pOrgsIn[l]  = new TemporalInvertedFile[cnt];
        this->pOrgsAft[l] = new TemporalInvertedFile[cnt];
        this->pRepsIn[l]  = new TemporalInvertedFile[cnt];
        this->pRepsAft[l] = new TemporalInvertedFile[cnt];

#ifdef CONSTRUCTION_TWO_PASSES
        for (auto p = 0; p < cnt; p++)
        {
            for (TermId tid = 0; tid < R.dictionarySize; tid++)
            {
                if (this->pOrgsIn_lsizes[l][p][tid] > 0)
                    this->pOrgsIn[l][p].lists[tid].reserve(this->pOrgsIn_lsizes[l][p][tid]);
                if (this->pOrgsAft_lsizes[l][p][tid] > 0)
                    this->pOrgsAft[l][p].lists[tid].reserve(this->pOrgsAft_lsizes[l][p][tid]);
                if (this->pRepsIn_lsizes[l][p][tid] > 0)
                    this->pRepsIn[l][p].lists[tid].reserve(this->pRepsIn_lsizes[l][p][tid]);
                if (this->pRepsAft_lsizes[l][p][tid] > 0)
                    this->pRepsAft[l][p].lists[tid].reserve(this->pRepsAft_lsizes[l][p][tid]);
            }
        }
#endif
    }
    
    // Step 3: fill partitions.
    for (const IRecord &r : R)
        this->updatePartitions(r);
    

#ifdef CONSTRUCTION_TWO_PASSES
    // Free auxiliary memory.
    for (auto l = 0; l < this->height; l++)
    {
        auto cnt = (int)(pow(2, this->numBits-l));
        
        for (auto p = 0; p < cnt; p++)
        {
            free(this->pOrgsIn_lsizes[l][p]);
            free(this->pOrgsAft_lsizes[l][p]);
            free(this->pRepsIn_lsizes[l][p]);
            free(this->pRepsAft_lsizes[l][p]);
        }

        delete[] this->pOrgsIn_lsizes[l];
        delete[] this->pOrgsAft_lsizes[l];
        delete[] this->pRepsIn_lsizes[l];
        delete[] this->pRepsAft_lsizes[l];
    }
    delete[] this->pOrgsIn_lsizes;
    delete[] this->pOrgsAft_lsizes;
    delete[] this->pRepsIn_lsizes;
    delete[] this->pRepsAft_lsizes;
#endif
}


irHINTa::irHINTa(const IRelation &R, const IRelation &U, const TermId dictionarySize, const unsigned int numBits = 0) : HierarchicalIRIndex(R, U, numBits)
{
    // Step 1: one pass to count the contents inside each partition.
    this->pOrgsIn_lsizes  = new TermId**[this->height];
    this->pOrgsAft_lsizes = new TermId**[this->height];
    this->pRepsIn_lsizes  = new TermId**[this->height];
    this->pRepsAft_lsizes = new TermId**[this->height];

    for (auto l = 0; l < this->height; l++)
    {
        auto cnt = (int)(pow(2, this->numBits-l));
        
        TermId *lsizes = new TermId[dictionarySize];
        this->pOrgsIn_lsizes[l]  = new TermId*[cnt];
        this->pOrgsAft_lsizes[l] = new TermId*[cnt];
        this->pRepsIn_lsizes[l]  = new TermId*[cnt];
        this->pRepsAft_lsizes[l] = new TermId*[cnt];
        
        for (auto p = 0; p < cnt; p++)
        {
            //calloc allocates memory and sets each counter to 0
            this->pOrgsIn_lsizes[l][p]  = (TermId *)calloc(dictionarySize, sizeof(TermId));
            this->pOrgsAft_lsizes[l][p] = (TermId *)calloc(dictionarySize, sizeof(TermId));
            this->pRepsIn_lsizes[l][p]  = (TermId *)calloc(dictionarySize, sizeof(TermId));
            this->pRepsAft_lsizes[l][p] = (TermId *)calloc(dictionarySize, sizeof(TermId));
        }
    }

    for (const IRecord &r : R)
        this->updateCounters(r);
    for (const IRecord &r : U)
        this->updateCounters(r);

    // Step 2: allocate necessary memory
    this->pOrgsIn  = new TemporalInvertedFile*[this->height];
    this->pOrgsAft = new TemporalInvertedFile*[this->height];
    this->pRepsIn  = new TemporalInvertedFile*[this->height];
    this->pRepsAft = new TemporalInvertedFile*[this->height];
    for (auto l = 0; l < this->height; l++)
    {
        auto cnt = (int)(pow(2, this->numBits-l));
        
        this->pOrgsIn[l]  = new TemporalInvertedFile[cnt];
        this->pOrgsAft[l] = new TemporalInvertedFile[cnt];
        this->pRepsIn[l]  = new TemporalInvertedFile[cnt];
        this->pRepsAft[l] = new TemporalInvertedFile[cnt];
        
        for (auto p = 0; p < cnt; p++)
        {
            for (TermId tid = 0; tid < dictionarySize; tid++)
            {
                if (this->pOrgsIn_lsizes[l][p][tid] > 0)
                    this->pOrgsIn[l][p].lists[tid].reserve(this->pOrgsIn_lsizes[l][p][tid]);
                if (this->pOrgsAft_lsizes[l][p][tid] > 0)
                    this->pOrgsAft[l][p].lists[tid].reserve(this->pOrgsAft_lsizes[l][p][tid]);
                if (this->pRepsIn_lsizes[l][p][tid] > 0)
                    this->pRepsIn[l][p].lists[tid].reserve(this->pRepsIn_lsizes[l][p][tid]);
                if (this->pRepsAft_lsizes[l][p][tid] > 0)
                    this->pRepsAft[l][p].lists[tid].reserve(this->pRepsAft_lsizes[l][p][tid]);
            }
        }
    }
    
    // Step 3: fill partitions.
    for (const IRecord &r : R)
        this->updatePartitions(r);
    

    // Free auxiliary memory.
    for (auto l = 0; l < this->height; l++)
    {
        auto cnt = (int)(pow(2, this->numBits-l));
        
        for (auto p = 0; p < cnt; p++)
        {
            free(this->pOrgsIn_lsizes[l][p]);
            free(this->pOrgsAft_lsizes[l][p]);
            free(this->pRepsIn_lsizes[l][p]);
            free(this->pRepsAft_lsizes[l][p]);
        }

        delete[] this->pOrgsIn_lsizes[l];
        delete[] this->pOrgsAft_lsizes[l];
        delete[] this->pRepsIn_lsizes[l];
        delete[] this->pRepsAft_lsizes[l];
    }
    delete[] this->pOrgsIn_lsizes;
    delete[] this->pOrgsAft_lsizes;
    delete[] this->pRepsIn_lsizes;
    delete[] this->pRepsAft_lsizes;
}


void irHINTa::getStats()
{
//    size_t sum = 0;
//    for (auto l = 0; l < this->height; l++)
//    {
//        auto cnt = pow(2, this->numBits-l);
//        
//        this->numPartitions += cnt;
//        for (int pid = 0; pid < cnt; pid++)
//        {
//            this->numOriginalsIn  += this->pOrgsIn[l][pid].size();
//            this->numOriginalsAft += this->pOrgsAft[l][pid].size();
//            this->numReplicasIn   += this->pRepsIn[l][pid].size();
//            this->numReplicasAft  += this->pRepsAft[l][pid].size();
//            if ((this->pOrgsIn[l][pid].empty()) && (this->pOrgsAft[l][pid].empty()) && (this->pRepsIn[l][pid].empty()) && (this->pRepsAft[l][pid].empty()))
//                this->numEmptyPartitions++;
//        }
//    }
//    
//    this->avgPartitionSize = (float)(this->numIndexedRecords+this->numReplicasIn+this->numReplicasAft)/(this->numPartitions-numEmptyPartitions);
}


size_t irHINTa::getSize()
{
    size_t size = 0;
    
    for (auto l = 0; l < this->height; l++)
    {
        auto cnt = pow(2, this->numBits-l);
        
        for (int pid = 0; pid < cnt; pid++)
        {
            size += this->pOrgsIn[l][pid].getSize();
            size += this->pOrgsAft[l][pid].getSize();
            size += this->pRepsIn[l][pid].getSize();
            size += this->pRepsAft[l][pid].getSize();
        }
    }
    
    return size;
}


irHINTa::~irHINTa()
{
    for (auto l = 0; l < this->height; l++)
    {
        delete[] this->pOrgsIn[l];
        delete[] this->pOrgsAft[l];
        delete[] this->pRepsIn[l];
        delete[] this->pRepsAft[l];
    }

    delete this->pOrgsIn;
    delete this->pOrgsAft;
    delete this->pRepsIn;
    delete this->pRepsAft;
}



// Updating
void irHINTa::insert(IRelation &U)
{
    for (auto &r : U)
        this->updatePartitions(r);
}



// Querying
inline void irHINTa::scanPartitionContainment_CheckBoth_gOverlaps(TemporalInvertedFile &IF, const RangeIRQuery &q, RelationId &result)
{
    RelationId candidates;
    auto numQueryTerms = q.terms.size();
    
    if (numQueryTerms == 1)
        IF.moveOut_CheckBoth_gOverlaps(q, result);
    else
    {
        if (!IF.moveOut_CheckBoth_gOverlaps(q, candidates))
            return;
        
        for (auto i = 1; i < numQueryTerms-1; i++)
        {
            if (!IF.intersect_gOverlaps(q, i, candidates))
                return;
        }
        
        IF.intersectAndOutput_gOverlaps(q, numQueryTerms-1, candidates, result);
    }
}

inline void irHINTa::scanPartitionContainment_CheckStart_gOverlaps(TemporalInvertedFile &IF, const RangeIRQuery &q, RelationId &result)
{
    RelationId candidates;
    auto numQueryTerms = q.terms.size();
    
    if (numQueryTerms == 1)
        IF.moveOut_CheckStart_gOverlaps(q, result);
    else
    {
        if (!IF.moveOut_CheckStart_gOverlaps(q, candidates))
            return;
        
        for (auto i = 1; i < numQueryTerms-1; i++)
        {
            if (!IF.intersect_gOverlaps(q, i, candidates))
                return;
        }
        
        IF.intersectAndOutput_gOverlaps(q, numQueryTerms-1, candidates, result);
    }
}

inline void irHINTa::scanPartitionContainment_CheckEnd_gOverlaps(TemporalInvertedFile &IF, const RangeIRQuery &q, RelationId &result)
{
    RelationId candidates;
    auto numQueryTerms = q.terms.size();
   
    if (numQueryTerms == 1)
        IF.moveOut_CheckEnd_gOverlaps(q, result);
    else
    {
        if (!IF.moveOut_CheckEnd_gOverlaps(q, candidates))
            return;
        
        for (auto i = 1; i < numQueryTerms-1; i++)
        {
            if (!IF.intersect_gOverlaps(q, i, candidates))
                return;
        }
        
        IF.intersectAndOutput_gOverlaps(q, numQueryTerms-1, candidates, result);
    }
}

inline void irHINTa::scanPartitionContainment_NoChecks_gOverlaps(TemporalInvertedFile &IF, const RangeIRQuery &q, RelationId &result)
{
    RelationId candidates;
    auto numQueryTerms = q.terms.size();
   
    if (numQueryTerms == 1)
        IF.moveOut_NoChecks_gOverlaps(q, result);
    else
    {
        if (!IF.moveOut_NoChecks_gOverlaps(q, candidates))
            return;
        
        for (auto i = 1; i < numQueryTerms-1; i++)
        {
            if (!IF.intersect_gOverlaps(q, i, candidates))
                return;
        }
        
        IF.intersectAndOutput_gOverlaps(q, numQueryTerms-1, candidates, result);
    }
}


void irHINTa::executeContainment(const RangeIRQuery &q, RelationId &result)
{
    Timestamp a = q.start >> (this->maxBits-this->numBits); // prefix
    Timestamp b = q.end   >> (this->maxBits-this->numBits); // prefix
    bool foundzero = false;
    bool foundone = false;

        
    for (auto l = 0; l < this->numBits; l++)
    {
//        cout << "l = " << l << "\ta = " << a << endl;
        if (foundone && foundzero)
        {
            // Partition totally covers lowest-level partition range that includes query range
            // all contents are guaranteed to be results
            
            // Handle the partition that contains a: consider both originals and replicas
            this->scanPartitionContainment_NoChecks_gOverlaps(this->pRepsIn[l][a], q, result);
            this->scanPartitionContainment_NoChecks_gOverlaps(this->pRepsAft[l][a], q, result);

            // Handle rest: consider only originals
            for (auto i = a; i <= b; i++)
            {
                this->scanPartitionContainment_NoChecks_gOverlaps(this->pOrgsIn[l][i], q, result);
                this->scanPartitionContainment_NoChecks_gOverlaps(this->pOrgsAft[l][i], q, result);
            }
        }
        else
        {
            // Comparisons needed
            
            // Handle the partition that contains a: consider both originals and replicas, comparisons needed
            if (a == b)
            {
                this->scanPartitionContainment_CheckBoth_gOverlaps(this->pOrgsIn[l][a], q, result);
                this->scanPartitionContainment_CheckStart_gOverlaps(this->pOrgsAft[l][a], q, result);
            }

            else
            {
                // Lemma 1
                this->scanPartitionContainment_CheckEnd_gOverlaps(this->pOrgsIn[l][a], q, result);
                this->scanPartitionContainment_NoChecks_gOverlaps(this->pOrgsAft[l][a], q, result);
            }

            // Lemma 1, 3
            this->scanPartitionContainment_CheckEnd_gOverlaps(this->pRepsIn[l][a], q, result);
            this->scanPartitionContainment_NoChecks_gOverlaps(this->pRepsAft[l][a], q, result);

            if (a < b)
            {
                // Handle the rest before the partition that contains b: consider only originals, no comparisons needed
                for (auto i = a+1; i < b; i++)
                {
                    this->scanPartitionContainment_NoChecks_gOverlaps(this->pOrgsIn[l][i], q, result);
                    this->scanPartitionContainment_NoChecks_gOverlaps(this->pOrgsAft[l][i], q, result);
                }

                // Handle the partition that contains b: consider only originals, comparisons needed
                this->scanPartitionContainment_CheckStart_gOverlaps(this->pOrgsIn[l][b], q, result);
                this->scanPartitionContainment_CheckStart_gOverlaps(this->pOrgsAft[l][b], q, result);
            }

            if (b%2) //last bit of b is 1
                foundone = 1;
            if (!(a%2)) //last bit of a is 0
                foundzero = 1;
        }
        a >>= 1; // a = a div 2
        b >>= 1; // b = b div 2
    }
    
    
    // Handle root.
    if (foundone && foundzero)
    {
        // All contents are guaranteed to be results
        this->scanPartitionContainment_NoChecks_gOverlaps(this->pOrgsIn[this->numBits][0], q, result);
    }
    else
    {
        // Comparisons needed
        this->scanPartitionContainment_CheckBoth_gOverlaps(this->pOrgsIn[this->numBits][0], q, result);
    }
}


//
//
//
//inline void irHINTa_v2::updateCounters(const IRecord &r)
//{
//    int level = 0;
//    Timestamp a = r.start >> (this->maxBits-this->numBits);
//    Timestamp b = r.end   >> (this->maxBits-this->numBits);
//    Timestamp prevb;
//    int firstfound = 0, lastfound = 0;
//
//
//    while (level < this->height && a <= b)
//    {
//        if (a%2)
//        { //last bit of a is 1
//            if (firstfound)
//            {
//                if ((a == b) && (!lastfound))
//                {
//                    this->pRepsIn_sizes[level][a]++;
//                    lastfound = 1;
//                }
//                else
//                    this->pRepsAft_sizes[level][a]++;
//            }
//            else
//            {
//                if ((a == b) && (!lastfound))
//                    this->pOrgsIn_sizes[level][a]++;
//                else
//                    this->pOrgsAft_sizes[level][a]++;
//                firstfound = 1;
//            }
//            a++;
//        }
//        if (!(b%2))
//        { //last bit of b is 0
//            prevb = b;
//            b--;
//            if ((!firstfound) && b < a)
//            {
//                if (!lastfound)
//                    this->pOrgsIn_sizes[level][prevb]++;
//                else
//                    this->pOrgsAft_sizes[level][prevb]++;
//            }
//            else
//            {
//                if (!lastfound)
//                {
//                    this->pRepsIn_sizes[level][prevb]++;
//                    lastfound = 1;
//                }
//                else
//                {
//                    this->pRepsAft_sizes[level][prevb]++;
//                }
//            }
//        }
//        a >>= 1; // a = a div 2
//        b >>= 1; // b = b div 2
//        level++;
//    }
//}


//
//// Comparators
//inline bool CompareTimestampPairsByStart(const pair<Timestamp, Timestamp> &lhs, const pair<Timestamp, Timestamp> &rhs)
//{
//    return (lhs.first < rhs.first);
//}
//
//inline bool CompareTimestampPairsByEnd(const pair<Timestamp, Timestamp> &lhs, const pair<Timestamp, Timestamp> &rhs)
//{
//    return (lhs.second < rhs.second);
//}
//
//
//



//#define V1
//#define V2
#define V3

#ifdef USE_SS
inline void irHINTb::updateCounters(const IRecord &r)
{
    int level = 0;
    Timestamp a = r.start >> (this->maxBits-this->numBits);
    Timestamp b = r.end   >> (this->maxBits-this->numBits);
    Timestamp prevb;
    int firstfound = 0, lastfound = 0;
    
    
    while (level < this->height && a <= b)
    {
        if (a%2)
        { //last bit of a is 1
            if (firstfound)
            {
                if ((a == b) && (!lastfound))
                {
                    this->pRepsIn_sizes[level][a]++;
                    lastfound = 1;
                }
                else
                    this->pRepsAft_sizes[level][a]++;
            }
            else
            {
                if ((a == b) && (!lastfound))
                    this->pOrgsIn_sizes[level][a]++;
                else
                    this->pOrgsAft_sizes[level][a]++;
                firstfound = 1;
            }
            a++;
        }
        if (!(b%2))
        { //last bit of b is 0
            prevb = b;
            b--;
            if ((!firstfound) && b < a)
            {
                if (!lastfound)
                    this->pOrgsIn_sizes[level][prevb]++;
                else
                    this->pOrgsAft_sizes[level][prevb]++;
            }
            else
            {
                if (!lastfound)
                {
                    this->pRepsIn_sizes[level][prevb]++;
                    lastfound = 1;
                }
                else
                {
                    this->pRepsAft_sizes[level][prevb]++;
                }
            }
        }
        a >>= 1; // a = a div 2
        b >>= 1; // b = b div 2
        level++;
    }
}


inline void irHINTb::updatePartitions(const IRecord &r)
{
    int level = 0;
    Timestamp a = r.start >> (this->maxBits-this->numBits);
    Timestamp b = r.end   >> (this->maxBits-this->numBits);
    Timestamp prevb;
    int firstfound = 0, lastfound = 0;
    

    while (level < this->height && a <= b)
    {
        if (a%2)
        { //last bit of a is 1
            if (firstfound)
            {
                if ((a == b) && (!lastfound))
                {
                    this->pRepsInTmp[level][this->pRepsIn_offsets[level][a]++] = Record(r.id, r.start, r.end);
                    this->pRepsIn[level][a].index(r);
                    lastfound = 1;
                }
                else
                {
                    this->pRepsAftTmp[level][this->pRepsAft_offsets[level][a]++] = Record(r.id, r.start, r.end);
                    this->pRepsAft[level][a].index(r);
                }
            }
            else
            {
                if ((a == b) && (!lastfound))
                {
                    this->pOrgsInTmp[level][this->pOrgsIn_offsets[level][a]++] = Record(r.id, r.start, r.end);
                    this->pOrgsIn[level][a].index(r);
                }
                else
                {
                    this->pOrgsAftTmp[level][this->pOrgsAft_offsets[level][a]++] = Record(r.id, r.start, r.end);
                    this->pOrgsAft[level][a].index(r);
                }
                firstfound = 1;
            }
            a++;
        }
        if (!(b%2))
        { //last bit of b is 0
            prevb = b;
            b--;
            if ((!firstfound) && b < a)
            {
                if (!lastfound)
                {
                    this->pOrgsInTmp[level][this->pOrgsIn_offsets[level][prevb]++] = Record(r.id, r.start, r.end);
                    this->pOrgsIn[level][prevb].index(r);
                }
                else
                {
                    this->pOrgsAftTmp[level][this->pOrgsAft_offsets[level][prevb]++] = Record(r.id, r.start, r.end);
                    this->pOrgsAft[level][prevb].index(r);
                }
            }
            else
            {
                if (!lastfound)
                {
                    this->pRepsInTmp[level][this->pRepsIn_offsets[level][prevb]++] = Record(r.id, r.start, r.end);
                    this->pRepsIn[level][prevb].index(r);
                    lastfound = 1;
                }
                else
                {
                    this->pRepsAftTmp[level][this->pRepsAft_offsets[level][prevb]++] = Record(r.id, r.start, r.end);
                    this->pRepsAft[level][prevb].index(r);
                }
            }
        }
        a >>= 1; // a = a div 2
        b >>= 1; // b = b div 2
        level++;
    }
}


irHINTb::irHINTb(const IRelation &R, const unsigned int numBits = 0) : HierarchicalIRIndex(R, numBits)
{
    OffsetEntry_SS_CM dummySE;
    Offsets_SS_CM_Iterator iterSEO, iterSEOBegin, iterSEOEnd;
    PartitionId tmp = -1;
    
    
    // Step 1: one pass to count the contents inside each partition.
    this->pOrgsIn_sizes  = (RecordId **)malloc(this->height*sizeof(RecordId *));
    this->pOrgsAft_sizes = (RecordId **)malloc(this->height*sizeof(RecordId *));
    this->pRepsIn_sizes  = (size_t **)malloc(this->height*sizeof(size_t *));
    this->pRepsAft_sizes = (size_t **)malloc(this->height*sizeof(size_t *));
    this->pOrgsIn_offsets  = (RecordId **)malloc(this->height*sizeof(RecordId *));
    this->pOrgsAft_offsets = (RecordId **)malloc(this->height*sizeof(RecordId *));
    this->pRepsIn_offsets  = (size_t **)malloc(this->height*sizeof(size_t *));
    this->pRepsAft_offsets = (size_t **)malloc(this->height*sizeof(size_t *));
    
    for (auto l = 0; l < this->height; l++)
    {
        auto cnt = (int)(pow(2, this->numBits-l));
        
        //calloc allocates memory and sets each counter to 0
        this->pOrgsIn_sizes[l]  = (RecordId *)calloc(cnt, sizeof(RecordId));
        this->pOrgsAft_sizes[l] = (RecordId *)calloc(cnt, sizeof(RecordId));
        this->pRepsIn_sizes[l]  = (size_t *)calloc(cnt, sizeof(size_t));
        this->pRepsAft_sizes[l] = (size_t *)calloc(cnt, sizeof(size_t));
        this->pOrgsIn_offsets[l]  = (RecordId *)calloc(cnt+1, sizeof(RecordId));
        this->pOrgsAft_offsets[l] = (RecordId *)calloc(cnt+1, sizeof(RecordId));
        this->pRepsIn_offsets[l]  = (size_t *)calloc(cnt+1, sizeof(size_t));
        this->pRepsAft_offsets[l] = (size_t *)calloc(cnt+1, sizeof(size_t));
    }
    
    for (const IRecord &r : R)
        this->updateCounters(r);
    
    
    // Step 2: allocate necessary memory.
    this->pOrgsInTmp  = new Relation[this->height];
    this->pOrgsAftTmp = new Relation[this->height];
    this->pRepsInTmp  = new Relation[this->height];
    this->pRepsAftTmp = new Relation[this->height];

    this->pOrgsIn  = new InvertedFile*[this->height];
    this->pOrgsAft = new InvertedFile*[this->height];
    this->pRepsIn  = new InvertedFile*[this->height];
    this->pRepsAft = new InvertedFile*[this->height];

    for (auto l = 0; l < this->height; l++)
    {
        auto cnt = (int)(pow(2, this->numBits-l));
        size_t sumOin = 0, sumOaft = 0, sumRin = 0, sumRaft = 0;
        
        for (auto pId = 0; pId < cnt; pId++)
        {
            this->pOrgsIn_offsets[l][pId]  = sumOin;
            this->pOrgsAft_offsets[l][pId] = sumOaft;
            this->pRepsIn_offsets[l][pId]  = sumRin;
            this->pRepsAft_offsets[l][pId] = sumRaft;
            sumOin  += this->pOrgsIn_sizes[l][pId];
            sumOaft += this->pOrgsAft_sizes[l][pId];
            sumRin  += this->pRepsIn_sizes[l][pId];
            sumRaft += this->pRepsAft_sizes[l][pId];
        }
        this->pOrgsIn_offsets[l][cnt]  = sumOin;
        this->pOrgsAft_offsets[l][cnt] = sumOaft;
        this->pRepsIn_offsets[l][cnt]  = sumRin;
        this->pRepsAft_offsets[l][cnt] = sumRaft;
        
        this->pOrgsInTmp[l].resize(sumOin);
        this->pOrgsAftTmp[l].resize(sumOaft);
        this->pRepsInTmp[l].resize(sumRin);
        this->pRepsAftTmp[l].resize(sumRaft);

        this->pOrgsIn[l]  = new InvertedFile[cnt];
        this->pOrgsAft[l] = new InvertedFile[cnt];
        this->pRepsIn[l]  = new InvertedFile[cnt];
        this->pRepsAft[l] = new InvertedFile[cnt];
    }
    
    
    // Step 3: fill partitions.
    for (const IRecord &r : R)
        this->updatePartitions(r);
    
    
    // Step 4: sort partition contents; first need to reset the offsets
    for (auto l = 0; l < this->height; l++)
    {
        auto cnt = (int)(pow(2, this->numBits-l));
        size_t sumOin = 0, sumOaft = 0, sumRin = 0, sumRaft = 0;
        
        for (auto pId = 0; pId < cnt; pId++)
        {
            this->pOrgsIn_offsets[l][pId]  = sumOin;
            this->pOrgsAft_offsets[l][pId] = sumOaft;
            this->pRepsIn_offsets[l][pId]  = sumRin;
            this->pRepsAft_offsets[l][pId] = sumRaft;
            sumOin  += this->pOrgsIn_sizes[l][pId];
            sumOaft += this->pOrgsAft_sizes[l][pId];
            sumRin  += this->pRepsIn_sizes[l][pId];
            sumRaft += this->pRepsAft_sizes[l][pId];
        }
        this->pOrgsIn_offsets[l][cnt]  = sumOin;
        this->pOrgsAft_offsets[l][cnt] = sumOaft;
        this->pRepsIn_offsets[l][cnt]  = sumRin;
        this->pRepsAft_offsets[l][cnt] = sumRaft;
    }
    
    for (auto l = 0; l < this->height; l++)
    {
        auto cnt = (int)(pow(2, this->numBits-l));
        for (auto pId = 0; pId < cnt; pId++)
        {
            sort(this->pOrgsInTmp[l].begin()+this->pOrgsIn_offsets[l][pId], this->pOrgsInTmp[l].begin()+this->pOrgsIn_offsets[l][pId+1]);
            sort(this->pOrgsAftTmp[l].begin()+this->pOrgsAft_offsets[l][pId], this->pOrgsAftTmp[l].begin()+this->pOrgsAft_offsets[l][pId+1]);
            sort(this->pRepsInTmp[l].begin()+this->pRepsIn_offsets[l][pId], this->pRepsInTmp[l].begin()+this->pRepsIn_offsets[l][pId+1], compareRecordsByEnd);
            sort(this->pRepsAftTmp[l].begin()+this->pRepsAft_offsets[l][pId], this->pRepsAftTmp[l].begin()+this->pRepsAft_offsets[l][pId+1], compareRecordsByEnd);
        }
    }
    
    
    // Step 5: break-down data to create id- and timestamp-dedicated arrays; free auxiliary memory.
    this->pOrgsInIds  = new RelationId[this->height];
    this->pOrgsAftIds = new RelationId[this->height];
    this->pRepsInIds  = new RelationId[this->height];
    this->pRepsAftIds = new RelationId[this->height];
    this->pOrgsInTimestamps  = new vector<pair<Timestamp, Timestamp> >[this->height];
    this->pOrgsAftTimestamps = new vector<pair<Timestamp, Timestamp> >[this->height];
    this->pRepsInTimestamps  = new vector<pair<Timestamp, Timestamp> >[this->height];
    this->pRepsAftTimestamps = new vector<pair<Timestamp, Timestamp> >[this->height];
    for (auto l = 0; l < this->height; l++)
    {
        auto cnt = pOrgsInTmp[l].size();
        
        this->pOrgsInIds[l].resize(cnt);
        this->pOrgsInTimestamps[l].resize(cnt);
        for (auto j = 0; j < cnt; j++)
        {
            this->pOrgsInIds[l][j] = this->pOrgsInTmp[l][j].id;
            this->pOrgsInTimestamps[l][j].first = this->pOrgsInTmp[l][j].start;
            this->pOrgsInTimestamps[l][j].second = this->pOrgsInTmp[l][j].end;
        }
        
        cnt = pOrgsAftTmp[l].size();
        this->pOrgsAftIds[l].resize(cnt);
        this->pOrgsAftTimestamps[l].resize(cnt);
        for (auto j = 0; j < cnt; j++)
        {
            this->pOrgsAftIds[l][j] = this->pOrgsAftTmp[l][j].id;
            this->pOrgsAftTimestamps[l][j].first = this->pOrgsAftTmp[l][j].start;
            this->pOrgsAftTimestamps[l][j].second = this->pOrgsAftTmp[l][j].end;
        }
        
        cnt = pRepsInTmp[l].size();
        this->pRepsInIds[l].resize(cnt);
        this->pRepsInTimestamps[l].resize(cnt);
        for (auto j = 0; j < cnt; j++)
        {
            this->pRepsInIds[l][j] = this->pRepsInTmp[l][j].id;
            this->pRepsInTimestamps[l][j].first = this->pRepsInTmp[l][j].start;
            this->pRepsInTimestamps[l][j].second = this->pRepsInTmp[l][j].end;
        }

        cnt = pRepsAftTmp[l].size();
        this->pRepsAftIds[l].resize(cnt);
        this->pRepsAftTimestamps[l].resize(cnt);
        for (auto j = 0; j < cnt; j++)
        {
            this->pRepsAftIds[l][j] = this->pRepsAftTmp[l][j].id;
            this->pRepsAftTimestamps[l][j].first = this->pRepsAftTmp[l][j].start;
            this->pRepsAftTimestamps[l][j].second = this->pRepsAftTmp[l][j].end;
        }
    }
    
    
    // Free auxiliary memory
    for (auto l = 0; l < this->height; l++)
    {
        free(this->pOrgsIn_offsets[l]);
        free(this->pOrgsAft_offsets[l]);
        free(this->pRepsIn_offsets[l]);
        free(this->pRepsAft_offsets[l]);
    }
    free(this->pOrgsIn_offsets);
    free(this->pOrgsAft_offsets);
    free(this->pRepsIn_offsets);
    free(this->pRepsAft_offsets);
    
    delete[] this->pOrgsInTmp;
    delete[] this->pOrgsAftTmp;
    delete[] this->pRepsInTmp;
    delete[] this->pRepsAftTmp;
    

    // Step 4: create offset pointers
    this->pOrgsIn_ioffsets  = new Offsets_SS_CM[this->height];
    this->pOrgsAft_ioffsets = new Offsets_SS_CM[this->height];
    this->pRepsIn_ioffsets  = new Offsets_SS_CM[this->height];
    this->pRepsAft_ioffsets = new Offsets_SS_CM[this->height];
    for (int l = this->height-1; l > -1; l--)
    {
        auto cnt = (int)(pow(2, this->numBits-l));
        size_t sumOin = 0, sumOaft = 0, sumRin = 0, sumRaft = 0;
        
        for (auto pId = 0; pId < cnt; pId++)
        {
            bool isEmpty = true;
            
            dummySE.tstamp = pId >> 1;//((pId >> (this->maxBits-this->numBits)) >> 1);
            if (this->pOrgsIn_sizes[l][pId] > 0)
            {
                isEmpty = false;
                tmp = -1;
                if (l < this->height-1)
                {
                    iterSEOBegin = this->pOrgsIn_ioffsets[l+1].begin();
                    iterSEOEnd = this->pOrgsIn_ioffsets[l+1].end();
                    iterSEO = lower_bound(iterSEOBegin, iterSEOEnd, dummySE);//, CompareOffsetsByTimestamp);
                    tmp = (iterSEO != iterSEOEnd)? (iterSEO-iterSEOBegin): -1;
                }
                this->pOrgsIn_ioffsets[l].push_back(OffsetEntry_SS_CM(pId, this->pOrgsInIds[l].begin()+sumOin, this->pOrgsInTimestamps[l].begin()+sumOin, tmp));
            }
            
            dummySE.tstamp = pId >> 1;//((pId >> (this->maxBits-this->numBits)) >> 1);
            if (this->pOrgsAft_sizes[l][pId] > 0)
            {
                isEmpty = false;
                tmp = -1;
                if (l < this->height-1)
                {
                    iterSEOBegin = this->pOrgsAft_ioffsets[l+1].begin();
                    iterSEOEnd = this->pOrgsAft_ioffsets[l+1].end();
                    iterSEO = lower_bound(iterSEOBegin, iterSEOEnd, dummySE);//, CompareOffsetsByTimestamp);
                    tmp = (iterSEO != iterSEOEnd)? (iterSEO-iterSEOBegin): -1;
                }
                this->pOrgsAft_ioffsets[l].push_back(OffsetEntry_SS_CM(pId, this->pOrgsAftIds[l].begin()+sumOaft, this->pOrgsAftTimestamps[l].begin()+sumOaft, tmp));
            }
            
            dummySE.tstamp = pId >> 1;//((pId >> (this->maxBits-this->numBits)) >> 1);
            if (this->pRepsIn_sizes[l][pId] > 0)
            {
                isEmpty = false;
                tmp = -1;
                if (l < this->height-1)
                {
                    iterSEOBegin = this->pRepsIn_ioffsets[l+1].begin();
                    iterSEOEnd = this->pRepsIn_ioffsets[l+1].end();
                    iterSEO = lower_bound(iterSEOBegin, iterSEOEnd, dummySE);//, CompareOffsetsByTimestamp);
                    tmp = (iterSEO != iterSEOEnd)? (iterSEO-iterSEOBegin): -1;
                }
                this->pRepsIn_ioffsets[l].push_back(OffsetEntry_SS_CM(pId, this->pRepsInIds[l].begin()+sumRin, this->pRepsInTimestamps[l].begin()+sumRin, tmp));
            }
            
            dummySE.tstamp = pId >> 1;//((pId >> (this->maxBits-this->numBits)) >> 1);
            if (this->pRepsAft_sizes[l][pId] > 0)
            {
                isEmpty = false;
                tmp = -1;
                if (l < this->height-1)
                {
                    iterSEOBegin = this->pRepsAft_ioffsets[l+1].begin();
                    iterSEOEnd = this->pRepsAft_ioffsets[l+1].end();
                    iterSEO = lower_bound(iterSEOBegin, iterSEOEnd, dummySE);//, CompareOffsetsByTimestamp);
                    tmp = (iterSEO != iterSEOEnd)? (iterSEO-iterSEOBegin): -1;
                }
                this->pRepsAft_ioffsets[l].push_back(OffsetEntry_SS_CM(pId, this->pRepsAftIds[l].begin()+sumRaft, this->pRepsAftTimestamps[l].begin()+sumRaft, tmp));
            }
            
            sumOin += this->pOrgsIn_sizes[l][pId];
            sumOaft += this->pOrgsAft_sizes[l][pId];
            sumRin += this->pRepsIn_sizes[l][pId];
            sumRaft += this->pRepsAft_sizes[l][pId];
            
            if (isEmpty)
                this->numEmptyPartitions++;
        }
    }
    
    
    // Free auxliary memory
    for (auto l = 0; l < this->height; l++)
    {
        free(this->pOrgsIn_sizes[l]);
        free(this->pOrgsAft_sizes[l]);
        free(this->pRepsIn_sizes[l]);
        free(this->pRepsAft_sizes[l]);
    }
    free(this->pOrgsIn_sizes);
    free(this->pOrgsAft_sizes);
    free(this->pRepsIn_sizes);
    free(this->pRepsAft_sizes);
}


void irHINTb::getStats()
{
//    size_t sum = 0;
//    for (auto l = 0; l < this->height; l++)
//    {
//        auto cnt = pow(2, this->numBits-l);
//        
//        this->numPartitions += cnt;
//
//        this->numOriginalsIn  += this->pOrgsInIds[l].size();
//        this->numOriginalsAft += this->pOrgsAftIds[l].size();
//        this->numReplicasIn   += this->pRepsInIds[l].size();
//        this->numReplicasAft  += this->pRepsAftIds[l].size();
//    }
//    
//    this->avgPartitionSize = (float)(this->numIndexedRecords+this->numReplicasIn+this->numReplicasAft)/(this->numPartitions-numEmptyPartitions);
}


size_t irHINTb::getSize()
{
    size_t size = 0;
    
    for (auto l = 0; l < this->height; l++)
    {
        auto cnt = pow(2, this->numBits-l);
        
        size += this->pOrgsInIds[l].size() * (sizeof(RecordId)+sizeof(Timestamp)+sizeof(Timestamp));
        size += this->pOrgsAftIds[l].size() * (sizeof(RecordId)+sizeof(Timestamp)+sizeof(Timestamp));
        size += this->pRepsInIds[l].size() * (sizeof(RecordId)+sizeof(Timestamp)+sizeof(Timestamp));
        size += this->pRepsAftIds[l].size() * (sizeof(RecordId)+sizeof(Timestamp)+sizeof(Timestamp));

        for (int pid = 0; pid < cnt; pid++)
        {
            size += this->pOrgsIn[l][pid].getSize();
            size += this->pOrgsAft[l][pid].getSize();
            size += this->pRepsIn[l][pid].getSize();
            size += this->pRepsAft[l][pid].getSize();
        }
    }
    
    return size;
}


irHINTb::~irHINTb()
{
    delete[] this->pOrgsIn_ioffsets;
    delete[] this->pOrgsAft_ioffsets;
    delete[] this->pRepsIn_ioffsets;
    delete[] this->pRepsAft_ioffsets;
    
    delete[] this->pOrgsInIds;
    delete[] this->pOrgsInTimestamps;
    delete[] this->pOrgsAftIds;
    delete[] this->pOrgsAftTimestamps;
    delete[] this->pRepsInIds;
    delete[] this->pRepsInTimestamps;
    delete[] this->pRepsAftIds;
    delete[] this->pRepsAftTimestamps;
    
    for (auto l = 0; l < this->height; l++)
    {
        delete[] this->pOrgsIn[l];
        delete[] this->pOrgsAft[l];
        delete[] this->pRepsIn[l];
        delete[] this->pRepsAft[l];
    }

    delete this->pOrgsIn;
    delete this->pOrgsAft;
    delete this->pRepsIn;
    delete this->pRepsAft;
}


// Auxiliary functions to determine exactly how to scan a partition.
inline bool irHINTb::getBounds(unsigned int level, Timestamp t, PartitionId &next_from, Offsets_SS_CM *ioffsets, vector<pair<Timestamp, Timestamp> > *timestamps, vector<pair<Timestamp, Timestamp> >::iterator &iterBegin, vector<pair<Timestamp, Timestamp> >::iterator &iterEnd, RelationIdIterator &iterI)
{
    OffsetEntry_SS_CM qdummy;
    Offsets_SS_CM_Iterator iterIO, iterIOBegin, iterIOEnd;
    size_t cnt = ioffsets[level].size();
    PartitionId from = next_from;

    if (cnt > 0)
    {
        // Do binary search or follow vertical pointers.
        if ((from == -1) || (from > cnt))
        {
            qdummy.tstamp = t;
            iterIOBegin = ioffsets[level].begin();
            iterIOEnd = ioffsets[level].end();

            iterIO = lower_bound(iterIOBegin, iterIOEnd, qdummy);//, CompareOffsetsByTimestamp);
            if ((iterIO != iterIOEnd) && (iterIO->tstamp == t))
            {
                iterI = iterIO->iterI;
                iterBegin = iterIO->iterT;
                iterEnd = ((iterIO+1 != iterIOEnd) ? (iterIO+1)->iterT : timestamps[level].end());

                next_from =iterIO->pid;
                
                return true;
            }
            else
                return false;
        }
        else
        {
            Timestamp tmp = (ioffsets[level][from]).tstamp;
            if (tmp < t)
            {
                while (((ioffsets[level][from]).tstamp < t) && (from < cnt))
                    from++;
            }
            else if (tmp > t)
            {
                while (((ioffsets[level][from]).tstamp > t) && (from > -1))
                    from--;
                if (((ioffsets[level][from]).tstamp != t) || (from == -1))
                    from++;
            }

            if ((from != cnt) && ((ioffsets[level][from]).tstamp == t))
            {
                iterI = (ioffsets[level][from]).iterI;
                iterBegin = (ioffsets[level][from]).iterT;
                iterEnd = ((from+1 != cnt) ? (ioffsets[level][from+1]).iterT : timestamps[level].end());

                next_from = (ioffsets[level][from]).pid;
                
                return true;
            }
            else
            {
                next_from = -1;

                return false;
            }
        }
    }
    else
    {
        next_from = -1;
        
        return false;
    }
}


inline bool irHINTb::getBounds(unsigned int level, Timestamp t, PartitionId &next_from, Offsets_SS_CM *ioffsets, RelationId *ids, RelationIdIterator &iterIBegin, RelationIdIterator &iterIEnd)
{
    OffsetEntry_SS_CM qdummy;
    Offsets_SS_CM_Iterator iterIO, iterIO2, iterIOBegin, iterIOEnd;
    size_t cnt = ioffsets[level].size();
    PartitionId from = next_from;


    if (cnt > 0)
    {
        if ((from == -1) || (from > cnt))
        {
            qdummy.tstamp = t;
            iterIOBegin = ioffsets[level].begin();
            iterIOEnd = ioffsets[level].end();
            iterIO = lower_bound(iterIOBegin, iterIOEnd, qdummy);//, CompareOffsetsByTimestamp);
            if ((iterIO != iterIOEnd) && (iterIO->tstamp == t))
            {
                iterIBegin = iterIO->iterI;
                iterIEnd = ((iterIO+1 != iterIOEnd) ? (iterIO+1)->iterI : ids[level].end());

                next_from =iterIO->pid;
                
                return true;
            }
            else
                return false;
        }
        else
        {
            Timestamp tmp = (ioffsets[level][from]).tstamp;
            if (tmp < t)
            {
                while (((ioffsets[level][from]).tstamp < t) && (from < cnt))
                    from++;
            }
            else if (tmp > t)
            {
                while (((ioffsets[level][from]).tstamp > t) && (from > -1))
                    from--;
                if (((ioffsets[level][from]).tstamp != t) || (from == -1))
                    from++;
            }

            if ((from != cnt) && ((ioffsets[level][from]).tstamp == t))
            {
                iterIBegin = (ioffsets[level][from]).iterI;
                iterIEnd = ((from+1 != cnt) ? (ioffsets[level][from+1]).iterI : ids[level].end());

                next_from = (ioffsets[level][from]).pid;
                
                return true;
            }
            else
            {
                next_from = -1;
                
                return false;
            }
        }
    }
    else
    {
        next_from = -1;
        
        return false;
    }
}


inline bool irHINTb::getBounds(unsigned int level, Timestamp ts, Timestamp te, PartitionId &next_from, PartitionId &next_to, Offsets_SS_CM *ioffsets, vector<pair<Timestamp, Timestamp> > *timestamps, vector<pair<Timestamp, Timestamp> >::iterator &iterBegin, vector<pair<Timestamp, Timestamp> >::iterator &iterEnd, RelationIdIterator &iterI)
{
    OffsetEntry_SS_CM qdummyA, qdummyB;
    Offsets_SS_CM_Iterator iterIO, iterIO2, iterIOBegin, iterIOEnd;
    size_t cnt = ioffsets[level].size();
    PartitionId from = next_from, to = next_to;


    if (cnt > 0)
    {
        // Do binary search or follow vertical pointers.
        if ((from == -1) || (to == -1))
        {
            qdummyA.tstamp = ts;
            iterIOBegin = ioffsets[level].begin();
            iterIOEnd = ioffsets[level].end();
            iterIO = lower_bound(iterIOBegin, iterIOEnd, qdummyA);//, CompareOffsetsByTimestamp);
            if ((iterIO != iterIOEnd) && (iterIO->tstamp <= te))
            {
                next_from =iterIO->pid;

                qdummyB.tstamp = te;
                iterI = iterIO->iterI;
                iterBegin = iterIO->iterT;

                iterIO2 = upper_bound(iterIO, iterIOEnd, qdummyB);//, CompareOffsetsByTimestamp);
//                iterIO2 = iterIO;
//                while ((iterIO2 != iterIOEnd) && (get<0>(*iterIO2) <= te))
//                    iterIO2++;

                iterEnd = ((iterIO2 != iterIOEnd) ? iterEnd = iterIO2->iterT: timestamps[level].end());

                if (iterIO2 != iterIOEnd)
                    next_to = iterIO2->pid;
                else
                    next_to = -1;
                
                return true;
            }
            else
            {
                next_from = -1;
                
                return false;
            }
        }
        else
        {
            Timestamp tmp = (ioffsets[level][from]).tstamp;
            if (tmp < ts)
            {
                while (((ioffsets[level][from]).tstamp < ts) && (from < cnt))
                    from++;
            }
            else if (tmp > ts)
            {
                while (((ioffsets[level][from]).tstamp > ts) && (from > -1))
                    from--;
                if (((ioffsets[level][from]).tstamp != ts) || (from == -1))
                    from++;
            }

            tmp = (ioffsets[level][to]).tstamp;
            if (tmp > te)
            {
                while (((ioffsets[level][to]).tstamp > te) && (to > -1))
                    to--;
                to++;
            }
//                else if (tmp <= te)
            else if (tmp == te)
            {
                while (((ioffsets[level][to]).tstamp <= te) && (to < cnt))
                    to++;
            }

            if ((from != cnt) && (from != -1) && (from < to))
            {
                iterI = (ioffsets[level][from]).iterI;
                iterBegin = (ioffsets[level][from]).iterT;
                iterEnd   = (to != cnt)? (ioffsets[level][to]).iterT: timestamps[level].end();

                next_from = (ioffsets[level][from]).pid;
                next_to   = (to != cnt) ? (ioffsets[level][to]).pid  : -1;
                
                return true;
            }
            else
            {
                next_from = next_to = -1;
             
                return false;
            }
        }
    }
    else
    {
        next_from = -1;
        next_to = -1;
        
        return false;
    }
}


inline bool irHINTb::getBounds(unsigned int level, Timestamp ts, Timestamp te, PartitionId &next_from, PartitionId &next_to, Offsets_SS_CM *ioffsets, RelationId *ids, RelationIdIterator &iterIBegin, RelationIdIterator &iterIEnd)
{
    OffsetEntry_SS_CM qdummyTS, qdummyTE;
    Offsets_SS_CM_Iterator iterIO, iterIO2, iterIOBegin, iterIOEnd;
    size_t cnt = ioffsets[level].size();
    PartitionId from = next_from, to = next_to;


    if (cnt > 0)
    {
        from = next_from;
        to = next_to;

        // Do binary search or follow vertical pointers.
        if ((from == -1) || (to == -1))
        {
            qdummyTS.tstamp = ts;
            iterIOBegin = ioffsets[level].begin();
            iterIOEnd = ioffsets[level].end();
            iterIO = lower_bound(iterIOBegin, iterIOEnd, qdummyTS);//, CompareOffsetsByTimestamp);
            if ((iterIO != iterIOEnd) && (iterIO->tstamp <= te))
            {
                next_from =iterIO->pid;

                qdummyTE.tstamp = te;
                iterIBegin = iterIO->iterI;

                iterIO2 = upper_bound(iterIO, iterIOEnd, qdummyTE);//, CompareOffsetsByTimestamp);
//                iterIO2 = iterIO;
//                while ((iterIO2 != iterIOEnd) && (get<0>(*iterIO2) <= b))
//                    iterIO2++;

                iterIEnd = ((iterIO2 != iterIOEnd) ? iterIEnd = iterIO2->iterI: ids[level].end());

                if (iterIO2 != iterIOEnd)
                    next_to = iterIO2->pid;
                else
                    next_to = -1;
                
                return true;
            }
            else
            {
                next_from = -1;
                
                return false;
            }
        }
        else
        {
            Timestamp tmp = (ioffsets[level][from]).tstamp;
            if (tmp < ts)
            {
                while (((ioffsets[level][from]).tstamp < ts) && (from < cnt))
                    from++;
            }
            else if (tmp > ts)
            {
                while (((ioffsets[level][from]).tstamp > ts) && (from > -1))
                    from--;
                if (((ioffsets[level][from]).tstamp != ts) || (from == -1))
                    from++;
            }

            tmp = (ioffsets[level][to]).tstamp;
            if (tmp > te)
            {
                while (((ioffsets[level][to]).tstamp > te) && (to > -1))
                    to--;
                to++;
            }
//                else if (tmp <= b)
            else if (tmp == te)
            {
                while (((ioffsets[level][to]).tstamp <= te) && (to < cnt))
                    to++;
            }

            if ((from != cnt) && (from != -1) && (from < to))
            {
                iterIBegin = (ioffsets[level][from]).iterI;
                iterIEnd   = (to != cnt)? (ioffsets[level][to]).iterI: ids[level].end();

                next_from = (ioffsets[level][from]).pid;
                next_to   = (to != cnt) ? (ioffsets[level][to]).pid  : -1;
                
                return true;
            }
            else
            {
                next_from = next_to = -1;
                
                return false;
            }
        }
    }
    else
    {
        next_from = -1;
        next_to = -1;
        
        return false;
    }
}


inline void irHINTb::scanPartition_CheckBoth_gOverlaps(const unsigned int level, const Timestamp t, Offsets_SS_CM *ioffsets, vector<pair<Timestamp, Timestamp> > *timestamps, InvertedFile **ifs, bool (*compare)(const pair<Timestamp, Timestamp>&, const pair<Timestamp, Timestamp>&), const RangeIRQuery &q, PartitionId &next_from, RelationId &result)
{
    vector<pair<Timestamp, Timestamp> >::iterator iter, iterBegin, iterEnd;
    RelationIdIterator iterI;
    RelationId candidates;

#ifdef V1
    if (!ifs[level][t].moveOut(q, candidates))
        return;

    for (auto i = 1; i < q.terms.size(); i++)
    {
        if (!ifs[level][t].intersect(q, i, candidates))
            return;
    }

    if (this->getBounds(level, t, next_from, ioffsets, timestamps, iterBegin, iterEnd, iterI))
    {
        vector<pair<Timestamp, Timestamp> >::iterator pivot = lower_bound(iterBegin, iterEnd, make_pair(q.end+1, q.end+1), compare);
        for (iter = iterBegin; iter != pivot; iter++)
        {
            if ((q.start <= iter->second) && (binary_search(candidates.begin(), candidates.end(), *iterI)))
                result.push_back(*iterI);
            iterI++;
        }
    }
#elif defined(V2)
    if (this->getBounds(level, t, next_from, ioffsets, timestamps, iterBegin, iterEnd, iterI))
    {
        vector<pair<Timestamp, Timestamp> >::iterator pivot = lower_bound(iterBegin, iterEnd, make_pair(q.end+1, q.end+1), compare);
        for (iter = iterBegin; iter != pivot; iter++)
        {
            if (q.start <= iter->second)
                candidates.push_back(*iterI);
            iterI++;
        }
    }
    
    if (candidates.empty())
        return;
    
    sort(candidates.begin(), candidates.end());
    for (auto i = 0; i < q.terms.size()-1; i++)
    {
        if (!ifs[level][t].intersect(q, i, candidates))
            return;
    }
    
    // Update result
//    result.insert(result.end(), candidates.begin(), candidates.end());
    ifs[level][t].intersectAndOutput(q, q.terms.size()-1, candidates, result);
#elif defined(V3)
    if (!ifs[level][t].moveOut(q, candidates))
        return;

    for (auto i = 1; i < q.terms.size(); i++)
    {
        if (!ifs[level][t].intersect(q, i, candidates))
            return;
    }

    RelationId tmp;
    if (this->getBounds(level, t, next_from, ioffsets, timestamps, iterBegin, iterEnd, iterI))
    {
        vector<pair<Timestamp, Timestamp> >::iterator pivot = lower_bound(iterBegin, iterEnd, make_pair(q.end+1, q.end+1), compare);
        for (iter = iterBegin; iter != pivot; iter++)
        {
            if (q.start <= iter->second)
                tmp.push_back(*iterI);
            iterI++;
        }
    }
    if (tmp.empty())
        return;
    sort(tmp.begin(), tmp.end());

    auto iterC    = candidates.begin();
    auto iterCEnd = candidates.end();
    auto iterT    = tmp.begin();
    auto iterTEnd = tmp.end();
    while ((iterC != iterCEnd) && (iterT != iterTEnd))
    {
        if (*iterT < *iterC)
            iterT++;
        else if (*iterT > *iterC)
            iterC++;
        else
        {
            result.push_back(*iterT);
            iterT++;
        }
    }
#endif
}


inline void irHINTb::scanPartition_CheckStart_gOverlaps(const unsigned int level, const Timestamp t, Offsets_SS_CM *ioffsets, vector<pair<Timestamp, Timestamp> > *timestamps, InvertedFile **ifs, bool (*compare)(const pair<Timestamp, Timestamp>&, const pair<Timestamp, Timestamp>&), const RangeIRQuery &q, PartitionId &next_from, RelationId &result)
{
    vector<pair<Timestamp, Timestamp> >::iterator iter, iterBegin, iterEnd;
    RelationIdIterator iterI;
    RelationId candidates;

#ifdef V1
    if (!ifs[level][t].moveOut(q, candidates))
        return;

    for (auto i = 1; i < q.terms.size(); i++)
    {
        if (!ifs[level][t].intersect(q, i, candidates))
            return;
    }

    if (this->getBounds(level, t, next_from, ioffsets, timestamps, iterBegin, iterEnd, iterI))
    {
        vector<pair<Timestamp, Timestamp> >::iterator pivot = lower_bound(iterBegin, iterEnd, make_pair(q.end+1, q.end+1), compare);
        for (iter = iterBegin; iter != pivot; iter++)
        {
            if (binary_search(candidates.begin(), candidates.end(), *iterI))
                result.push_back(*iterI);

            iterI++;
        }
    }
#elif defined(V2)
    if (this->getBounds(level, t, next_from, ioffsets, timestamps, iterBegin, iterEnd, iterI))
    {
        vector<pair<Timestamp, Timestamp> >::iterator pivot = lower_bound(iterBegin, iterEnd, make_pair(q.end+1, q.end+1), compare);
        for (iter = iterBegin; iter != pivot; iter++)
        {
            candidates.push_back(*iterI);
            iterI++;
        }
    }
    
    if (candidates.empty())
        return;
    
    sort(candidates.begin(), candidates.end());
    for (auto i = 0; i < q.terms.size()-1; i++)
    {
        if (!ifs[level][t].intersect(q, i, candidates))
            return;
    }
    
    // Update result
//    result.insert(result.end(), candidates.begin(), candidates.end());
    ifs[level][t].intersectAndOutput(q, q.terms.size()-1, candidates, result);
#elif defined(V3)
    if (!ifs[level][t].moveOut(q, candidates))
        return;

    for (auto i = 1; i < q.terms.size(); i++)
    {
        if (!ifs[level][t].intersect(q, i, candidates))
            return;
    }

    RelationId tmp;
    if (this->getBounds(level, t, next_from, ioffsets, timestamps, iterBegin, iterEnd, iterI))
    {
        vector<pair<Timestamp, Timestamp> >::iterator pivot = lower_bound(iterBegin, iterEnd, make_pair(q.end+1, q.end+1), compare);
        for (iter = iterBegin; iter != pivot; iter++)
        {
            tmp.push_back(*iterI);
            iterI++;
        }
    }
    if (tmp.empty())
        return;
    sort(tmp.begin(), tmp.end());

    auto iterC    = candidates.begin();
    auto iterCEnd = candidates.end();
    auto iterT    = tmp.begin();
    auto iterTEnd = tmp.end();
    while ((iterC != iterCEnd) && (iterT != iterTEnd))
    {
        if (*iterT < *iterC)
            iterT++;
        else if (*iterT > *iterC)
            iterC++;
        else
        {
            result.push_back(*iterT);
            iterT++;
        }
    }
#endif
}


inline void irHINTb::scanPartition_CheckEnd_gOverlaps(const unsigned int level, const Timestamp t, Offsets_SS_CM *ioffsets, vector<pair<Timestamp, Timestamp> > *timestamps, InvertedFile **ifs, const RangeIRQuery &q, PartitionId &next_from, RelationId &result)
{
    vector<pair<Timestamp, Timestamp> >::iterator iter, iterBegin, iterEnd;
    RelationIdIterator iterI;
    RelationId candidates;

#ifdef V1
    if (!ifs[level][t].moveOut(q, candidates))
        return;

    for (auto i = 1; i < q.terms.size(); i++)
    {
        if (!ifs[level][t].intersect(q, i, candidates))
            return;
    }

    if (this->getBounds(level, t, next_from, ioffsets, timestamps, iterBegin, iterEnd, iterI))
    {
        for (iter = iterBegin; iter != iterEnd; iter++)
        {
            if ((q.start <= iter->second) && (binary_search(candidates.begin(), candidates.end(), *iterI)))
                result.push_back(*iterI);
            iterI++;
        }
    }
#elif defined(V2)
    if (this->getBounds(level, t, next_from, ioffsets, timestamps, iterBegin, iterEnd, iterI))
    {
        for (iter = iterBegin; iter != iterEnd; iter++)
        {
            if (q.start <= iter->second)
                candidates.push_back(*iterI);
            iterI++;
        }
    }
    if (candidates.empty())
        return;
    
    sort(candidates.begin(), candidates.end());
    for (auto i = 0; i < q.terms.size()-1; i++)
    {
        if (!ifs[level][t].intersect(q, i, candidates))
            return;
    }
    
    // Update result
//    result.insert(result.end(), candidates.begin(), candidates.end());
    ifs[level][t].intersectAndOutput(q, q.terms.size()-1, candidates, result);
#elif defined(V3)
    if (!ifs[level][t].moveOut(q, candidates))
        return;

    for (auto i = 1; i < q.terms.size(); i++)
    {
        if (!ifs[level][t].intersect(q, i, candidates))
            return;
    }

    RelationId tmp;
    if (this->getBounds(level, t, next_from, ioffsets, timestamps, iterBegin, iterEnd, iterI))
    {
        for (iter = iterBegin; iter != iterEnd; iter++)
        {
            if (q.start <= iter->second)
                tmp.push_back(*iterI);
            iterI++;
        }
    }
    if (tmp.empty())
        return;
    sort(tmp.begin(), tmp.end());

    auto iterC    = candidates.begin();
    auto iterCEnd = candidates.end();
    auto iterT    = tmp.begin();
    auto iterTEnd = tmp.end();
    while ((iterC != iterCEnd) && (iterT != iterTEnd))
    {
        if (*iterT < *iterC)
            iterT++;
        else if (*iterT > *iterC)
            iterC++;
        else
        {
            result.push_back(*iterT);
            iterT++;
        }
    }
#endif
}


inline void irHINTb::scanPartition_CheckEnd_gOverlaps(const unsigned int level, const Timestamp t, Offsets_SS_CM *ioffsets, vector<pair<Timestamp, Timestamp> > *timestamps, InvertedFile **ifs, bool (*compare)(const pair<Timestamp, Timestamp>&, const pair<Timestamp, Timestamp>&), const RangeIRQuery &q, PartitionId &next_from, RelationId &result)
{
    vector<pair<Timestamp, Timestamp> >::iterator iter, iterBegin, iterEnd;
    RelationIdIterator iterI;
    RelationId candidates;
    
#ifdef V1
    if (!ifs[level][t].moveOut(q, candidates))
        return;

    for (auto i = 1; i < q.terms.size(); i++)
    {
        if (!ifs[level][t].intersect(q, i, candidates))
            return;
    }

    if (this->getBounds(level, t, next_from, ioffsets, timestamps, iterBegin, iterEnd, iterI))
    {
        iter = lower_bound(iterBegin, iterEnd, make_pair(q.start, q.start), compare);
        iterI += iter-iterBegin;
        while (iter != iterEnd)
        {
            if (binary_search(candidates.begin(), candidates.end(), *iterI))
                result.push_back(*iterI);

            iter++;
            iterI++;
        }
    }
#elif defined(V2)
    if (this->getBounds(level, t, next_from, ioffsets, timestamps, iterBegin, iterEnd, iterI))
    {
        iter = lower_bound(iterBegin, iterEnd, make_pair(q.start, q.start), compare);
        iterI += iter-iterBegin;
        while (iter != iterEnd)
        {
            candidates.push_back(*iterI);

            iter++;
            iterI++;
        }
    }
    if (candidates.empty())
        return;
    
    sort(candidates.begin(), candidates.end());
    for (auto i = 0; i < q.terms.size()-1; i++)
    {
        if (!ifs[level][t].intersect(q, i, candidates))
            return;
    }
    
    // Update result
//    result.insert(result.end(), candidates.begin(), candidates.end());
    ifs[level][t].intersectAndOutput(q, q.terms.size()-1, candidates, result);
#elif defined(V3)
    if (!ifs[level][t].moveOut(q, candidates))
        return;

    for (auto i = 1; i < q.terms.size(); i++)
    {
        if (!ifs[level][t].intersect(q, i, candidates))
            return;
    }

    RelationId tmp;
    if (this->getBounds(level, t, next_from, ioffsets, timestamps, iterBegin, iterEnd, iterI))
    {
        iter = lower_bound(iterBegin, iterEnd, make_pair(q.start, q.start), compare);
        iterI += iter-iterBegin;
        while (iter != iterEnd)
        {
            tmp.push_back(*iterI);

            iter++;
            iterI++;
        }
    }
    if (tmp.empty())
        return;
    sort(tmp.begin(), tmp.end());

    auto iterC    = candidates.begin();
    auto iterCEnd = candidates.end();
    auto iterT    = tmp.begin();
    auto iterTEnd = tmp.end();
    while ((iterC != iterCEnd) && (iterT != iterTEnd))
    {
        if (*iterT < *iterC)
            iterT++;
        else if (*iterT > *iterC)
            iterC++;
        else
        {
            result.push_back(*iterT);
            iterT++;
        }
    }
#endif
}


inline void irHINTb::scanPartition_NoChecks_gOverlaps(const unsigned int level, const Timestamp t, Offsets_SS_CM *ioffsets, RelationId *ids, InvertedFile **ifs, const RangeIRQuery &q, PartitionId &next_from, RelationId &result)
{
    RelationId candidates;    

    if (!ifs[level][t].moveOut(q, candidates))
        return;

    for (auto i = 1; i < q.terms.size()-1; i++)
    {
        if (!ifs[level][t].intersect(q, i, candidates))
            return;
    }
    
    ifs[level][t].intersectAndOutput(q, q.terms.size()-1, candidates, result);

//#ifdef V1
//    if (!ifs[level][t].moveOut(q, candidates))
//        return;
//
//    for (auto i = 1; i < q.terms.size(); i++)
//    {
//        if (!ifs[level][t].intersect(q, i, candidates))
//            return;
//    }
//    cout << "C = {";
//    for (const RecordId &c: candidates)
//        cout << c << " ";
//    cout<<"}"<<endl;
//
//    if (this->getBounds(level, t, next_from, ioffsets, ids, iterIBegin, iterIEnd))
//    {
//        for (iterI = iterIBegin; iterI != iterIEnd; iterI++)
//        {
//            cout << "\tbs r" << (*iterI) << endl;
//            if (binary_search(candidates.begin(), candidates.end(), *iterI))
//                result.push_back(*iterI);
//        }
//    }
//#elif defined(V2)
//    if (this->getBounds(level, t, next_from, ioffsets, ids, iterIBegin, iterIEnd))
//    {
//        for (iterI = iterIBegin; iterI != iterIEnd; iterI++)
//            candidates.push_back(*iterI);
//    }
//    if (candidates.empty())
//        return;
//    
//    sort(candidates.begin(), candidates.end());
//    for (auto i = 0; i < q.terms.size(); i++)
//    {
//        if (!ifs[level][t].intersect(q, i, candidates))
//            return;
//    }
//    
//    // Update result
//    result.insert(result.end(), candidates.begin(), candidates.end());
//#elif defined(V3)
//    if (!ifs[level][t].moveOut(q, candidates))
//        return;
//
//    for (auto i = 1; i < q.terms.size(); i++)
//    {
//        if (!ifs[level][t].intersect(q, i, candidates))
//            return;
//    }
//
//    RelationId tmp;
//    if (this->getBounds(level, t, next_from, ioffsets, ids, iterIBegin, iterIEnd))
//    {
//        for (iterI = iterIBegin; iterI != iterIEnd; iterI++)
//                tmp.push_back(*iterI);
//    }
//    if (tmp.empty())
//        return;
//    sort(tmp.begin(), tmp.end());
//
//    auto iterC    = candidates.begin();
//    auto iterCEnd = candidates.end();
//    auto iterT    = tmp.begin();
//    auto iterTEnd = tmp.end();
//    while ((iterC != iterCEnd) && (iterT != iterTEnd))
//    {
//        if (*iterT < *iterC)
//            iterT++;
//        else if (*iterT > *iterC)
//            iterC++;
//        else
//        {
//            result.push_back(*iterT);
//            iterT++;
//        }
//    }
//#endif
}

//
//inline void irHINTb::scanPartitions_NoChecks_gOverlaps(const unsigned int level, const Timestamp ts, const Timestamp te, Offsets_SS_CM *ioffsets, RelationId *ids, InvertedFile **ifs, const RangeIRQuery &q, PartitionId &next_from, PartitionId &next_to, RelationId &result)
//{
//    RelationIdIterator iterI, iterIBegin, iterIEnd;
//    OffsetEntry_SS_CM qdummyTS, qdummyTE;
//    RelationId candidates;
//    
//#ifdef V1
//#elif defined(V2)
//    if (this->getBounds(level, ts, te, next_from, next_to, ioffsets, ids, iterIBegin, iterIEnd))
//    {
//        for (iterI = iterIBegin; iterI != iterIEnd; iterI++)
//            candidates.push_back(*iterI);
//    }
//    if (candidates.empty())
//        return;
//    
//    sort(candidates.begin(), candidates.end());
//    for (auto p = ts; p <= te; p++)
//    {
//        for (auto i = 0; i < q.terms.size(); i++)
//        {
//            if (!ifs[level][p].intersect(q, i, candidates))
//                break;
//        }
//    }
//    
//    // Update result
//    result.insert(result.end(), candidates.begin(), candidates.end());
//#elif defined(V3)
//#endif
//}



void irHINTb::executeContainment(const RangeIRQuery &q, RelationId &result)
{
    vector<pair<Timestamp, Timestamp> >::iterator iter, iterBegin, iterEnd;
    RelationIdIterator iterI, iterIBegin, iterIEnd;
    Timestamp a = q.start >> (this->maxBits-this->numBits); // prefix
    Timestamp b = q.end   >> (this->maxBits-this->numBits); // prefix
    bool foundzero = false;
    bool foundone = false;
    PartitionId next_fromOinA = -1, next_fromOaftA = -1, next_fromRinA = -1, next_fromRaftA = -1, next_fromOinB = -1, next_fromOaftB = -1, next_fromOinAB = -1, next_toOinAB = -1, next_fromOaftAB = -1, next_toOaftAB = -1, next_fromR = -1, next_fromO = -1, next_toO = -1;


    for (auto l = 0; l < this->numBits; l++)
    {
//        cout << "l = " << l << "\ta = " << a << endl;
        if (foundone && foundzero)
        {
            // Partition totally covers lowest-level partition range that includes query range
            // all contents are guaranteed to be results

            // Handle the partition that contains a: consider both originals and replicas
            this->scanPartition_NoChecks_gOverlaps(l, a, this->pRepsIn_ioffsets, this->pRepsInIds, this->pRepsIn, q, next_fromRinA, result);
            this->scanPartition_NoChecks_gOverlaps(l, a, this->pRepsAft_ioffsets, this->pRepsAftIds, this->pRepsAft, q, next_fromRaftA, result);

            for (auto i = a; i <= b; i++)
            {
                this->scanPartition_NoChecks_gOverlaps(l, i, this->pOrgsIn_ioffsets, this->pOrgsInIds, this->pOrgsIn, q, next_fromOinAB, result);
                this->scanPartition_NoChecks_gOverlaps(l, i, this->pOrgsAft_ioffsets, this->pOrgsAftIds, this->pOrgsAft, q, next_fromOaftAB, result);
            }
        }
        else
        {
            // Comparisons needed

            // Handle the partition that contains a: consider both originals and replicas, comparisons needed
            if (a == b)
            {
                // Special case when query overlaps only one partition, Lemma 3
                this->scanPartition_CheckBoth_gOverlaps(l, a, this->pOrgsIn_ioffsets, this->pOrgsInTimestamps, this->pOrgsIn, CompareTimestampPairsByStart, q, next_fromOinA, result);
                this->scanPartition_CheckStart_gOverlaps(l, a, this->pOrgsAft_ioffsets, this->pOrgsAftTimestamps, this->pOrgsAft, CompareTimestampPairsByStart, q, next_fromOaftA, result);
            }
            else
            {
                // Lemma 1
                this->scanPartition_CheckEnd_gOverlaps(l, a, this->pOrgsIn_ioffsets, this->pOrgsInTimestamps, this->pOrgsIn, q, next_fromOinA, result);
                this->scanPartition_NoChecks_gOverlaps(l, a, this->pOrgsAft_ioffsets, this->pOrgsAftIds, this->pOrgsAft, q, next_fromOaftA, result);
            }

            // Lemma 1, 3
            this->scanPartition_CheckEnd_gOverlaps(l, a, this->pRepsIn_ioffsets, this->pRepsInTimestamps, this->pRepsIn, CompareTimestampPairsByEnd, q, next_fromRinA, result);
            this->scanPartition_NoChecks_gOverlaps(l, a, this->pRepsAft_ioffsets, this->pRepsAftIds, this->pRepsAft, q, next_fromRaftA, result);

            if (a < b)
            {
                // Handle the rest before the partition that contains b: consider only originals, no comparisons needed
                for (auto i = a+1; i < b; i++)
                {
                    this->scanPartition_NoChecks_gOverlaps(l, i, this->pOrgsIn_ioffsets, this->pOrgsInIds, this->pOrgsIn, q, next_fromOinAB, result);
                    this->scanPartition_NoChecks_gOverlaps(l, i, this->pOrgsAft_ioffsets, this->pOrgsAftIds, this->pOrgsAft, q, next_fromOaftAB, result);
                }

                // Handle the partition that contains b: consider only originals, comparisons needed
                this->scanPartition_CheckStart_gOverlaps(l, b, this->pOrgsIn_ioffsets, this->pOrgsInTimestamps, this->pOrgsIn, CompareTimestampPairsByStart, q, next_fromOinB, result);
                this->scanPartition_CheckStart_gOverlaps(l, b, this->pOrgsAft_ioffsets, this->pOrgsAftTimestamps, this->pOrgsAft, CompareTimestampPairsByStart, q, next_fromOaftB, result);
            }

            if (b%2) //last bit of b is 1
                foundone = 1;
            if (!(a%2)) //last bit of a is 0
                foundzero = 1;
        }
        a >>= 1; // a = a div 2
        b >>= 1; // b = b div 2
    }

    // Handle root.
    // TODO
//    if (foundone && foundzero)
//    {
//        // All contents are guaranteed to be results
//        iterIBegin = this->pOrgsInIds[this->numBits].begin();
//        iterIEnd = this->pOrgsInIds[this->numBits].end();
//        for (iterI = iterIBegin; iterI != iterIEnd; iterI++)
//        {
//            IRecord &ir = (*this->iR)[*iterI];
//
//            if (ir.contains(Q.terms, 1))
//                result.push_back(*iterI);
//        }
//    }
//    else
//    {
//        // Comparisons needed
//        iterI = this->pOrgsInIds[this->numBits].begin();
//        iterBegin = this->pOrgsInTimestamps[this->numBits].begin();
//        iterEnd = lower_bound(iterBegin, this->pOrgsInTimestamps[this->numBits].end(), make_pair<Timestamp, Timestamp>(Q.end+1, Q.end+1), CompareTimestampPairsByStart);
//        for (iter = iterBegin; iter != iterEnd; iter++)
//        {
//            IRecord &ir = (*this->iR)[*iterI];
//
//            if ((Q.start <= iter->second) && (ir.contains(Q.terms, 1)))
//                result.push_back(*iterI);
//
//            iterI++;
//        }
//    }

//    for (const RecordId &rid : result)
//        cout << "r" << rid << endl;
}
#else
inline void irHINTb::updateCounters(const IRecord &r)
{
    int level = 0;
    Timestamp a = r.start >> (this->maxBits-this->numBits);
    Timestamp b = r.end   >> (this->maxBits-this->numBits);
    Timestamp prevb;
    int firstfound = 0, lastfound = 0;
    
    
    while (level < this->height && a <= b)
    {
        if (a%2)
        { //last bit of a is 1
            if (firstfound)
            {
                if ((a == b) && (!lastfound))
                {
                    this->pRepsIn_sizes[level][a]++;
                    lastfound = 1;
                }
                else
                    this->pRepsAft_sizes[level][a]++;
            }
            else
            {
                if ((a == b) && (!lastfound))
                    this->pOrgsIn_sizes[level][a]++;
                else
                    this->pOrgsAft_sizes[level][a]++;
                firstfound = 1;
            }
            a++;
        }
        if (!(b%2))
        { //last bit of b is 0
            prevb = b;
            b--;
            if ((!firstfound) && b < a)
            {
                if (!lastfound)
                    this->pOrgsIn_sizes[level][prevb]++;
                else
                    this->pOrgsAft_sizes[level][prevb]++;
            }
            else
            {
                if (!lastfound)
                {
                    this->pRepsIn_sizes[level][prevb]++;
                    lastfound = 1;
                }
                else
                {
                    this->pRepsAft_sizes[level][prevb]++;
                }
            }
        }
        a >>= 1; // a = a div 2
        b >>= 1; // b = b div 2
        level++;
    }
}


inline void irHINTb::updatePartitions(const IRecord &r)
{
    int level = 0;
    Timestamp a = r.start >> (this->maxBits-this->numBits);
    Timestamp b = r.end   >> (this->maxBits-this->numBits);
    Timestamp prevb;
    int firstfound = 0, lastfound = 0;

    
    while (level < this->height && a <= b)
    {
        if (a%2)
        { //last bit of a is 1
            if (firstfound)
            {
                if ((a == b) && (!lastfound))
                {
                    this->pRepsInTmp[level][a].emplace_back(r.id, r.start, r.end);
//                    this->pRepsInTmp[level][a].[this->pRepsIn_sizes[level][a]] = Record(r.id, r.start, r.end);
//                    this->pRepsIn_sizes[level][a]++;
                    this->pRepsIn[level][a].index(r);
                    lastfound = 1;
                }
                else
                {
                    this->pRepsAftTmp[level][a].emplace_back(r.id, r.start, r.end);
//                    this->pRepsAftTmp[level][a][this->pRepsAft_sizes[level][a]] = Record(r.id, r.start, r.end);;
//                    this->pRepsAft_sizes[level][a]++;
                    this->pRepsAft[level][a].index(r);
                }
            }
            else
            {
                if ((a == b) && (!lastfound))
                {
                    this->pOrgsInTmp[level][a].emplace_back(r.id, r.start, r.end);
//                    this->pOrgsInTmp[level][a][this->pOrgsIn_sizes[level][a]] = Record(r.id, r.start, r.end);
//                    this->pOrgsIn_sizes[level][a]++;
                    this->pOrgsIn[level][a].index(r);
                }
                else
                {
                    this->pOrgsAftTmp[level][a].emplace_back(r.id, r.start, r.end);
//                    this->pOrgsAftTmp[level][a][this->pOrgsAft_sizes[level][a]] = Record(r.id, r.start, r.end);
//                    this->pOrgsAft_sizes[level][a]++;
                    this->pOrgsAft[level][a].index(r);
                }
                firstfound = 1;
            }
            a++;
        }
        if (!(b%2))
        { //last bit of b is 0
            prevb = b;
            b--;
            if ((!firstfound) && b < a)
            {
                if (!lastfound)
                {
                    this->pOrgsInTmp[level][prevb].emplace_back(r.id, r.start, r.end);
//                    this->pOrgsInTmp[level][prevb][this->pOrgsIn_sizes[level][prevb]] = Record(r.id, r.start, r.end);
//                    this->pOrgsIn_sizes[level][prevb]++;
                    this->pOrgsIn[level][prevb].index(r);
                }
                else
                {
                    this->pOrgsAftTmp[level][prevb].emplace_back(r.id, r.start, r.end);
//                    this->pOrgsAftTmp[level][prevb][this->pOrgsAft_sizes[level][prevb]] = Record(r.id, r.start, r.end);
//                    this->pOrgsAft_sizes[level][prevb]++;
                    this->pOrgsAft[level][prevb].index(r);
                }
            }
            else
            {
                if (!lastfound)
                {
                    this->pRepsInTmp[level][prevb].emplace_back(r.id, r.start, r.end);
//                    this->pRepsInTmp[level][prevb][this->pRepsIn_sizes[level][prevb]] = Record(r.id, r.start, r.end);
//                    this->pRepsIn_sizes[level][prevb]++;
                    this->pRepsIn[level][prevb].index(r);
                    lastfound = 1;
                }
                else
                {
                    this->pRepsAftTmp[level][prevb].emplace_back(r.id, r.start, r.end);
//                    this->pRepsAftTmp[level][prevb][this->pRepsAft_sizes[level][prevb]] = Record(r.id, r.start, r.end);
//                    this->pRepsAft_sizes[level][prevb]++;
                    this->pRepsAft[level][prevb].index(r);
                }
            }
        }
        a >>= 1; // a = a div 2
        b >>= 1; // b = b div 2
        level++;
    }
}


irHINTb::irHINTb(const IRelation &R, const unsigned int numBits = 0) : HierarchicalIRIndex(R, numBits)
{
    // Step 1: one pass to count the contents inside each partition.
    this->pOrgsIn_sizes  = (RecordId **)malloc(this->height*sizeof(RecordId *));
    this->pOrgsAft_sizes = (RecordId **)malloc(this->height*sizeof(RecordId *));
    this->pRepsIn_sizes  = (RecordId **)malloc(this->height*sizeof(RecordId *));
    this->pRepsAft_sizes = (RecordId **)malloc(this->height*sizeof(RecordId *));
    
    for (auto l = 0; l < this->height; l++)
    {
        auto cnt = (int)(pow(2, this->numBits-l));
        
        //calloc allocates memory and sets each counter to 0
        this->pOrgsIn_sizes[l]  = (RecordId *)calloc(cnt, sizeof(RecordId));
        this->pOrgsAft_sizes[l] = (RecordId *)calloc(cnt, sizeof(RecordId));
        this->pRepsIn_sizes[l]  = (RecordId *)calloc(cnt, sizeof(RecordId));
        this->pRepsAft_sizes[l] = (RecordId *)calloc(cnt, sizeof(RecordId));
    }
    
    for (const IRecord &r : R)
        this->updateCounters(r);
    
    // Step 2: allocate necessary memory.
    this->pOrgsInTmp  = new Relation*[this->height];
    this->pOrgsAftTmp = new Relation*[this->height];
    this->pRepsInTmp  = new Relation*[this->height];
    this->pRepsAftTmp = new Relation*[this->height];
    
    this->pOrgsIn  = new InvertedFile*[this->height];
    this->pOrgsAft = new InvertedFile*[this->height];
    this->pRepsIn  = new InvertedFile*[this->height];
    this->pRepsAft = new InvertedFile*[this->height];
    
    for (auto l = 0; l < this->height; l++)
    {
        auto cnt = (int)(pow(2, this->numBits-l));
        
        this->pOrgsInTmp[l]  = new Relation[cnt];
        this->pOrgsAftTmp[l] = new Relation[cnt];
        this->pRepsInTmp[l]  = new Relation[cnt];
        this->pRepsAftTmp[l] = new Relation[cnt];
        
        this->pOrgsIn[l]  = new InvertedFile[cnt];
        this->pOrgsAft[l] = new InvertedFile[cnt];
        this->pRepsIn[l]  = new InvertedFile[cnt];
        this->pRepsAft[l] = new InvertedFile[cnt];
        
        for (auto pId = 0; pId < cnt; pId++)
        {
            this->pOrgsInTmp[l][pId].reserve(this->pOrgsIn_sizes[l][pId]);
            this->pOrgsAftTmp[l][pId].reserve(this->pOrgsAft_sizes[l][pId]);
            this->pRepsInTmp[l][pId].reserve(this->pRepsIn_sizes[l][pId]);
            this->pRepsAftTmp[l][pId].reserve(this->pRepsAft_sizes[l][pId]);
        }
    }
    for (auto l = 0; l < this->height; l++)
    {
        auto cnt = (int)(pow(2, this->numBits-l));
        
        memset(this->pOrgsIn_sizes[l], 0, cnt*sizeof(RecordId));
        memset(this->pOrgsAft_sizes[l], 0, cnt*sizeof(RecordId));
        memset(this->pRepsIn_sizes[l], 0, cnt*sizeof(RecordId));
        memset(this->pRepsAft_sizes[l], 0, cnt*sizeof(RecordId));
    }
    
    // Step 3: fill partitions.
    for (const IRecord &r : R)
        this->updatePartitions(r);
    
    // Step 4: sort partition contents.
    for (auto l = 0; l < this->height; l++)
    {
        auto cnt = (int)(pow(2, this->numBits-l));
        for (auto pId = 0; pId < cnt; pId++)
        {
            this->pOrgsInTmp[l][pId].sortByStart();
            this->pOrgsAftTmp[l][pId].sortByStart();
            this->pRepsInTmp[l][pId].sortByEnd();
        }
    }
    
    // Copy and free auxiliary memory.
    this->pOrgsInIds  = new RelationId*[this->height];
    this->pOrgsAftIds = new RelationId*[this->height];
    this->pRepsInIds  = new RelationId*[this->height];
    this->pRepsAftIds = new RelationId*[this->height];
    this->pOrgsInTimestamps  = new vector<pair<Timestamp, Timestamp> >*[this->height];
    this->pOrgsAftTimestamps = new vector<pair<Timestamp, Timestamp> >*[this->height];
    this->pRepsInTimestamps  = new vector<pair<Timestamp, Timestamp> >*[this->height];
    this->pRepsAftTimestamps = new vector<pair<Timestamp, Timestamp> >*[this->height];
    for (auto l = 0; l < this->height; l++)
    {
        auto cnt = (int)(pow(2, this->numBits-l));
        
        this->pOrgsInIds[l]  = new RelationId[cnt];
        this->pOrgsAftIds[l] = new RelationId[cnt];
        this->pRepsInIds[l]  = new RelationId[cnt];
        this->pRepsAftIds[l] = new RelationId[cnt];
        this->pOrgsInTimestamps[l]  = new vector<pair<Timestamp, Timestamp> >[cnt];
        this->pOrgsAftTimestamps[l] = new vector<pair<Timestamp, Timestamp> >[cnt];
        this->pRepsInTimestamps[l]  = new vector<pair<Timestamp, Timestamp> >[cnt];
        this->pRepsAftTimestamps[l] = new vector<pair<Timestamp, Timestamp> >[cnt];
        
        for (auto pId = 0; pId < cnt; pId++)
        {
            auto cnt = this->pOrgsInTmp[l][pId].size();
            this->pOrgsInIds[l][pId].reserve(cnt);
            this->pOrgsInTimestamps[l][pId].reserve(cnt);
            for (auto j = 0; j < cnt; j++)
            {
                this->pOrgsInIds[l][pId].push_back(this->pOrgsInTmp[l][pId][j].id);
                this->pOrgsInTimestamps[l][pId].push_back(make_pair(this->pOrgsInTmp[l][pId][j].start, this->pOrgsInTmp[l][pId][j].end));
            }
            
            cnt = this->pOrgsAftTmp[l][pId].size();
            this->pOrgsAftIds[l][pId].reserve(cnt);
            this->pOrgsAftTimestamps[l][pId].reserve(cnt);
            for (auto j = 0; j < cnt; j++)
            {
                this->pOrgsAftIds[l][pId].push_back(this->pOrgsAftTmp[l][pId][j].id);
                this->pOrgsAftTimestamps[l][pId].push_back(make_pair(this->pOrgsAftTmp[l][pId][j].start, this->pOrgsAftTmp[l][pId][j].end));
            }
            
            cnt = this->pRepsInTmp[l][pId].size();
            this->pRepsInIds[l][pId].reserve(cnt);
            this->pRepsInTimestamps[l][pId].reserve(cnt);
            for (auto j = 0; j < cnt; j++)
            {
                this->pRepsInIds[l][pId].push_back(this->pRepsInTmp[l][pId][j].id);
                this->pRepsInTimestamps[l][pId].push_back(make_pair(this->pRepsInTmp[l][pId][j].start, this->pRepsInTmp[l][pId][j].end));
            }
            
            cnt = this->pRepsAftTmp[l][pId].size();
            this->pRepsAftIds[l][pId].reserve(cnt);
            this->pRepsAftTimestamps[l][pId].reserve(cnt);
            for (auto j = 0; j < cnt; j++)
            {
                this->pRepsAftIds[l][pId].push_back(this->pRepsAftTmp[l][pId][j].id);
                this->pRepsAftTimestamps[l][pId].push_back(make_pair(this->pRepsAftTmp[l][pId][j].start, this->pRepsAftTmp[l][pId][j].end));
            }
        }
        
        delete[] this->pOrgsInTmp[l];
        delete[] this->pOrgsAftTmp[l];
        delete[] this->pRepsInTmp[l];
        delete[] this->pRepsAftTmp[l];
    }
    delete[] this->pOrgsInTmp;
    delete[] this->pOrgsAftTmp;
    delete[] this->pRepsInTmp;
    delete[] this->pRepsAftTmp;
}


irHINTb::irHINTb(const IRelation &R, const IRelation &U, const unsigned int numBits = 0) : HierarchicalIRIndex(R, U, numBits)
{
    // Step 1: one pass to count the contents inside each partition.
    this->pOrgsIn_sizes  = (RecordId **)malloc(this->height*sizeof(RecordId *));
    this->pOrgsAft_sizes = (RecordId **)malloc(this->height*sizeof(RecordId *));
    this->pRepsIn_sizes  = (RecordId **)malloc(this->height*sizeof(RecordId *));
    this->pRepsAft_sizes = (RecordId **)malloc(this->height*sizeof(RecordId *));
    
    for (auto l = 0; l < this->height; l++)
    {
        auto cnt = (int)(pow(2, this->numBits-l));
        
        //calloc allocates memory and sets each counter to 0
        this->pOrgsIn_sizes[l]  = (RecordId *)calloc(cnt, sizeof(RecordId));
        this->pOrgsAft_sizes[l] = (RecordId *)calloc(cnt, sizeof(RecordId));
        this->pRepsIn_sizes[l]  = (RecordId *)calloc(cnt, sizeof(RecordId));
        this->pRepsAft_sizes[l] = (RecordId *)calloc(cnt, sizeof(RecordId));
    }
    
    for (const IRecord &r : R)
        this->updateCounters(r);
    for (const IRecord &r : U)
        this->updateCounters(r);

    // Step 2: allocate necessary memory.
    this->pOrgsInTmp  = new Relation*[this->height];
    this->pOrgsAftTmp = new Relation*[this->height];
    this->pRepsInTmp  = new Relation*[this->height];
    this->pRepsAftTmp = new Relation*[this->height];
    
    this->pOrgsIn  = new InvertedFile*[this->height];
    this->pOrgsAft = new InvertedFile*[this->height];
    this->pRepsIn  = new InvertedFile*[this->height];
    this->pRepsAft = new InvertedFile*[this->height];
    
    for (auto l = 0; l < this->height; l++)
    {
        auto cnt = (int)(pow(2, this->numBits-l));
        
        this->pOrgsInTmp[l]  = new Relation[cnt];
        this->pOrgsAftTmp[l] = new Relation[cnt];
        this->pRepsInTmp[l]  = new Relation[cnt];
        this->pRepsAftTmp[l] = new Relation[cnt];
        
        this->pOrgsIn[l]  = new InvertedFile[cnt];
        this->pOrgsAft[l] = new InvertedFile[cnt];
        this->pRepsIn[l]  = new InvertedFile[cnt];
        this->pRepsAft[l] = new InvertedFile[cnt];
        
        for (auto pId = 0; pId < cnt; pId++)
        {
            this->pOrgsInTmp[l][pId].reserve(this->pOrgsIn_sizes[l][pId]);
            this->pOrgsAftTmp[l][pId].reserve(this->pOrgsAft_sizes[l][pId]);
            this->pRepsInTmp[l][pId].reserve(this->pRepsIn_sizes[l][pId]);
            this->pRepsAftTmp[l][pId].reserve(this->pRepsAft_sizes[l][pId]);
        }
    }
    
    // Step 3: fill partitions.
    for (const IRecord &r : R)
        this->updatePartitions(r);
    
    // Step 4: sort partition contents.
    for (auto l = 0; l < this->height; l++)
    {
        auto cnt = (int)(pow(2, this->numBits-l));
        for (auto pId = 0; pId < cnt; pId++)
        {
            this->pOrgsInTmp[l][pId].sortByStart();
            this->pOrgsAftTmp[l][pId].sortByStart();
            this->pRepsInTmp[l][pId].sortByEnd();
        }
    }
    
    // Copy and free auxiliary memory.
    this->pOrgsInIds  = new RelationId*[this->height];
    this->pOrgsAftIds = new RelationId*[this->height];
    this->pRepsInIds  = new RelationId*[this->height];
    this->pRepsAftIds = new RelationId*[this->height];
    this->pOrgsInTimestamps  = new vector<pair<Timestamp, Timestamp> >*[this->height];
    this->pOrgsAftTimestamps = new vector<pair<Timestamp, Timestamp> >*[this->height];
    this->pRepsInTimestamps  = new vector<pair<Timestamp, Timestamp> >*[this->height];
    this->pRepsAftTimestamps = new vector<pair<Timestamp, Timestamp> >*[this->height];
    for (auto l = 0; l < this->height; l++)
    {
        auto cnt = (int)(pow(2, this->numBits-l));
        
        this->pOrgsInIds[l]  = new RelationId[cnt];
        this->pOrgsAftIds[l] = new RelationId[cnt];
        this->pRepsInIds[l]  = new RelationId[cnt];
        this->pRepsAftIds[l] = new RelationId[cnt];
        this->pOrgsInTimestamps[l]  = new vector<pair<Timestamp, Timestamp> >[cnt];
        this->pOrgsAftTimestamps[l] = new vector<pair<Timestamp, Timestamp> >[cnt];
        this->pRepsInTimestamps[l]  = new vector<pair<Timestamp, Timestamp> >[cnt];
        this->pRepsAftTimestamps[l] = new vector<pair<Timestamp, Timestamp> >[cnt];
        
        for (auto pId = 0; pId < cnt; pId++)
        {
            auto cnt = this->pOrgsInTmp[l][pId].size();
            this->pOrgsInIds[l][pId].reserve(cnt);
            this->pOrgsInTimestamps[l][pId].reserve(cnt);
            for (auto j = 0; j < cnt; j++)
            {
                this->pOrgsInIds[l][pId].push_back(this->pOrgsInTmp[l][pId][j].id);
                this->pOrgsInTimestamps[l][pId].push_back(make_pair(this->pOrgsInTmp[l][pId][j].start, this->pOrgsInTmp[l][pId][j].end));
            }
            
            cnt = this->pOrgsAftTmp[l][pId].size();
            this->pOrgsAftIds[l][pId].reserve(cnt);
            this->pOrgsAftTimestamps[l][pId].reserve(cnt);
            for (auto j = 0; j < cnt; j++)
            {
                this->pOrgsAftIds[l][pId].push_back(this->pOrgsAftTmp[l][pId][j].id);
                this->pOrgsAftTimestamps[l][pId].push_back(make_pair(this->pOrgsAftTmp[l][pId][j].start, this->pOrgsAftTmp[l][pId][j].end));
            }
            
            cnt = this->pRepsInTmp[l][pId].size();
            this->pRepsInIds[l][pId].reserve(cnt);
            this->pRepsInTimestamps[l][pId].reserve(cnt);
            for (auto j = 0; j < cnt; j++)
            {
                this->pRepsInIds[l][pId].push_back(this->pRepsInTmp[l][pId][j].id);
                this->pRepsInTimestamps[l][pId].push_back(make_pair(this->pRepsInTmp[l][pId][j].start, this->pRepsInTmp[l][pId][j].end));
            }
            
            cnt = this->pRepsAftTmp[l][pId].size();
            this->pRepsAftIds[l][pId].reserve(cnt);
            this->pRepsAftTimestamps[l][pId].reserve(cnt);
            for (auto j = 0; j < cnt; j++)
            {
                this->pRepsAftIds[l][pId].push_back(this->pRepsAftTmp[l][pId][j].id);
                this->pRepsAftTimestamps[l][pId].push_back(make_pair(this->pRepsAftTmp[l][pId][j].start, this->pRepsAftTmp[l][pId][j].end));
            }
        }
        
        delete[] this->pOrgsInTmp[l];
        delete[] this->pOrgsAftTmp[l];
        delete[] this->pRepsInTmp[l];
        delete[] this->pRepsAftTmp[l];
    }
    delete[] this->pOrgsInTmp;
    delete[] this->pOrgsAftTmp;
    delete[] this->pRepsInTmp;
    delete[] this->pRepsAftTmp;
    
    for (auto l = 0; l < this->height; l++)
    {
        auto cnt = (int)(pow(2, this->numBits-l));
        
        memset(this->pOrgsIn_sizes[l], 0, cnt*sizeof(RecordId));
        memset(this->pOrgsAft_sizes[l], 0, cnt*sizeof(RecordId));
        memset(this->pRepsIn_sizes[l], 0, cnt*sizeof(RecordId));
        memset(this->pRepsAft_sizes[l], 0, cnt*sizeof(RecordId));
    }
}


void irHINTb::getStats()
{
//    size_t sum = 0;
//    for (auto l = 0; l < this->height; l++)
//    {
//        auto cnt = pow(2, this->numBits-l);
//
//        this->numPartitions += cnt;
//
//        this->numOriginalsIn  += this->pOrgsInIds[l].size();
//        this->numOriginalsAft += this->pOrgsAftIds[l].size();
//        this->numReplicasIn   += this->pRepsInIds[l].size();
//        this->numReplicasAft  += this->pRepsAftIds[l].size();
//    }
//
//    this->avgPartitionSize = (float)(this->numIndexedRecords+this->numReplicasIn+this->numReplicasAft)/(this->numPartitions-numEmptyPartitions);
}


size_t irHINTb::getSize()
{
    size_t size = 0;
    
    for (auto l = 0; l < this->height; l++)
    {
        auto cnt = pow(2, this->numBits-l);
        
        for (int pid = 0; pid < cnt; pid++)
        {
            size += this->pOrgsInIds[l][pid].size()  * (sizeof(RecordId) + 2*sizeof(Timestamp));
            size += this->pOrgsAftIds[l][pid].size() * (sizeof(RecordId) + 2*sizeof(Timestamp));
            size += this->pRepsInIds[l][pid].size()  * (sizeof(RecordId) + 2*sizeof(Timestamp));
            size += this->pRepsAftIds[l][pid].size() * (sizeof(RecordId) + 2*sizeof(Timestamp));

            size += this->pOrgsIn[l][pid].getSize();
            size += this->pOrgsAft[l][pid].getSize();
            size += this->pRepsIn[l][pid].getSize();
            size += this->pRepsAft[l][pid].getSize();
        }
    }
    
    return size;
}


irHINTb::~irHINTb()
{
    for (auto l = 0; l < this->height; l++)
    {
        delete[] this->pOrgsInIds[l];
        delete[] this->pOrgsInTimestamps[l];
        delete[] this->pOrgsAftIds[l];
        delete[] this->pOrgsAftTimestamps[l];
        delete[] this->pRepsInIds[l];
        delete[] this->pRepsInTimestamps[l];
        delete[] this->pRepsAftIds[l];
        
        delete[] this->pOrgsIn[l];
        delete[] this->pOrgsAft[l];
        delete[] this->pRepsIn[l];
        delete[] this->pRepsAft[l];
    }
    
    delete[] this->pOrgsInIds;
    delete[] this->pOrgsInTimestamps;
    delete[] this->pOrgsAftIds;
    delete[] this->pOrgsAftTimestamps;
    delete[] this->pRepsInIds;
    delete[] this->pRepsInTimestamps;
    delete[] this->pRepsAftIds;
    
    delete this->pOrgsIn;
    delete this->pOrgsAft;
    delete this->pRepsIn;
    delete this->pRepsAft;
    
    for (auto l = 0; l < this->height; l++)
    {
        if (this->pOrgsIn_sizes[l])
        {
            free(this->pOrgsIn_sizes[l]);
            free(this->pOrgsAft_sizes[l]);
            free(this->pRepsIn_sizes[l]);
            free(this->pRepsAft_sizes[l]);
        }
    }
    if (this->pOrgsIn_sizes)
    {
        free(this->pOrgsIn_sizes);
        free(this->pOrgsAft_sizes);
        free(this->pRepsIn_sizes);
        free(this->pRepsAft_sizes);
    }
}



// Updating
//#define SORTING

void irHINTb::insert(IRelation &U)
{
#ifdef SORTING
    for (auto &r : U)
    {
        int level = 0;
        Timestamp a = r.start >> (this->maxBits-this->numBits);
        Timestamp b = r.end   >> (this->maxBits-this->numBits);
        Timestamp prevb;
        int firstfound = 0, lastfound = 0;
        pair<Timestamp, Timestamp> p = make_pair(r.start, r.end);
        
        while (level < this->height && a <= b)
        {
            if (a%2)
            { //last bit of a is 1
                if (firstfound)
                {
                    if ((a == b) && (!lastfound))
                    {
                        this->pRepsInIds[level][a].push_back(r.id);
                        this->pRepsInTimestamps[level][a].push_back(p);
                        this->pRepsIn[level][a].index(r);
                        this->pRepsIn_sizes[level][a] = 1;
                        lastfound = 1;
                    }
                    else
                    {
                        this->pRepsAftIds[level][a].push_back(r.id);
                        this->pRepsAftTimestamps[level][a].push_back(p);
                        this->pRepsAft[level][a].index(r);
                    }
                }
                else
                {
                    if ((a == b) && (!lastfound))
                    {
                        this->pOrgsInIds[level][a].push_back(r.id);
                        this->pOrgsInTimestamps[level][a].push_back(p);
                        this->pOrgsIn[level][a].index(r);
                        this->pOrgsIn_sizes[level][a] = 1;
                    }
                    else
                    {
                        this->pOrgsAftIds[level][a].push_back(r.id);
                        this->pOrgsAftTimestamps[level][a].push_back(p);
                        this->pOrgsAft[level][a].index(r);
                        this->pOrgsAft_sizes[level][a] = 1;
                    }
                    firstfound = 1;
                }
                a++;
            }
            if (!(b%2))
            { //last bit of b is 0
                prevb = b;
                b--;
                if ((!firstfound) && b < a)
                {
                    if (!lastfound)
                    {
                        this->pOrgsInIds[level][prevb].push_back(r.id);
                        this->pOrgsInTimestamps[level][prevb].push_back(p);
                        this->pOrgsIn[level][prevb].index(r);
                        this->pOrgsIn_sizes[level][prevb] = 1;
                    }
                    else
                    {
                        this->pOrgsAftIds[level][prevb].push_back(r.id);
                        this->pOrgsAftTimestamps[level][prevb].push_back(p);
                        this->pOrgsAft[level][prevb].index(r);
                        this->pOrgsAft_sizes[level][prevb] = 1;
                    }
                }
                else
                {
                    if (!lastfound)
                    {
                        this->pRepsInIds[level][prevb].push_back(r.id);
                        this->pRepsInTimestamps[level][prevb].push_back(p);
                        this->pRepsIn[level][prevb].index(r);
                        this->pRepsIn_sizes[level][prevb] = 1;
                        lastfound = 1;
                    }
                    else
                    {
                        this->pRepsAftIds[level][prevb].push_back(r.id);
                        this->pRepsAftTimestamps[level][prevb].push_back(p);
                        this->pRepsAft[level][prevb].index(r);
                    }
                }
            }
            a >>= 1; // a = a div 2
            b >>= 1; // b = b div 2
            level++;
        }
    }
    
    for (auto l = 0; l < this->height; l++)
    {
        auto cnt = pow(2, this->numBits-l);
        
        for (int pid = 0; pid < cnt; pid++)
        {
            if (this->pOrgsIn_sizes[l][pid] != 0)
                sort(this->pOrgsInTimestamps[l][pid].begin(), this->pOrgsInTimestamps[l][pid].end(), compareTimestampsByStart);
            if (this->pOrgsAft_sizes[l][pid] != 0)
                sort(this->pOrgsAftTimestamps[l][pid].begin(), this->pOrgsAftTimestamps[l][pid].end(), compareTimestampsByStart);
            if (this->pRepsIn_sizes[l][pid] != 0)
                sort(this->pRepsInTimestamps[l][pid].begin(), this->pRepsInTimestamps[l][pid].end(), compareTimestampsByEnd);
        }
    }
#else
    for (auto &r : U)
    {
        int level = 0;
        Timestamp a = r.start >> (this->maxBits-this->numBits);
        Timestamp b = r.end   >> (this->maxBits-this->numBits);
        Timestamp prevb;
        int firstfound = 0, lastfound = 0;
        pair<Timestamp, Timestamp> p = make_pair(r.start, r.end);
        vector<pair<Timestamp, Timestamp>>::iterator pivot;
        
        while (level < this->height && a <= b)
        {
            if (a%2)
            { //last bit of a is 1
                if (firstfound)
                {
                    if ((a == b) && (!lastfound))
                    {
                        pivot = upper_bound(this->pRepsInTimestamps[level][a].begin(), this->pRepsInTimestamps[level][a].end(), p, compareTimestampsByEnd);
                        this->pRepsInIds[level][a].insert(this->pRepsInIds[level][a].begin()+(pivot-this->pRepsInTimestamps[level][a].begin()), r.id);
                        this->pRepsInTimestamps[level][a].insert(pivot, p);
                        this->pRepsIn[level][a].index(r);
                        lastfound = 1;
                    }
                    else
                    {
                        this->pRepsAftIds[level][a].push_back(r.id);
                        this->pRepsAftTimestamps[level][a].push_back(make_pair(r.start, r.end));
                        this->pRepsAft[level][a].index(r);
                    }
                }
                else
                {
                    if ((a == b) && (!lastfound))
                    {
                        pivot = upper_bound(this->pOrgsInTimestamps[level][a].begin(), this->pOrgsInTimestamps[level][a].end(), p, compareTimestampsByStart);
                        this->pOrgsInIds[level][a].insert(this->pOrgsInIds[level][a].begin()+(pivot-this->pOrgsInTimestamps[level][a].begin()), r.id);
                        this->pOrgsInTimestamps[level][a].insert(pivot, p);
                        this->pOrgsIn[level][a].index(r);
                    }
                    else
                    {
                        pivot = upper_bound(this->pOrgsAftTimestamps[level][a].begin(), this->pOrgsAftTimestamps[level][a].end(), p, compareTimestampsByStart);
                        this->pOrgsAftIds[level][a].insert(this->pOrgsAftIds[level][a].begin()+(pivot-this->pOrgsAftTimestamps[level][a].begin()), r.id);
                        this->pOrgsAftTimestamps[level][a].insert(pivot, p);
                        this->pOrgsAft[level][a].index(r);
                    }
                    firstfound = 1;
                }
                a++;
            }
            if (!(b%2))
            { //last bit of b is 0
                prevb = b;
                b--;
                if ((!firstfound) && b < a)
                {
                    if (!lastfound)
                    {
                        pivot = upper_bound(this->pOrgsInTimestamps[level][prevb].begin(), this->pOrgsInTimestamps[level][prevb].end(), p, compareTimestampsByStart);
                        this->pOrgsInIds[level][prevb].insert(this->pOrgsInIds[level][prevb].begin()+(pivot-this->pOrgsInTimestamps[level][prevb].begin()), r.id);
                        this->pOrgsInTimestamps[level][prevb].insert(pivot, p);
                        this->pOrgsIn[level][prevb].index(r);
                    }
                    else
                    {
                        pivot = upper_bound(this->pOrgsAftTimestamps[level][prevb].begin(), this->pOrgsAftTimestamps[level][prevb].end(), p, compareTimestampsByStart);
                        this->pOrgsAftIds[level][prevb].insert(this->pOrgsAftIds[level][prevb].begin()+(pivot-this->pOrgsAftTimestamps[level][prevb].begin()), r.id);
                        this->pOrgsAftTimestamps[level][prevb].insert(pivot, p);
                        this->pOrgsAft[level][prevb].index(r);
                    }
                }
                else
                {
                    if (!lastfound)
                    {
                        pivot = upper_bound(this->pRepsInTimestamps[level][prevb].begin(), this->pRepsInTimestamps[level][prevb].end(), p, compareTimestampsByEnd);
                        this->pRepsInIds[level][prevb].insert(this->pRepsInIds[level][prevb].begin()+(pivot-this->pRepsInTimestamps[level][prevb].begin()), r.id);
                        this->pRepsInTimestamps[level][prevb].insert(pivot, p);
                        this->pRepsIn[level][prevb].index(r);
                        lastfound = 1;
                    }
                    else
                    {
                        this->pRepsAftIds[level][prevb].push_back(r.id);
                        this->pRepsAftTimestamps[level][prevb].push_back(make_pair(r.start, r.end));
                        this->pRepsAft[level][prevb].index(r);
                    }
                }
            }
            a >>= 1; // a = a div 2
            b >>= 1; // b = b div 2
            level++;
        }
    }
#endif
}



// Querying
inline void irHINTb::scanPartition_CheckBoth_gOverlaps(const unsigned int level, const Timestamp t, RelationId **ids, vector<pair<Timestamp, Timestamp> > **timestamps, InvertedFile **ifs, bool (*compare)(const pair<Timestamp, Timestamp>&, const pair<Timestamp, Timestamp>&), const RangeIRQuery &q, RelationId &result)
{
    vector<pair<Timestamp, Timestamp> >::iterator iter, iterBegin, iterEnd;
    RelationIdIterator iterI;
    RelationId candidates;

#ifdef V1
    if (!ifs[level][t].moveOut(q, candidates))
        return;

    for (auto i = 1; i < q.terms.size(); i++)
    {
        if (!ifs[level][t].intersect(q, i, candidates))
            return;
    }

    iterBegin = timestamps[level][t].begin();
    iterEnd = timestamps[level][t].end();
    iterI = ids[level][t].begin();

    vector<pair<Timestamp, Timestamp> >::iterator pivot = lower_bound(iterBegin, iterEnd, make_pair(q.end+1, q.end+1), compare);
    for (iter = iterBegin; iter != pivot; iter++)
    {
        if ((q.start <= iter->second) && (binary_search(candidates.begin(), candidates.end(), *iterI)))
            result.push_back(*iterI);
        iterI++;
    }
#elif defined(V2)
    iterBegin = timestamps[level][t].begin();
    iterEnd = timestamps[level][t].end();
    iterI = ids[level][t].begin();

    vector<pair<Timestamp, Timestamp> >::iterator pivot = lower_bound(iterBegin, iterEnd, make_pair(q.end+1, q.end+1), compare);
    for (iter = iterBegin; iter != pivot; iter++)
    {
        if (q.start <= iter->second)
            candidates.push_back(*iterI);
        iterI++;
    }

    if (candidates.empty())
        return;
    
    sort(candidates.begin(), candidates.end());
    for (auto i = 0; i < q.terms.size()-1; i++)
    {
        if (!ifs[level][t].intersect(q, i, candidates))
            return;
    }
    
    // Update result
//    result.insert(result.end(), candidates.begin(), candidates.end());
    ifs[level][t].intersectAndOutput(q, q.terms.size()-1, candidates, result);
#elif defined(V3)
    if (!ifs[level][t].moveOut(q, candidates))
        return;

    for (auto i = 1; i < q.terms.size(); i++)
    {
        if (!ifs[level][t].intersect(q, i, candidates))
            return;
    }

    iterBegin = timestamps[level][t].begin();
    iterEnd = timestamps[level][t].end();
    iterI = ids[level][t].begin();

    RelationId tmp;
    vector<pair<Timestamp, Timestamp> >::iterator pivot = lower_bound(iterBegin, iterEnd, make_pair(q.end+1, q.end+1), compare);
    for (iter = iterBegin; iter != pivot; iter++)
    {
        if (q.start <= iter->second)
            tmp.push_back(*iterI);
        iterI++;
    }

    if (tmp.empty())
        return;
    sort(tmp.begin(), tmp.end());

    auto iterC    = candidates.begin();
    auto iterCEnd = candidates.end();
    auto iterT    = tmp.begin();
    auto iterTEnd = tmp.end();
    while ((iterC != iterCEnd) && (iterT != iterTEnd))
    {
        if (*iterT < *iterC)
            iterT++;
        else if (*iterT > *iterC)
            iterC++;
        else
        {
            result.push_back(*iterT);
            iterT++;
        }
    }
#endif
}


inline void irHINTb::scanPartition_CheckStart_gOverlaps(const unsigned int level, const Timestamp t, RelationId **ids, vector<pair<Timestamp, Timestamp> > **timestamps, InvertedFile **ifs, bool (*compare)(const pair<Timestamp, Timestamp>&, const pair<Timestamp, Timestamp>&), const RangeIRQuery &q, RelationId &result)
{
    vector<pair<Timestamp, Timestamp> >::iterator iter, iterBegin, iterEnd;
    RelationIdIterator iterI;
    RelationId candidates;

#ifdef V1
    if (!ifs[level][t].moveOut(q, candidates))
        return;

    for (auto i = 1; i < q.terms.size(); i++)
    {
        if (!ifs[level][t].intersect(q, i, candidates))
            return;
    }

    iterBegin = timestamps[level][t].begin();
    iterEnd = timestamps[level][t].end();
    iterI = ids[level][t].begin();

    vector<pair<Timestamp, Timestamp> >::iterator pivot = lower_bound(iterBegin, iterEnd, make_pair(q.end+1, q.end+1), compare);
    for (iter = iterBegin; iter != pivot; iter++)
    {
        if (binary_search(candidates.begin(), candidates.end(), *iterI))
            result.push_back(*iterI);

        iterI++;
    }
#elif defined(V2)
    iterBegin = timestamps[level][t].begin();
    iterEnd = timestamps[level][t].end();
    iterI = ids[level][t].begin();

    vector<pair<Timestamp, Timestamp> >::iterator pivot = lower_bound(iterBegin, iterEnd, make_pair(q.end+1, q.end+1), compare);
    for (iter = iterBegin; iter != pivot; iter++)
    {
        candidates.push_back(*iterI);
        iterI++;
    }

    if (candidates.empty())
        return;
    
    sort(candidates.begin(), candidates.end());
    for (auto i = 0; i < q.terms.size()-1; i++)
    {
        if (!ifs[level][t].intersect(q, i, candidates))
            return;
    }
    
    // Update result
//    result.insert(result.end(), candidates.begin(), candidates.end());
    ifs[level][t].intersectAndOutput(q, q.terms.size()-1, candidates, result);
#elif defined(V3)
    if (!ifs[level][t].moveOut(q, candidates))
        return;

    for (auto i = 1; i < q.terms.size(); i++)
    {
        if (!ifs[level][t].intersect(q, i, candidates))
            return;
    }

    iterBegin = timestamps[level][t].begin();
    iterEnd = timestamps[level][t].end();
    iterI = ids[level][t].begin();

    RelationId tmp;
    vector<pair<Timestamp, Timestamp> >::iterator pivot = lower_bound(iterBegin, iterEnd, make_pair(q.end+1, q.end+1), compare);
    for (iter = iterBegin; iter != pivot; iter++)
    {
        tmp.push_back(*iterI);
        iterI++;
    }

    if (tmp.empty())
        return;
    sort(tmp.begin(), tmp.end());

    auto iterC    = candidates.begin();
    auto iterCEnd = candidates.end();
    auto iterT    = tmp.begin();
    auto iterTEnd = tmp.end();
    while ((iterC != iterCEnd) && (iterT != iterTEnd))
    {
        if (*iterT < *iterC)
            iterT++;
        else if (*iterT > *iterC)
            iterC++;
        else
        {
            result.push_back(*iterT);
            iterT++;
        }
    }
#endif
}


inline void irHINTb::scanPartition_CheckEnd_gOverlaps(const unsigned int level, const Timestamp t, RelationId **ids, vector<pair<Timestamp, Timestamp> > **timestamps, InvertedFile **ifs, const RangeIRQuery &q, RelationId &result)
{
    vector<pair<Timestamp, Timestamp> >::iterator iter, iterBegin, iterEnd;
    RelationIdIterator iterI;
    RelationId candidates;

#ifdef V1
    if (!ifs[level][t].moveOut(q, candidates))
        return;

    for (auto i = 1; i < q.terms.size(); i++)
    {
        if (!ifs[level][t].intersect(q, i, candidates))
            return;
    }

    iterBegin = timestamps[level][t].begin();
    iterEnd = timestamps[level][t].end();
    iterI = ids[level][t].begin();
    for (iter = iterBegin; iter != iterEnd; iter++)
    {
        if ((q.start <= iter->second) && (binary_search(candidates.begin(), candidates.end(), *iterI)))
            result.push_back(*iterI);
        iterI++;
    }
#elif defined(V2)
    iterBegin = timestamps[level][t].begin();
    iterEnd = timestamps[level][t].end();
    iterI = ids[level][t].begin();
    for (iter = iterBegin; iter != iterEnd; iter++)
    {
        if (q.start <= iter->second)
            candidates.push_back(*iterI);
        iterI++;
    }
    if (candidates.empty())
        return;
    
    sort(candidates.begin(), candidates.end());
    for (auto i = 0; i < q.terms.size()-1; i++)
    {
        if (!ifs[level][t].intersect(q, i, candidates))
            return;
    }
    
    // Update result
//    result.insert(result.end(), candidates.begin(), candidates.end());
    ifs[level][t].intersectAndOutput(q, q.terms.size()-1, candidates, result);
#elif defined(V3)
    if (!ifs[level][t].moveOut(q, candidates))
        return;

    for (auto i = 1; i < q.terms.size(); i++)
    {
        if (!ifs[level][t].intersect(q, i, candidates))
            return;
    }

    iterBegin = timestamps[level][t].begin();
    iterEnd = timestamps[level][t].end();
    iterI = ids[level][t].begin();

    RelationId tmp;
    for (iter = iterBegin; iter != iterEnd; iter++)
    {
        if (q.start <= iter->second)
            tmp.push_back(*iterI);
        iterI++;
    }
    if (tmp.empty())
        return;
    sort(tmp.begin(), tmp.end());

    auto iterC    = candidates.begin();
    auto iterCEnd = candidates.end();
    auto iterT    = tmp.begin();
    auto iterTEnd = tmp.end();
    while ((iterC != iterCEnd) && (iterT != iterTEnd))
    {
        if (*iterT < *iterC)
            iterT++;
        else if (*iterT > *iterC)
            iterC++;
        else
        {
            result.push_back(*iterT);
            iterT++;
        }
    }
#endif
}


inline void irHINTb::scanPartition_CheckEnd_gOverlaps(const unsigned int level, const Timestamp t, RelationId **ids, vector<pair<Timestamp, Timestamp> > **timestamps, InvertedFile **ifs, bool (*compare)(const pair<Timestamp, Timestamp>&, const pair<Timestamp, Timestamp>&), const RangeIRQuery &q, RelationId &result)
{
    vector<pair<Timestamp, Timestamp> >::iterator iter, iterBegin, iterEnd;
    RelationIdIterator iterI;
    RelationId candidates;
    
#ifdef V1
    if (!ifs[level][t].moveOut(q, candidates))
        return;

    for (auto i = 1; i < q.terms.size(); i++)
    {
        if (!ifs[level][t].intersect(q, i, candidates))
            return;
    }

    iterBegin = timestamps[level][t].begin();
    iterEnd = timestamps[level][t].end();
    iterI = ids[level][t].begin();
    
    iter = lower_bound(iterBegin, iterEnd, make_pair(q.start, q.start), compare);
    iterI += iter-iterBegin;
    while (iter != iterEnd)
    {
        if (binary_search(candidates.begin(), candidates.end(), *iterI))
            result.push_back(*iterI);

        iter++;
        iterI++;
    }
#elif defined(V2)
    iterBegin = timestamps[level][t].begin();
    iterEnd = timestamps[level][t].end();
    iterI = ids[level][t].begin();

    iter = lower_bound(iterBegin, iterEnd, make_pair(q.start, q.start), compare);
    iterI += iter-iterBegin;
    while (iter != iterEnd)
    {
        candidates.push_back(*iterI);

        iter++;
        iterI++;
    }
    if (candidates.empty())
        return;
    
    sort(candidates.begin(), candidates.end());
    for (auto i = 0; i < q.terms.size()-1; i++)
    {
        if (!ifs[level][t].intersect(q, i, candidates))
            return;
    }
    
    // Update result
//    result.insert(result.end(), candidates.begin(), candidates.end());
    ifs[level][t].intersectAndOutput(q, q.terms.size()-1, candidates, result);
#elif defined(V3)
    if (!ifs[level][t].moveOut(q, candidates))
        return;

    for (auto i = 1; i < q.terms.size(); i++)
    {
        if (!ifs[level][t].intersect(q, i, candidates))
            return;
    }

    iterBegin = timestamps[level][t].begin();
    iterEnd = timestamps[level][t].end();
    iterI = ids[level][t].begin();

    RelationId tmp;
    iter = lower_bound(iterBegin, iterEnd, make_pair(q.start, q.start), compare);
    iterI += iter-iterBegin;
    while (iter != iterEnd)
    {
        tmp.push_back(*iterI);

        iter++;
        iterI++;
    }
    if (tmp.empty())
        return;
    sort(tmp.begin(), tmp.end());

    auto iterC    = candidates.begin();
    auto iterCEnd = candidates.end();
    auto iterT    = tmp.begin();
    auto iterTEnd = tmp.end();
    while ((iterC != iterCEnd) && (iterT != iterTEnd))
    {
        if (*iterT < *iterC)
            iterT++;
        else if (*iterT > *iterC)
            iterC++;
        else
        {
            result.push_back(*iterT);
            iterT++;
        }
    }
#endif
}


inline void irHINTb::scanPartition_NoChecks_gOverlaps(const unsigned int level, const Timestamp t, RelationId **ids, InvertedFile **ifs, const RangeIRQuery &q, RelationId &result)
{
    RelationId candidates;

    if (!ifs[level][t].moveOut(q, candidates))
        return;

    for (auto i = 1; i < q.terms.size()-1; i++)
    {
        if (!ifs[level][t].intersect(q, i, candidates))
            return;
    }
    
    ifs[level][t].intersectAndOutput(q, q.terms.size()-1, candidates, result);
}


void irHINTb::executeContainment(const RangeIRQuery &q, RelationId &result)
{
    vector<pair<Timestamp, Timestamp> >::iterator iter, iterBegin, iterEnd;
    RelationIdIterator iterI, iterIBegin, iterIEnd;
    Timestamp a = q.start >> (this->maxBits-this->numBits); // prefix
    Timestamp b = q.end   >> (this->maxBits-this->numBits); // prefix
    bool foundzero = false;
    bool foundone = false;


    for (auto l = 0; l < this->numBits; l++)
    {
//        cout << "l = " << l << "\ta = " << a << endl;
        if (foundone && foundzero)
        {
            // Partition totally covers lowest-level partition range that includes query range
            // all contents are guaranteed to be results

            // Handle the partition that contains a: consider both originals and replicas
            this->scanPartition_NoChecks_gOverlaps(l, a, this->pRepsInIds, this->pRepsIn, q, result);
            this->scanPartition_NoChecks_gOverlaps(l, a, this->pRepsAftIds, this->pRepsAft, q, result);

            for (auto i = a; i <= b; i++)
            {
                this->scanPartition_NoChecks_gOverlaps(l, i, this->pOrgsInIds, this->pOrgsIn, q, result);
                this->scanPartition_NoChecks_gOverlaps(l, i, this->pOrgsAftIds, this->pOrgsAft, q, result);
            }
        }
        else
        {
            // Comparisons needed

            // Handle the partition that contains a: consider both originals and replicas, comparisons needed
            if (a == b)
            {
                // Special case when query overlaps only one partition, Lemma 3
                this->scanPartition_CheckBoth_gOverlaps(l, a, this->pOrgsInIds, this->pOrgsInTimestamps, this->pOrgsIn, CompareTimestampPairsByStart, q, result);
                this->scanPartition_CheckStart_gOverlaps(l, a, this->pOrgsAftIds, this->pOrgsAftTimestamps, this->pOrgsAft, CompareTimestampPairsByStart, q, result);
            }
            else
            {
                // Lemma 1
                this->scanPartition_CheckEnd_gOverlaps(l, a, this->pOrgsInIds, this->pOrgsInTimestamps, this->pOrgsIn, q, result);
                this->scanPartition_NoChecks_gOverlaps(l, a, this->pOrgsAftIds, this->pOrgsAft, q, result);
            }

            // Lemma 1, 3
            this->scanPartition_CheckEnd_gOverlaps(l, a, this->pRepsInIds, this->pRepsInTimestamps, this->pRepsIn, CompareTimestampPairsByEnd, q, result);
            this->scanPartition_NoChecks_gOverlaps(l, a, this->pRepsAftIds, this->pRepsAft, q, result);

            if (a < b)
            {
                // Handle the rest before the partition that contains b: consider only originals, no comparisons needed
                for (auto i = a+1; i < b; i++)
                {
                    this->scanPartition_NoChecks_gOverlaps(l, i, this->pOrgsInIds, this->pOrgsIn, q, result);
                    this->scanPartition_NoChecks_gOverlaps(l, i, this->pOrgsAftIds, this->pOrgsAft, q, result);
                }

                // Handle the partition that contains b: consider only originals, comparisons needed
                this->scanPartition_CheckStart_gOverlaps(l, b, this->pOrgsInIds, this->pOrgsInTimestamps, this->pOrgsIn, CompareTimestampPairsByStart, q, result);
                this->scanPartition_CheckStart_gOverlaps(l, b, this->pOrgsAftIds, this->pOrgsAftTimestamps, this->pOrgsAft, CompareTimestampPairsByStart, q, result);
            }

            if (b%2) //last bit of b is 1
                foundone = 1;
            if (!(a%2)) //last bit of a is 0
                foundzero = 1;
        }
        a >>= 1; // a = a div 2
        b >>= 1; // b = b div 2
    }

    // Handle root.
    if (foundone && foundzero)
    {
        // All contents are guaranteed to be results
        this->scanPartition_NoChecks_gOverlaps(this->numBits, 0, this->pOrgsInIds, this->pOrgsIn, q, result);
    }
    else
    {
        // Comparisons needed
        this->scanPartition_CheckBoth_gOverlaps(this->numBits, 0, this->pOrgsInIds, this->pOrgsInTimestamps, this->pOrgsIn, CompareTimestampPairsByStart, q, result);
    }
}
#endif

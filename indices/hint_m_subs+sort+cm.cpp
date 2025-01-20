#include "hint_m.h"



inline void mergeSort(RelationIdIterator iterPBegin, RelationIdIterator iterPEnd, RelationId &candidates, RelationId &result)
{
    auto iterP = iterPBegin;
    auto iterC    = candidates.begin();
    auto iterCEnd = candidates.end();

    while ((iterP != iterPEnd) && (iterC != iterCEnd))
    {
        if (*iterP < *iterC)
            iterP++;
        else if (*iterP > *iterC)
            iterC++;
        else
        {
//            cout << "\t\t" << *iterP << endl;
            result.push_back(*iterP);
            iterP++;
            
#ifdef INTERSECT_WITH_SHRINKING
            iterC = candidates.erase(iterC);
#else
            iterC++;
#endif
        }
    }
}


inline void mergeSort(RelationIdIterator iterPBegin, RelationIdIterator iterPEnd, RelationId &candidates, vector<RelationId> &vec_result)
{
    auto iterP = iterPBegin;
    auto iterC    = candidates.begin();
    auto iterCEnd = candidates.end();
    RelationId tmp;

    while ((iterP != iterPEnd) && (iterC != iterCEnd))
    {
        if (*iterP < *iterC)
            iterP++;
        else if (*iterP > *iterC)
            iterC++;
        else
        {
//            cout << "\t\t" << *iterP << endl;
            tmp.push_back(*iterP);
            iterP++;
            
#ifdef INTERSECT_WITH_SHRINKING
            iterC = candidates.erase(iterC);
#else
            iterC++;
#endif
        }
    }
    
//    cout << "|tmp| = " << tmp.size() << endl;
    if (!tmp.empty())
        vec_result.push_back(tmp);
}


inline void HINT_M_SubsSort_CM::updateCounters(const Record &r)
{
    int level = 0;
    Timestamp a = (max(r.start,this->gstart)-this->gstart) >> (this->maxBits-this->numBits);
    Timestamp b = (min(r.end,this->gend)-this->gstart)     >> (this->maxBits-this->numBits);
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


inline void HINT_M_SubsSort_CM::updatePartitions(const Record &r)
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
//                    this->pRepsInTmp[level][a][this->pRepsIn_sizes[level][a]] = Record(r.id, r.start, r.end);
//                    this->pRepsIn_sizes[level][a]++;
//                    lastfound = 1;
//                }
//                else
//                {
//                    this->pRepsAftTmp[level][a][this->pRepsAft_sizes[level][a]] = Record(r.id, r.start, r.end);
//                    this->pRepsAft_sizes[level][a]++;
//                }
//            }
//            else
//            {
//                if ((a == b) && (!lastfound))
//                {
//                    this->pOrgsInTmp[level][a][this->pOrgsIn_sizes[level][a]] = Record(r.id, r.start, r.end);
//                    this->pOrgsIn_sizes[level][a]++;
//                }
//                else
//                {
//                    this->pOrgsAftTmp[level][a][this->pOrgsAft_sizes[level][a]] = Record(r.id, r.start, r.end);
//                    this->pOrgsAft_sizes[level][a]++;
//                }
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
//                {
//                    this->pOrgsInTmp[level][prevb][this->pOrgsIn_sizes[level][prevb]] = Record(r.id, r.start, r.end);
//                    this->pOrgsIn_sizes[level][prevb]++;
//                }
//                else
//                {
//                    this->pOrgsAftTmp[level][prevb][this->pOrgsAft_sizes[level][prevb]] = Record(r.id, r.start, r.end);
//                    this->pOrgsAft_sizes[level][prevb]++;
//                }
//            }
//            else
//            {
//                if (!lastfound)
//                {
//                    this->pRepsInTmp[level][prevb][this->pRepsIn_sizes[level][prevb]] = Record(r.id, r.start, r.end);
//                    this->pRepsIn_sizes[level][prevb]++;
//                    lastfound = 1;
//                }
//                else
//                {
//                    this->pRepsAftTmp[level][prevb][this->pRepsAft_sizes[level][prevb]] = Record(r.id, r.start, r.end);
//                    this->pRepsAft_sizes[level][prevb]++;
//                }
//            }
//        }
//        a >>= 1; // a = a div 2
//        b >>= 1; // b = b div 2
//        level++;
//    }
//}
{
    int level = 0;
    Timestamp a = (max(r.start,this->gstart)-this->gstart) >> (this->maxBits-this->numBits);
    Timestamp b = (min(r.end,this->gend)-this->gstart)     >> (this->maxBits-this->numBits);
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
                    this->pRepsInTmp[level][a].push_back(r);
//                    this->pRepsInTmp[level][a][this->pRepsIn_sizes[level][a]] = Record(r.id, r.start, r.end);
//                    this->pRepsIn_sizes[level][a]++;
                    lastfound = 1;
                }
                else
                {
                    this->pRepsAftTmp[level][a].push_back(r);
//                    this->pRepsAftTmp[level][a][this->pRepsAft_sizes[level][a]] = Record(r.id, r.start, r.end);
//                    this->pRepsAft_sizes[level][a]++;
                }
            }
            else
            {
                if ((a == b) && (!lastfound))
                {
                    this->pOrgsInTmp[level][a].push_back(r);
//                    this->pOrgsInTmp[level][a][this->pOrgsIn_sizes[level][a]] = Record(r.id, r.start, r.end);
//                    this->pOrgsIn_sizes[level][a]++;
                }
                else
                {
                    this->pOrgsAftTmp[level][a].push_back(r);
//                    this->pOrgsAftTmp[level][a][this->pOrgsAft_sizes[level][a]] = Record(r.id, r.start, r.end);
//                    this->pOrgsAft_sizes[level][a]++;
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
                    this->pOrgsInTmp[level][prevb].push_back(r);
//                    this->pOrgsInTmp[level][prevb][this->pOrgsIn_sizes[level][prevb]] = Record(r.id, r.start, r.end);
//                    this->pOrgsIn_sizes[level][prevb]++;
                }
                else
                {
                    this->pOrgsAftTmp[level][prevb].push_back(r);
//                    this->pOrgsAftTmp[level][prevb][this->pOrgsAft_sizes[level][prevb]] = Record(r.id, r.start, r.end);
//                    this->pOrgsAft_sizes[level][prevb]++;
                }
            }
            else
            {
                if (!lastfound)
                {
                    this->pRepsInTmp[level][prevb].push_back(r);
//                    this->pRepsInTmp[level][prevb][this->pRepsIn_sizes[level][prevb]] = Record(r.id, r.start, r.end);
//                    this->pRepsIn_sizes[level][prevb]++;
                    lastfound = 1;
                }
                else
                {
                    this->pRepsAftTmp[level][prevb].push_back(r);
//                    this->pRepsAftTmp[level][prevb][this->pRepsAft_sizes[level][prevb]] = Record(r.id, r.start, r.end);
//                    this->pRepsAft_sizes[level][prevb]++;
                }
            }
        }
        a >>= 1; // a = a div 2
        b >>= 1; // b = b div 2
        level++;
    }
}


HINT_M_SubsSort_CM::HINT_M_SubsSort_CM(const Relation &R, const unsigned int numBits = 0) : HierarchicalIndex(R, numBits)
{
    // Step 1: one pass to count the contents inside each partition.
    this->pOrgsIn_sizes  = (RecordId **)malloc(this->height*sizeof(RecordId *));
    this->pOrgsAft_sizes = (RecordId **)malloc(this->height*sizeof(RecordId *));
    this->pRepsIn_sizes  = (size_t **)malloc(this->height*sizeof(size_t *));
    this->pRepsAft_sizes = (size_t **)malloc(this->height*sizeof(size_t *));
    
    for (auto l = 0; l < this->height; l++)
    {
        auto cnt = (int)(pow(2, this->numBits-l));
        
        //calloc allocates memory and sets each counter to 0
        this->pOrgsIn_sizes[l]  = (RecordId *)calloc(cnt, sizeof(RecordId));
        this->pOrgsAft_sizes[l] = (RecordId *)calloc(cnt, sizeof(RecordId));
        this->pRepsIn_sizes[l]  = (size_t *)calloc(cnt, sizeof(size_t));
        this->pRepsAft_sizes[l] = (size_t *)calloc(cnt, sizeof(size_t));
    }
    
    for (const Record &r : R)
        this->updateCounters(r);

    // Step 2: allocate necessary memory.
    this->pOrgsInTmp  = new Relation*[this->height];
    this->pOrgsAftTmp = new Relation*[this->height];
    this->pRepsInTmp  = new Relation*[this->height];
    this->pRepsAftTmp = new Relation*[this->height];
    
    for (auto l = 0; l < this->height; l++)
    {
        auto cnt = (int)(pow(2, this->numBits-l));
        
        this->pOrgsInTmp[l]  = new Relation[cnt];
        this->pOrgsAftTmp[l] = new Relation[cnt];
        this->pRepsInTmp[l]  = new Relation[cnt];
        this->pRepsAftTmp[l] = new Relation[cnt];
        
        for (auto pId = 0; pId < cnt; pId++)
        {
            this->pOrgsInTmp[l][pId].reserve(this->pOrgsIn_sizes[l][pId]);
            this->pOrgsAftTmp[l][pId].reserve(this->pOrgsAft_sizes[l][pId]);
            this->pRepsInTmp[l][pId].reserve(this->pRepsIn_sizes[l][pId]);
            this->pRepsAftTmp[l][pId].reserve(this->pRepsAft_sizes[l][pId]);
        }
    }
//    for (auto l = 0; l < this->height; l++)
//    {
//        auto cnt = (int)(pow(2, this->numBits-l));
//        
//        memset(this->pOrgsIn_sizes[l], 0, cnt*sizeof(RecordId));
//        memset(this->pOrgsAft_sizes[l], 0, cnt*sizeof(RecordId));
//        memset(this->pRepsIn_sizes[l], 0, cnt*sizeof(size_t));
//        memset(this->pRepsAft_sizes[l], 0, cnt*sizeof(size_t));
//    }

    // Step 3: fill partitions.
    for (const Record &r : R)
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
            this->pOrgsInIds[l][pId].resize(cnt);
            this->pOrgsInTimestamps[l][pId].resize(cnt);
            for (auto j = 0; j < cnt; j++)
            {
                this->pOrgsInIds[l][pId][j] = this->pOrgsInTmp[l][pId][j].id;
                this->pOrgsInTimestamps[l][pId][j].first = this->pOrgsInTmp[l][pId][j].start;
                this->pOrgsInTimestamps[l][pId][j].second = this->pOrgsInTmp[l][pId][j].end;
            }
            
            cnt = this->pOrgsAftTmp[l][pId].size();
            this->pOrgsAftIds[l][pId].resize(cnt);
            this->pOrgsAftTimestamps[l][pId].resize(cnt);
            for (auto j = 0; j < cnt; j++)
            {
                this->pOrgsAftIds[l][pId][j] = this->pOrgsAftTmp[l][pId][j].id;
                this->pOrgsAftTimestamps[l][pId][j].first = this->pOrgsAftTmp[l][pId][j].start;
                this->pOrgsAftTimestamps[l][pId][j].second = this->pOrgsAftTmp[l][pId][j].end;
            }
            
            cnt = this->pRepsInTmp[l][pId].size();
            this->pRepsInIds[l][pId].resize(cnt);
            this->pRepsInTimestamps[l][pId].resize(cnt);
            for (auto j = 0; j < cnt; j++)
            {
                this->pRepsInIds[l][pId][j] = this->pRepsInTmp[l][pId][j].id;
                this->pRepsInTimestamps[l][pId][j].first = this->pRepsInTmp[l][pId][j].start;
                this->pRepsInTimestamps[l][pId][j].second = this->pRepsInTmp[l][pId][j].end;
            }

            cnt = this->pRepsAftTmp[l][pId].size();
            this->pRepsAftIds[l][pId].resize(cnt);
            this->pRepsAftTimestamps[l][pId].resize(cnt);
            for (auto j = 0; j < cnt; j++)
            {
                this->pRepsAftIds[l][pId][j] = this->pRepsAftTmp[l][pId][j].id;
                this->pRepsAftTimestamps[l][pId][j].first = this->pRepsAftTmp[l][pId][j].start;
                this->pRepsAftTimestamps[l][pId][j].second = this->pRepsAftTmp[l][pId][j].end;
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


HINT_M_SubsSort_CM::HINT_M_SubsSort_CM(const Relation &R, const Relation &U, const unsigned int numBits = 0) : HierarchicalIndex(R, U, numBits)
{
    // Step 1: one pass to count the contents inside each partition.
    this->pOrgsIn_sizes  = (RecordId **)malloc(this->height*sizeof(RecordId *));
    this->pOrgsAft_sizes = (RecordId **)malloc(this->height*sizeof(RecordId *));
    this->pRepsIn_sizes  = (size_t **)malloc(this->height*sizeof(size_t *));
    this->pRepsAft_sizes = (size_t **)malloc(this->height*sizeof(size_t *));
    
    for (auto l = 0; l < this->height; l++)
    {
        auto cnt = (int)(pow(2, this->numBits-l));
        
        //calloc allocates memory and sets each counter to 0
        this->pOrgsIn_sizes[l]  = (RecordId *)calloc(cnt, sizeof(RecordId));
        this->pOrgsAft_sizes[l] = (RecordId *)calloc(cnt, sizeof(RecordId));
        this->pRepsIn_sizes[l]  = (size_t *)calloc(cnt, sizeof(size_t));
        this->pRepsAft_sizes[l] = (size_t *)calloc(cnt, sizeof(size_t));
    }
    
    for (const Record &r : R)
        this->updateCounters(r);
    for (const Record &r : U)
        this->updateCounters(r);

    // Step 2: allocate necessary memory.
    this->pOrgsInTmp  = new Relation*[this->height];
    this->pOrgsAftTmp = new Relation*[this->height];
    this->pRepsInTmp  = new Relation*[this->height];
    this->pRepsAftTmp = new Relation*[this->height];
    
    for (auto l = 0; l < this->height; l++)
    {
        auto cnt = (int)(pow(2, this->numBits-l));
        
        this->pOrgsInTmp[l]  = new Relation[cnt];
        this->pOrgsAftTmp[l] = new Relation[cnt];
        this->pRepsInTmp[l]  = new Relation[cnt];
        this->pRepsAftTmp[l] = new Relation[cnt];
        
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
        memset(this->pRepsIn_sizes[l], 0, cnt*sizeof(size_t));
        memset(this->pRepsAft_sizes[l], 0, cnt*sizeof(size_t));
    }

    // Step 3: fill partitions.
    for (const Record &r : R)
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
            auto cnt = this->pOrgsIn_sizes[l][pId];
            this->pOrgsInIds[l][pId].reserve(cnt);
            this->pOrgsInTimestamps[l][pId].reserve(cnt);
            for (const Record &r : this->pOrgsInTmp[l][pId])
            {
                this->pOrgsInIds[l][pId].push_back(r.id);
                this->pOrgsInTimestamps[l][pId].push_back(make_pair(r.start, r.end));
            }
            
            cnt = this->pOrgsAft_sizes[l][pId];
            this->pOrgsAftIds[l][pId].reserve(cnt);
            this->pOrgsAftTimestamps[l][pId].reserve(cnt);
            for (const Record &r : this->pOrgsAftTmp[l][pId])
            {
                this->pOrgsAftIds[l][pId].push_back(r.id);
                this->pOrgsAftTimestamps[l][pId].push_back(make_pair(r.start, r.end));
            }
            
            cnt = this->pRepsIn_sizes[l][pId];
            this->pRepsInIds[l][pId].reserve(cnt);
            this->pRepsInTimestamps[l][pId].reserve(cnt);
            for (const Record &r : this->pRepsInTmp[l][pId])
            {
                this->pRepsInIds[l][pId].push_back(r.id);
                this->pRepsInTimestamps[l][pId].push_back(make_pair(r.start, r.end));
            }

            cnt = this->pRepsAft_sizes[l][pId];
            this->pRepsAftIds[l][pId].reserve(cnt);
            this->pRepsAftTimestamps[l][pId].reserve(cnt);
            for (const Record &r : this->pRepsAftTmp[l][pId])
            {
                this->pRepsAftIds[l][pId].push_back(r.id);
                this->pRepsAftTimestamps[l][pId].push_back(make_pair(r.start, r.end));
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


void HINT_M_SubsSort_CM::getStats()
{
//    size_t sum = 0;
//    for (auto l = 0; l < this->height; l++)
//    {
//        auto cnt = pow(2, this->numBits-l);
//        
//        this->numPartitions += cnt;
//        for (int pid = 0; pid < cnt; pid++)
//        {
//            this->numOriginalsIn  += this->pOrgsInIds[l][pid].size();
//            this->numOriginalsAft += this->pOrgsAftIds[l][pid].size();
//            this->numReplicasIn   += this->pRepsInIds[l][pid].size();
//            this->numReplicasAft  += this->pRepsAftIds[l][pid].size();
//            if ((this->pOrgsInIds[l][pid].empty()) && (this->pOrgsAftIds[l][pid].empty()) && (this->pRepsInIds[l][pid].empty()) && (this->pRepsAftIds[l][pid].empty()))
//                this->numEmptyPartitions++;
//        }
//    }
//    
//    this->avgPartitionSize = (float)(this->numIndexedRecords+this->numReplicasIn+this->numReplicasAft)/(this->numPartitions-numEmptyPartitions);
}


size_t HINT_M_SubsSort_CM::getSize()
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
        }
    }
    
    return size;
}


HINT_M_SubsSort_CM::~HINT_M_SubsSort_CM()
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
    }
    
    delete[] this->pOrgsInIds;
    delete[] this->pOrgsInTimestamps;
    delete[] this->pOrgsAftIds;
    delete[] this->pOrgsAftTimestamps;
    delete[] this->pRepsInIds;
    delete[] this->pRepsInTimestamps;
    delete[] this->pRepsAftIds;
    
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


//#define SORTING

// Updating
void HINT_M_SubsSort_CM::insert(const Record &r)
{
    int level = 0;
    Timestamp a = (max(r.start,this->gstart)-this->gstart) >> (this->maxBits-this->numBits);
    Timestamp b = (min(r.end,this->gend)-this->gstart)     >> (this->maxBits-this->numBits);
    Timestamp prevb;
    int firstfound = 0, lastfound = 0;
    pair<Timestamp, Timestamp> p = make_pair(r.start, r.end);

#ifdef SORTING
//    while (level < this->height && a <= b)
//    {
//        if (a%2)
//        { //last bit of a is 1
//            if (firstfound)
//            {
//                if ((a == b) && (!lastfound))
//                {
//                    this->pRepsInIds[level][a].push_back(r.id);
//                    this->pRepsInTimestamps[level][a].push_back(p);
//                    lastfound = 1;
//                }
//                else
//                {
//                    this->pRepsAftIds[level][a].push_back(r.id);
//                    this->pRepsAftTimestamps[level][a].push_back(p);
//                }
//            }
//            else
//            {
//                if ((a == b) && (!lastfound))
//                {
//                    this->pOrgsInIds[level][a].push_back(r.id);
//                    this->pOrgsInTimestamps[level][a].push_back(p);
//                }
//                else
//                {
//                    this->pOrgsAftIds[level][a].push_back(r.id);
//                    this->pOrgsAftTimestamps[level][a].push_back(p);
//                }
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
//                {
//                    this->pOrgsInIds[level][prevb].push_back(r.id);
//                    this->pOrgsInTimestamps[level][prevb].push_back(p);
//                }
//                else
//                {
//                    this->pOrgsAftIds[level][prevb].push_back(r.id);
//                    this->pOrgsAftTimestamps[level][prevb].push_back(p);
//                }
//            }
//            else
//            {
//                if (!lastfound)
//                {
//                    this->pRepsInIds[level][prevb].push_back(r.id);
//                    this->pRepsInTimestamps[level][prevb].push_back(p);
//                    lastfound = 1;
//                }
//                else
//                {
//                    this->pRepsAftIds[level][prevb].push_back(r.id);
//                    this->pRepsAftTimestamps[level][prevb].push_back(p);
//                }
//            }
//        }
//        a >>= 1; // a = a div 2
//        b >>= 1; // b = b div 2
//        level++;
//    }
#else
    // Need to maintain sorting, but due to using CM, it's complicated
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
                    lastfound = 1;
                }
                else
                {
                    this->pRepsAftIds[level][a].push_back(r.id);
                    this->pRepsAftTimestamps[level][a].push_back(p);
                }
            }
            else
            {
                if ((a == b) && (!lastfound))
                {
                    pivot = upper_bound(this->pOrgsInTimestamps[level][a].begin(), this->pOrgsInTimestamps[level][a].end(), p, compareTimestampsByStart);
                    this->pOrgsInIds[level][a].insert(this->pOrgsInIds[level][a].begin()+(pivot-this->pOrgsInTimestamps[level][a].begin()), r.id);
                    this->pOrgsInTimestamps[level][a].insert(pivot, p);
                }
                else
                {
                    pivot = upper_bound(this->pOrgsAftTimestamps[level][a].begin(), this->pOrgsAftTimestamps[level][a].end(), p, compareTimestampsByStart);
                    this->pOrgsAftIds[level][a].insert(this->pOrgsAftIds[level][a].begin()+(pivot-this->pOrgsAftTimestamps[level][a].begin()), r.id);
                    this->pOrgsAftTimestamps[level][a].insert(pivot, p);
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
                }
                else
                {
                    pivot = upper_bound(this->pOrgsAftTimestamps[level][prevb].begin(), this->pOrgsAftTimestamps[level][prevb].end(), p, compareTimestampsByStart);
                    this->pOrgsAftIds[level][prevb].insert(this->pOrgsAftIds[level][prevb].begin()+(pivot-this->pOrgsAftTimestamps[level][prevb].begin()), r.id);
                    this->pOrgsAftTimestamps[level][prevb].insert(pivot, p);
                }
            }
            else
            {
                if (!lastfound)
                {
                    pivot = upper_bound(this->pRepsInTimestamps[level][prevb].begin(), this->pRepsInTimestamps[level][prevb].end(), p, compareTimestampsByEnd);
                    this->pRepsInIds[level][prevb].insert(this->pRepsInIds[level][prevb].begin()+(pivot-this->pRepsInTimestamps[level][prevb].begin()), r.id);
                    this->pRepsInTimestamps[level][prevb].insert(pivot, p);
                    lastfound = 1;
                }
                else
                {
                    this->pRepsAftIds[level][prevb].push_back(r.id);
                    this->pRepsAftTimestamps[level][prevb].push_back(p);
                }
            }
        }
        a >>= 1; // a = a div 2
        b >>= 1; // b = b div 2
        level++;
    }
#endif
}



// Querying
inline void HINT_M_SubsSort_CM::scanPartition_CheckBoth_moveOut_gOverlaps(unsigned int level, Timestamp t, RelationId **ids, vector<pair<Timestamp, Timestamp>> **timestamps, bool (*compare)(const pair<Timestamp, Timestamp>&, const pair<Timestamp, Timestamp>&), const RangeQuery &q, RelationId &result)
{
    auto iterI = ids[level][t].begin();
    auto iterBegin = timestamps[level][t].begin();
    auto iterEnd = lower_bound(iterBegin, timestamps[level][t].end(), pair<Timestamp, Timestamp>(q.end+1, q.end+1));
    
    for (auto iter = iterBegin; iter != iterEnd; iter++)
    {
        if (q.start <= iter->second)
            result.push_back(*iterI);
        iterI++;
    }
}


inline void HINT_M_SubsSort_CM::scanPartition_CheckEnd_moveOut_gOverlaps(unsigned int level, Timestamp t, RelationId **ids, vector<pair<Timestamp, Timestamp>> **timestamps, const RangeQuery &q, RelationId &result)
{
    auto iterI = ids[level][t].begin();
    auto iterBegin = timestamps[level][t].begin();
    auto iterEnd = timestamps[level][t].end();

    for (auto iter = iterBegin; iter != iterEnd; iter++)
    {
        if (q.start <= iter->second)
            result.push_back(*iterI);
        iterI++;
    }
}


inline void HINT_M_SubsSort_CM::scanPartition_CheckEnd_moveOut_gOverlaps(unsigned int level, Timestamp t, RelationId **ids, vector<pair<Timestamp, Timestamp>> **timestamps, bool (*compare)(const pair<Timestamp, Timestamp>&, const pair<Timestamp, Timestamp>&), const RangeQuery &q, RelationId &result)
{
    auto iterI = ids[level][t].begin();
    auto iterBegin = timestamps[level][t].begin();
    auto iterEnd = timestamps[level][t].end();
    auto iter = lower_bound(iterBegin, iterEnd, make_pair(q.start, q.start), compare);
    
    iterI += iter-iterBegin;
    while (iter != iterEnd)
    {
        result.push_back(*iterI);

        iter++;
        iterI++;
    }
}


inline void HINT_M_SubsSort_CM::scanPartition_CheckStart_moveOut_gOverlaps(unsigned int level, Timestamp t, RelationId **ids, vector<pair<Timestamp, Timestamp>> **timestamps, bool (*compare)(const pair<Timestamp, Timestamp>&, const pair<Timestamp, Timestamp>&), const RangeQuery &q, RelationId &result)
{
    auto iterI = ids[level][t].begin();
    auto iterBegin = timestamps[level][t].begin();
    auto iterEnd = lower_bound(iterBegin, timestamps[level][t].end(), pair<Timestamp, Timestamp>(q.end+1, q.end+1));

    for (auto iter = iterBegin; iter != iterEnd; iter++)
    {
        result.push_back(*iterI);
        iterI++;
    }
}


inline void HINT_M_SubsSort_CM::scanPartition_NoChecks_moveOut_gOverlaps(unsigned int level, Timestamp t, RelationId **ids, RelationId &result)
{
    auto iterIBegin = ids[level][t].begin();
    auto iterIEnd = ids[level][t].end();

//    for (auto iterI = iterIBegin; iterI != iterIEnd; iterI++)
//        result.push_back(*iterI);
    result.insert(result.end(), iterIBegin, iterIEnd);
}


inline void HINT_M_SubsSort_CM::scanPartitions_NoChecks_moveOut_gOverlaps(unsigned int level, Timestamp ts, Timestamp te, RelationId **ids, RelationId &result)
{
    for (auto j = ts; j <= te; j++)
    {
        auto iterIBegin = ids[level][j].begin();
        auto iterIEnd = ids[level][j].end();
        
//        for (auto iterI = iterIBegin; iterI != iterIEnd; iterI++)
//            result.push_back(*iterI);
        result.insert(result.end(), iterIBegin, iterIEnd);
    }
}


void HINT_M_SubsSort_CM::moveOut_gOverlaps(const RangeQuery &q, RelationId &candidates)
{
    if (q.start > this->gend || q.end < this->gstart)
        return;

    Timestamp a = (max(q.start,this->gstart)-this->gstart) >> (this->maxBits-this->numBits);
    Timestamp b = (min(q.end,this->gend)-this->gstart)     >> (this->maxBits-this->numBits);
    bool foundzero = false;
    bool foundone = false;
    
    
    for (auto l = 0; l < this->numBits; l++)
    {
        if (foundone && foundzero)
        {
            // Partition totally covers lowest-level partition range that includes query range
            // all contents are guaranteed to be results
            
            // Handle the partition that contains a: consider both originals and replicas
            this->scanPartition_NoChecks_moveOut_gOverlaps(l, a, this->pRepsInIds, candidates);
            this->scanPartition_NoChecks_moveOut_gOverlaps(l, a, this->pRepsAftIds, candidates);
            
            // Handle rest: consider only originals
            this->scanPartitions_NoChecks_moveOut_gOverlaps(l, a, b, this->pOrgsInIds, candidates);
            this->scanPartitions_NoChecks_moveOut_gOverlaps(l, a, b, this->pOrgsAftIds, candidates);
        }
        else
        {
            // Comparisons needed
            
            // Handle the partition that contains a: consider both originals and replicas, comparisons needed
            if (a == b)
            {
                this->scanPartition_CheckBoth_moveOut_gOverlaps(l, a, this->pOrgsInIds, this->pOrgsInTimestamps, CompareTimestampPairsByStart, q, candidates);
                this->scanPartition_CheckStart_moveOut_gOverlaps(l, a, this->pOrgsAftIds, this->pOrgsAftTimestamps, CompareTimestampPairsByStart, q, candidates);
            }

            else
            {
                // Lemma 1
                this->scanPartition_CheckEnd_moveOut_gOverlaps(l, a, this->pOrgsInIds, this->pOrgsInTimestamps, q, candidates);
                this->scanPartition_NoChecks_moveOut_gOverlaps(l, a, this->pOrgsAftIds, candidates);
            }

            // Lemma 1, 3
            this->scanPartition_CheckEnd_moveOut_gOverlaps(l, a, this->pRepsInIds, this->pRepsInTimestamps, CompareTimestampPairsByEnd, q, candidates);
            this->scanPartition_NoChecks_moveOut_gOverlaps(l, a, this->pRepsAftIds, candidates);

            if (a < b)
            {
                // Handle the rest before the partition that contains b: consider only originals, no comparisons needed
                this->scanPartitions_NoChecks_moveOut_gOverlaps(l, a+1, b-1, this->pOrgsInIds, candidates);
                this->scanPartitions_NoChecks_moveOut_gOverlaps(l, a+1, b-1, this->pOrgsAftIds, candidates);

                // Handle the partition that contains b: consider only originals, comparisons needed
                this->scanPartition_CheckStart_moveOut_gOverlaps(l, b, this->pOrgsInIds, this->pOrgsInTimestamps, CompareTimestampPairsByStart, q, candidates);
                this->scanPartition_CheckStart_moveOut_gOverlaps(l, b, this->pOrgsAftIds, this->pOrgsAftTimestamps, CompareTimestampPairsByStart, q, candidates);
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
        auto iterIBegin = this->pOrgsInIds[this->numBits][0].begin();
        auto iterIEnd = this->pOrgsInIds[this->numBits][0].end();
        
//        for (auto iterI = iterIBegin; iterI != iterIEnd; iterI++)
//            candidates.push_back(*iterI);
        candidates.insert(candidates.end(), iterIBegin, iterIEnd);
    }
    else
    {
        // Comparisons needed
        auto iterI = this->pOrgsInIds[this->numBits][0].begin();
        auto iterBegin = this->pOrgsInTimestamps[this->numBits][0].begin();
        auto iterEnd = lower_bound(iterBegin, this->pOrgsInTimestamps[this->numBits][0].end(), make_pair<Timestamp, Timestamp>(q.end+1, q.end+1), CompareTimestampPairsByStart);
        for (auto iter = iterBegin; iter != iterEnd; iter++)
        {
            if ((iter->first <= q.end) && (q.start <= iter->second))
                candidates.push_back(*iterI);
            iterI++;
        }
    }
}


inline void HINT_M_SubsSort_CM::scanPartition_CheckBoth_intersect_gOverlaps(unsigned int level, Timestamp t, RelationId **ids, vector<pair<Timestamp, Timestamp>> **timestamps, bool (*compare)(const pair<Timestamp, Timestamp>&, const pair<Timestamp, Timestamp>&), const RangeQuery &q, RelationId &candidates, RelationId &result)
{
    auto iterI = ids[level][t].begin();
    auto iterBegin = timestamps[level][t].begin();
    auto iterEnd = lower_bound(iterBegin, timestamps[level][t].end(), pair<Timestamp, Timestamp>(q.end+1, q.end+1));
    
    for (auto iter = iterBegin; iter != iterEnd; iter++)
    {
#ifdef INTERSECT_WITH_SHRINKING
        if (q.start <= iter->second)
        {
            auto iterC = lower_bound(candidates.begin(), candidates.end(), *iterI);
            if ((iterC != candidates.end()) && (*iterC == *iterI))
            {
                candidates.erase(iterC);
                result.push_back(*iterI);
            }
        }
#else
        if ((q.start <= iter->second) && (binary_search(candidates.begin(), candidates.end(), *iterI)))
            result.push_back(*iterI);
#endif
        iterI++;
    }
}


inline void HINT_M_SubsSort_CM::scanPartition_CheckEnd_intersect_gOverlaps(unsigned int level, Timestamp t, RelationId **ids, vector<pair<Timestamp, Timestamp>> **timestamps, const RangeQuery &q, RelationId &candidates, RelationId &result)
{
    auto iterI = ids[level][t].begin();
    auto iterBegin = timestamps[level][t].begin();
    auto iterEnd = timestamps[level][t].end();

    for (auto iter = iterBegin; iter != iterEnd; iter++)
    {
#ifdef INTERSECT_WITH_SHRINKING
        if (q.start <= iter->second)
        {
            auto iterC = lower_bound(candidates.begin(), candidates.end(), *iterI);
            if ((iterC != candidates.end()) && (*iterC == *iterI))
            {
                candidates.erase(iterC);
                result.push_back(*iterI);
            }
        }
#else
        if ((q.start <= iter->second) && (binary_search(candidates.begin(), candidates.end(), *iterI)))
            result.push_back(*iterI);
#endif
        iterI++;
    }
}


inline void HINT_M_SubsSort_CM::scanPartition_CheckEnd_intersect_gOverlaps(unsigned int level, Timestamp t, RelationId **ids, vector<pair<Timestamp, Timestamp>> **timestamps, bool (*compare)(const pair<Timestamp, Timestamp>&, const pair<Timestamp, Timestamp>&), const RangeQuery &q, RelationId &candidates, RelationId &result)
{
    auto iterI = ids[level][t].begin();
    auto iterBegin = timestamps[level][t].begin();
    auto iterEnd = timestamps[level][t].end();
    auto iter = lower_bound(iterBegin, iterEnd, make_pair(q.start, q.start), compare);
    
    iterI += iter-iterBegin;
    while (iter != iterEnd)
    {
#ifdef INTERSECT_WITH_SHRINKING
        auto iterC = lower_bound(candidates.begin(), candidates.end(), *iterI);
        if ((iterC != candidates.end()) && (*iterC == *iterI))
        {
            candidates.erase(iterC);
            result.push_back(*iterI);
        }
#else
        if (binary_search(candidates.begin(), candidates.end(), *iterI))
            result.push_back(*iterI);
#endif
        iter++;
        iterI++;
    }
}


inline void HINT_M_SubsSort_CM::scanPartition_CheckStart_intersect_gOverlaps(unsigned int level, Timestamp t, RelationId **ids, vector<pair<Timestamp, Timestamp>> **timestamps, bool (*compare)(const pair<Timestamp, Timestamp>&, const pair<Timestamp, Timestamp>&), const RangeQuery &q, RelationId &candidates, RelationId &result)
{
    auto iterI = ids[level][t].begin();
    auto iterBegin = timestamps[level][t].begin();
    auto iterEnd = lower_bound(iterBegin, timestamps[level][t].end(), pair<Timestamp, Timestamp>(q.end+1, q.end+1));

    for (auto iter = iterBegin; iter != iterEnd; iter++)
    {
#ifdef INTERSECT_WITH_SHRINKING
        auto iterC = lower_bound(candidates.begin(), candidates.end(), *iterI);
        if ((iterC != candidates.end()) && (*iterC == *iterI))
        {
            candidates.erase(iterC);
            result.push_back(*iterI);
        }
#else
        if (binary_search(candidates.begin(), candidates.end(), *iterI))
            result.push_back(*iterI);
#endif
        iterI++;
    }
}


inline void HINT_M_SubsSort_CM::scanPartition_NoChecks_intersect_gOverlaps(unsigned int level, Timestamp t, RelationId **ids, RelationId &candidates, RelationId &result)
{
    auto iterIBegin = ids[level][t].begin();
    auto iterIEnd = ids[level][t].end();

    for (auto iterI = iterIBegin; iterI != iterIEnd; iterI++)
    {
#ifdef INTERSECT_WITH_SHRINKING
            auto iterC = lower_bound(candidates.begin(), candidates.end(), *iterI);
            if ((iterC != candidates.end()) && (*iterC == *iterI))
            {
                candidates.erase(iterC);
                result.push_back(*iterI);
            }
#else
            if (binary_search(candidates.begin(), candidates.end(), *iterI))
                result.push_back(*iterI);
#endif
    }
}


inline void HINT_M_SubsSort_CM::scanPartitions_NoChecks_intersect_gOverlaps(unsigned int level, Timestamp ts, Timestamp te, RelationId **ids, RelationId &candidates, RelationId &result)
{
    for (auto j = ts; j <= te; j++)
    {
        auto iterIBegin = ids[level][j].begin();
        auto iterIEnd = ids[level][j].end();
        for (auto iterI = iterIBegin; iterI != iterIEnd; iterI++)
        {
#ifdef INTERSECT_WITH_SHRINKING
            auto iterC = lower_bound(candidates.begin(), candidates.end(), *iterI);
            if ((iterC != candidates.end()) && (*iterC == *iterI))
            {
                candidates.erase(iterC);
                result.push_back(*iterI);
            }
#else
            if (binary_search(candidates.begin(), candidates.end(), *iterI))
                result.push_back(*iterI);
#endif
        }
    }
}


void HINT_M_SubsSort_CM::intersect_gOverlaps(const RangeQuery &q, RelationId &candidates)
{
    if (q.start > this->gend || q.end < this->gstart)
    {
        candidates.clear();
        return;
    }

    RelationId result;
    Timestamp a = (max(q.start,this->gstart)-this->gstart) >> (this->maxBits-this->numBits);
    Timestamp b = (min(q.end,this->gend)-this->gstart)     >> (this->maxBits-this->numBits);
    bool foundzero = false;
    bool foundone = false;
    
    
    for (auto l = 0; l < this->numBits; l++)
    {
        if (foundone && foundzero)
        {
            // Partition totally covers lowest-level partition range that includes query range
            // all contents are guaranteed to be results
            
            // Handle the partition that contains a: consider both originals and replicas
            this->scanPartition_NoChecks_intersect_gOverlaps(l, a, this->pRepsInIds, candidates, result);
            this->scanPartition_NoChecks_intersect_gOverlaps(l, a, this->pRepsAftIds, candidates, result);
            
            // Handle rest: consider only originals
            this->scanPartitions_NoChecks_intersect_gOverlaps(l, a, b, this->pOrgsInIds, candidates, result);
            this->scanPartitions_NoChecks_intersect_gOverlaps(l, a, b, this->pOrgsAftIds, candidates, result);
        }
        else
        {
            // Comparisons needed
            
            // Handle the partition that contains a: consider both originals and replicas, comparisons needed
            if (a == b)
            {
                this->scanPartition_CheckBoth_intersect_gOverlaps(l, a, this->pOrgsInIds, this->pOrgsInTimestamps, CompareTimestampPairsByStart, q, candidates, result);
                this->scanPartition_CheckStart_intersect_gOverlaps(l, a, this->pOrgsAftIds, this->pOrgsAftTimestamps, CompareTimestampPairsByStart, q, candidates, result);
            }

            else
            {
                // Lemma 1
                this->scanPartition_CheckEnd_intersect_gOverlaps(l, a, this->pOrgsInIds, this->pOrgsInTimestamps, q, candidates, result);
                this->scanPartition_NoChecks_intersect_gOverlaps(l, a, this->pOrgsAftIds, candidates, result);
            }

            // Lemma 1, 3
            this->scanPartition_CheckEnd_intersect_gOverlaps(l, a, this->pRepsInIds, this->pRepsInTimestamps, CompareTimestampPairsByEnd, q, candidates, result);
            this->scanPartition_NoChecks_intersect_gOverlaps(l, a, this->pRepsAftIds, candidates, result);

            if (a < b)
            {
                // Handle the rest before the partition that contains b: consider only originals, no comparisons needed
                this->scanPartitions_NoChecks_intersect_gOverlaps(l, a+1, b-1, this->pOrgsInIds, candidates, result);
                this->scanPartitions_NoChecks_intersect_gOverlaps(l, a+1, b-1, this->pOrgsAftIds, candidates, result);

                // Handle the partition that contains b: consider only originals, comparisons needed
                this->scanPartition_CheckStart_intersect_gOverlaps(l, b, this->pOrgsInIds, this->pOrgsInTimestamps, CompareTimestampPairsByStart, q, candidates, result);
                this->scanPartition_CheckStart_intersect_gOverlaps(l, b, this->pOrgsAftIds, this->pOrgsAftTimestamps, CompareTimestampPairsByStart, q, candidates, result);
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
        auto iterIBegin = this->pOrgsInIds[this->numBits][0].begin();
        auto iterIEnd = this->pOrgsInIds[this->numBits][0].end();
        for (auto iterI = iterIBegin; iterI != iterIEnd; iterI++)
        {
            if (binary_search(candidates.begin(), candidates.end(), *iterI))
                result.push_back(*iterI);
        }
    }
    else
    {
        // Comparisons needed
        auto iterI = this->pOrgsInIds[this->numBits][0].begin();
        auto iterBegin = this->pOrgsInTimestamps[this->numBits][0].begin();
        auto iterEnd = lower_bound(iterBegin, this->pOrgsInTimestamps[this->numBits][0].end(), make_pair<Timestamp, Timestamp>(q.end+1, q.end+1), CompareTimestampPairsByStart);
        for (auto iter = iterBegin; iter != iterEnd; iter++)
        {
            if ((iter->first <= q.end) && (q.start <= iter->second) && (binary_search(candidates.begin(), candidates.end(), *iterI)))
                result.push_back(*iterI);
            iterI++;
        }
    }
    
    candidates.swap(result);
}


void HINT_M_SubsSort_CM::intersectAndOutput_gOverlaps(const RangeQuery &q, RelationId &candidates, RelationId &result)
{
    if (q.start > this->gend || q.end < this->gstart)
    {
        result.clear();
        return;
    }

    Timestamp a = (max(q.start,this->gstart)-this->gstart) >> (this->maxBits-this->numBits);
    Timestamp b = (min(q.end,this->gend)-this->gstart)     >> (this->maxBits-this->numBits);
    bool foundzero = false;
    bool foundone = false;
    
    
    for (auto l = 0; l < this->numBits; l++)
    {
        if (foundone && foundzero)
        {
            // Partition totally covers lowest-level partition range that includes query range
            // all contents are guaranteed to be results
            
            // Handle the partition that contains a: consider both originals and replicas
            this->scanPartition_NoChecks_intersect_gOverlaps(l, a, this->pRepsInIds, candidates, result);
            this->scanPartition_NoChecks_intersect_gOverlaps(l, a, this->pRepsAftIds, candidates, result);
            
            // Handle rest: consider only originals
            this->scanPartitions_NoChecks_intersect_gOverlaps(l, a, b, this->pOrgsInIds, candidates, result);
            this->scanPartitions_NoChecks_intersect_gOverlaps(l, a, b, this->pOrgsAftIds, candidates, result);
        }
        else
        {
            // Comparisons needed
            
            // Handle the partition that contains a: consider both originals and replicas, comparisons needed
            if (a == b)
            {
                this->scanPartition_CheckBoth_intersect_gOverlaps(l, a, this->pOrgsInIds, this->pOrgsInTimestamps, CompareTimestampPairsByStart, q, candidates, result);
                this->scanPartition_CheckStart_intersect_gOverlaps(l, a, this->pOrgsAftIds, this->pOrgsAftTimestamps, CompareTimestampPairsByStart, q, candidates, result);
            }

            else
            {
                // Lemma 1
                this->scanPartition_CheckEnd_intersect_gOverlaps(l, a, this->pOrgsInIds, this->pOrgsInTimestamps, q, candidates, result);
                this->scanPartition_NoChecks_intersect_gOverlaps(l, a, this->pOrgsAftIds, candidates, result);
            }

            // Lemma 1, 3
            this->scanPartition_CheckEnd_intersect_gOverlaps(l, a, this->pRepsInIds, this->pRepsInTimestamps, CompareTimestampPairsByEnd, q, candidates, result);
            this->scanPartition_NoChecks_intersect_gOverlaps(l, a, this->pRepsAftIds, candidates, result);

            if (a < b)
            {
                // Handle the rest before the partition that contains b: consider only originals, no comparisons needed
                this->scanPartitions_NoChecks_intersect_gOverlaps(l, a+1, b-1, this->pOrgsInIds, candidates, result);
                this->scanPartitions_NoChecks_intersect_gOverlaps(l, a+1, b-1, this->pOrgsAftIds, candidates, result);

                // Handle the partition that contains b: consider only originals, comparisons needed
                this->scanPartition_CheckStart_intersect_gOverlaps(l, b, this->pOrgsInIds, this->pOrgsInTimestamps, CompareTimestampPairsByStart, q, candidates, result);
                this->scanPartition_CheckStart_intersect_gOverlaps(l, b, this->pOrgsAftIds, this->pOrgsAftTimestamps, CompareTimestampPairsByStart, q, candidates, result);
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
        auto iterIBegin = this->pOrgsInIds[this->numBits][0].begin();
        auto iterIEnd = this->pOrgsInIds[this->numBits][0].end();
        for (auto iterI = iterIBegin; iterI != iterIEnd; iterI++)
        {
            if (binary_search(candidates.begin(), candidates.end(), *iterI))
                result.push_back(*iterI);
        }
    }
    else
    {
        // Comparisons needed
        auto iterI = this->pOrgsInIds[this->numBits][0].begin();
        auto iterBegin = this->pOrgsInTimestamps[this->numBits][0].begin();
        auto iterEnd = lower_bound(iterBegin, this->pOrgsInTimestamps[this->numBits][0].end(), make_pair<Timestamp, Timestamp>(q.end+1, q.end+1), CompareTimestampPairsByStart);
        for (auto iter = iterBegin; iter != iterEnd; iter++)
        {
            if ((iter->first <= q.end) && (q.start <= iter->second) && (binary_search(candidates.begin(), candidates.end(), *iterI)))
                result.push_back(*iterI);
            iterI++;
        }
    }
}



inline void HINT_M_SubsSortByRecordId_CM::updateCounters(const Record &r)
{
    int level = 0;
    Timestamp a = (max(r.start,this->gstart)-this->gstart) >> (this->maxBits-this->numBits);
    Timestamp b = (min(r.end,this->gend)-this->gstart)     >> (this->maxBits-this->numBits);
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


inline void HINT_M_SubsSortByRecordId_CM::updatePartitions(const Record &r)
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
//                    this->pRepsInTmp[level][a][this->pRepsIn_sizes[level][a]] = Record(r.id, r.start, r.end);
//                    this->pRepsIn_sizes[level][a]++;
//                    lastfound = 1;
//                }
//                else
//                {
//                    this->pRepsAftTmp[level][a][this->pRepsAft_sizes[level][a]] = Record(r.id, r.start, r.end);
//                    this->pRepsAft_sizes[level][a]++;
//                }
//            }
//            else
//            {
//                if ((a == b) && (!lastfound))
//                {
//                    this->pOrgsInTmp[level][a][this->pOrgsIn_sizes[level][a]] = Record(r.id, r.start, r.end);
//                    this->pOrgsIn_sizes[level][a]++;
//                }
//                else
//                {
//                    this->pOrgsAftTmp[level][a][this->pOrgsAft_sizes[level][a]] = Record(r.id, r.start, r.end);
//                    this->pOrgsAft_sizes[level][a]++;
//                }
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
//                {
//                    this->pOrgsInTmp[level][prevb][this->pOrgsIn_sizes[level][prevb]] = Record(r.id, r.start, r.end);
//                    this->pOrgsIn_sizes[level][prevb]++;
//                }
//                else
//                {
//                    this->pOrgsAftTmp[level][prevb][this->pOrgsAft_sizes[level][prevb]] = Record(r.id, r.start, r.end);
//                    this->pOrgsAft_sizes[level][prevb]++;
//                }
//            }
//            else
//            {
//                if (!lastfound)
//                {
//                    this->pRepsInTmp[level][prevb][this->pRepsIn_sizes[level][prevb]] = Record(r.id, r.start, r.end);
//                    this->pRepsIn_sizes[level][prevb]++;
//                    lastfound = 1;
//                }
//                else
//                {
//                    this->pRepsAftTmp[level][prevb][this->pRepsAft_sizes[level][prevb]] = Record(r.id, r.start, r.end);
//                    this->pRepsAft_sizes[level][prevb]++;
//                }
//            }
//        }
//        a >>= 1; // a = a div 2
//        b >>= 1; // b = b div 2
//        level++;
//    }
//}
{
    int level = 0;
    Timestamp a = (max(r.start,this->gstart)-this->gstart) >> (this->maxBits-this->numBits);
    Timestamp b = (min(r.end,this->gend)-this->gstart)     >> (this->maxBits-this->numBits);
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
                    this->pRepsInTmp[level][a].push_back(r);
//                    this->pRepsInTmp[level][a][this->pRepsIn_sizes[level][a]] = Record(r.id, r.start, r.end);
//                    this->pRepsIn_sizes[level][a]++;
                    lastfound = 1;
                }
                else
                {
                    this->pRepsAftTmp[level][a].push_back(r);
//                    this->pRepsAftTmp[level][a][this->pRepsAft_sizes[level][a]] = Record(r.id, r.start, r.end);
//                    this->pRepsAft_sizes[level][a]++;
                }
            }
            else
            {
                if ((a == b) && (!lastfound))
                {
                    this->pOrgsInTmp[level][a].push_back(r);
//                    this->pOrgsInTmp[level][a][this->pOrgsIn_sizes[level][a]] = Record(r.id, r.start, r.end);
//                    this->pOrgsIn_sizes[level][a]++;
                }
                else
                {
                    this->pOrgsAftTmp[level][a].push_back(r);
//                    this->pOrgsAftTmp[level][a][this->pOrgsAft_sizes[level][a]] = Record(r.id, r.start, r.end);
//                    this->pOrgsAft_sizes[level][a]++;
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
                    this->pOrgsInTmp[level][prevb].push_back(r);
//                    this->pOrgsInTmp[level][prevb][this->pOrgsIn_sizes[level][prevb]] = Record(r.id, r.start, r.end);
//                    this->pOrgsIn_sizes[level][prevb]++;
                }
                else
                {
                    this->pOrgsAftTmp[level][prevb].push_back(r);
//                    this->pOrgsAftTmp[level][prevb][this->pOrgsAft_sizes[level][prevb]] = Record(r.id, r.start, r.end);
//                    this->pOrgsAft_sizes[level][prevb]++;
                }
            }
            else
            {
                if (!lastfound)
                {
                    this->pRepsInTmp[level][prevb].push_back(r);
//                    this->pRepsInTmp[level][prevb][this->pRepsIn_sizes[level][prevb]] = Record(r.id, r.start, r.end);
//                    this->pRepsIn_sizes[level][prevb]++;
                    lastfound = 1;
                }
                else
                {
                    this->pRepsAftTmp[level][prevb].push_back(r);
//                    this->pRepsAftTmp[level][prevb][this->pRepsAft_sizes[level][prevb]] = Record(r.id, r.start, r.end);
//                    this->pRepsAft_sizes[level][prevb]++;
                }
            }
        }
        a >>= 1; // a = a div 2
        b >>= 1; // b = b div 2
        level++;
    }
}


HINT_M_SubsSortByRecordId_CM::HINT_M_SubsSortByRecordId_CM(const Relation &R, const unsigned int numBits = 0)  : HierarchicalIndex(R, numBits)
{
    // Step 1: one pass to count the contents inside each partition.
    this->pOrgsIn_sizes  = (RecordId **)malloc(this->height*sizeof(RecordId *));
    this->pOrgsAft_sizes = (RecordId **)malloc(this->height*sizeof(RecordId *));
    this->pRepsIn_sizes  = (size_t **)malloc(this->height*sizeof(size_t *));
    this->pRepsAft_sizes = (size_t **)malloc(this->height*sizeof(size_t *));
    
    for (auto l = 0; l < this->height; l++)
    {
        auto cnt = (int)(pow(2, this->numBits-l));
        
        //calloc allocates memory and sets each counter to 0
        this->pOrgsIn_sizes[l]  = (RecordId *)calloc(cnt, sizeof(RecordId));
        this->pOrgsAft_sizes[l] = (RecordId *)calloc(cnt, sizeof(RecordId));
        this->pRepsIn_sizes[l]  = (size_t *)calloc(cnt, sizeof(size_t));
        this->pRepsAft_sizes[l] = (size_t *)calloc(cnt, sizeof(size_t));
    }
    
    for (const Record &r : R)
        this->updateCounters(r);
    
    // Step 2: allocate necessary memory.
    this->pOrgsInTmp  = new Relation*[this->height];
    this->pOrgsAftTmp = new Relation*[this->height];
    this->pRepsInTmp  = new Relation*[this->height];
    this->pRepsAftTmp = new Relation*[this->height];
    
    for (auto l = 0; l < this->height; l++)
    {
        auto cnt = (int)(pow(2, this->numBits-l));
        
        this->pOrgsInTmp[l]  = new Relation[cnt];
        this->pOrgsAftTmp[l] = new Relation[cnt];
        this->pRepsInTmp[l]  = new Relation[cnt];
        this->pRepsAftTmp[l] = new Relation[cnt];
        
        for (auto pId = 0; pId < cnt; pId++)
        {
            this->pOrgsInTmp[l][pId].reserve(this->pOrgsIn_sizes[l][pId]);
            this->pOrgsAftTmp[l][pId].reserve(this->pOrgsAft_sizes[l][pId]);
            this->pRepsInTmp[l][pId].reserve(this->pRepsIn_sizes[l][pId]);
            this->pRepsAftTmp[l][pId].reserve(this->pRepsAft_sizes[l][pId]);
        }
    }
//    for (auto l = 0; l < this->height; l++)
//    {
//        auto cnt = (int)(pow(2, this->numBits-l));
//        
//        memset(this->pOrgsIn_sizes[l], 0, cnt*sizeof(RecordId));
//        memset(this->pOrgsAft_sizes[l], 0, cnt*sizeof(RecordId));
//        memset(this->pRepsIn_sizes[l], 0, cnt*sizeof(size_t));
//        memset(this->pRepsAft_sizes[l], 0, cnt*sizeof(size_t));
//    }
    
    // Step 3: fill partitions.
    for (const Record &r : R)
        this->updatePartitions(r);

    // Step 4: sort partition contents.
    // No need, they're sorted by record id, by construction
//    for (auto l = 0; l < this->height; l++)
//    {
//        auto cnt = (int)(pow(2, this->numBits-l));
//        for (auto pId = 0; pId < cnt; pId++)
//        {
//            this->pOrgsInTmp[l][pId].sortByStart();
//            this->pOrgsAftTmp[l][pId].sortByStart();
//            this->pRepsInTmp[l][pId].sortByEnd();
//        }
//    }
    
    // Free auxiliary memory.
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
            this->pOrgsInIds[l][pId].resize(cnt);
            this->pOrgsInTimestamps[l][pId].resize(cnt);
            for (auto j = 0; j < cnt; j++)
            {
                this->pOrgsInIds[l][pId][j] = this->pOrgsInTmp[l][pId][j].id;
                this->pOrgsInTimestamps[l][pId][j].first = this->pOrgsInTmp[l][pId][j].start;
                this->pOrgsInTimestamps[l][pId][j].second = this->pOrgsInTmp[l][pId][j].end;
            }
            
            cnt = this->pOrgsAftTmp[l][pId].size();
            this->pOrgsAftIds[l][pId].resize(cnt);
            this->pOrgsAftTimestamps[l][pId].resize(cnt);
            for (auto j = 0; j < cnt; j++)
            {
                this->pOrgsAftIds[l][pId][j] = this->pOrgsAftTmp[l][pId][j].id;
                this->pOrgsAftTimestamps[l][pId][j].first = this->pOrgsAftTmp[l][pId][j].start;
                this->pOrgsAftTimestamps[l][pId][j].second = this->pOrgsAftTmp[l][pId][j].end;
            }
            
            cnt = this->pRepsInTmp[l][pId].size();
            this->pRepsInIds[l][pId].resize(cnt);
            this->pRepsInTimestamps[l][pId].resize(cnt);
            for (auto j = 0; j < cnt; j++)
            {
                this->pRepsInIds[l][pId][j] = this->pRepsInTmp[l][pId][j].id;
                this->pRepsInTimestamps[l][pId][j].first = this->pRepsInTmp[l][pId][j].start;
                this->pRepsInTimestamps[l][pId][j].second = this->pRepsInTmp[l][pId][j].end;
            }

            cnt = this->pRepsAftTmp[l][pId].size();
            this->pRepsAftIds[l][pId].resize(cnt);
            this->pRepsAftTimestamps[l][pId].resize(cnt);
            for (auto j = 0; j < cnt; j++)
            {
                this->pRepsAftIds[l][pId][j] = this->pRepsAftTmp[l][pId][j].id;
                this->pRepsAftTimestamps[l][pId][j].first = this->pRepsAftTmp[l][pId][j].start;
                this->pRepsAftTimestamps[l][pId][j].second = this->pRepsAftTmp[l][pId][j].end;
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


HINT_M_SubsSortByRecordId_CM::HINT_M_SubsSortByRecordId_CM(const Relation &R, const Relation &U, const unsigned int numBits = 0)  : HierarchicalIndex(R, U, numBits)
{
    // Step 1: one pass to count the contents inside each partition.
    this->pOrgsIn_sizes  = (RecordId **)malloc(this->height*sizeof(RecordId *));
    this->pOrgsAft_sizes = (RecordId **)malloc(this->height*sizeof(RecordId *));
    this->pRepsIn_sizes  = (size_t **)malloc(this->height*sizeof(size_t *));
    this->pRepsAft_sizes = (size_t **)malloc(this->height*sizeof(size_t *));
    
    for (auto l = 0; l < this->height; l++)
    {
        auto cnt = (int)(pow(2, this->numBits-l));
        
        //calloc allocates memory and sets each counter to 0
        this->pOrgsIn_sizes[l]  = (RecordId *)calloc(cnt, sizeof(RecordId));
        this->pOrgsAft_sizes[l] = (RecordId *)calloc(cnt, sizeof(RecordId));
        this->pRepsIn_sizes[l]  = (size_t *)calloc(cnt, sizeof(size_t));
        this->pRepsAft_sizes[l] = (size_t *)calloc(cnt, sizeof(size_t));
    }
    
    for (const Record &r : R)
        this->updateCounters(r);
    for (const Record &r : U)
        this->updateCounters(r);

    // Step 2: allocate necessary memory.
    this->pOrgsInTmp  = new Relation*[this->height];
    this->pOrgsAftTmp = new Relation*[this->height];
    this->pRepsInTmp  = new Relation*[this->height];
    this->pRepsAftTmp = new Relation*[this->height];
    
    for (auto l = 0; l < this->height; l++)
    {
        auto cnt = (int)(pow(2, this->numBits-l));
        
        this->pOrgsInTmp[l]  = new Relation[cnt];
        this->pOrgsAftTmp[l] = new Relation[cnt];
        this->pRepsInTmp[l]  = new Relation[cnt];
        this->pRepsAftTmp[l] = new Relation[cnt];
        
        for (auto pId = 0; pId < cnt; pId++)
        {
            this->pOrgsInTmp[l][pId].reserve(this->pOrgsIn_sizes[l][pId]);
            this->pOrgsAftTmp[l][pId].reserve(this->pOrgsAft_sizes[l][pId]);
            this->pRepsInTmp[l][pId].reserve(this->pRepsIn_sizes[l][pId]);
            this->pRepsAftTmp[l][pId].reserve(this->pRepsAft_sizes[l][pId]);
        }
    }
//    for (auto l = 0; l < this->height; l++)
//    {
//        auto cnt = (int)(pow(2, this->numBits-l));
//        
//        memset(this->pOrgsIn_sizes[l], 0, cnt*sizeof(RecordId));
//        memset(this->pOrgsAft_sizes[l], 0, cnt*sizeof(RecordId));
//        memset(this->pRepsIn_sizes[l], 0, cnt*sizeof(size_t));
//        memset(this->pRepsAft_sizes[l], 0, cnt*sizeof(size_t));
//    }
    
    // Step 3: fill partitions.
    for (const Record &r : R)
        this->updatePartitions(r);

    // Step 4: sort partition contents.
    // No need, they're sorted by record id, by construction
//    for (auto l = 0; l < this->height; l++)
//    {
//        auto cnt = (int)(pow(2, this->numBits-l));
//        for (auto pId = 0; pId < cnt; pId++)
//        {
//            this->pOrgsInTmp[l][pId].sortByStart();
//            this->pOrgsAftTmp[l][pId].sortByStart();
//            this->pRepsInTmp[l][pId].sortByEnd();
//        }
//    }
    
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
            auto cnt = this->pOrgsIn_sizes[l][pId];
            this->pOrgsInIds[l][pId].reserve(cnt);
            this->pOrgsInTimestamps[l][pId].reserve(cnt);
            for (const Record &r : this->pOrgsInTmp[l][pId])
            {
                this->pOrgsInIds[l][pId].push_back(r.id);
                this->pOrgsInTimestamps[l][pId].push_back(make_pair(r.start, r.end));
            }
            
            cnt = this->pOrgsAft_sizes[l][pId];
            this->pOrgsAftIds[l][pId].reserve(cnt);
            this->pOrgsAftTimestamps[l][pId].reserve(cnt);
            for (const Record &r : this->pOrgsAftTmp[l][pId])
            {
                this->pOrgsAftIds[l][pId].push_back(r.id);
                this->pOrgsAftTimestamps[l][pId].push_back(make_pair(r.start, r.end));
            }
            
            cnt = this->pRepsIn_sizes[l][pId];
            this->pRepsInIds[l][pId].reserve(cnt);
            this->pRepsInTimestamps[l][pId].reserve(cnt);
            for (const Record &r : this->pRepsInTmp[l][pId])
            {
                this->pRepsInIds[l][pId].push_back(r.id);
                this->pRepsInTimestamps[l][pId].push_back(make_pair(r.start, r.end));
            }

            cnt = this->pRepsAft_sizes[l][pId];
            this->pRepsAftIds[l][pId].reserve(cnt);
            this->pRepsAftTimestamps[l][pId].reserve(cnt);
            for (const Record &r : this->pRepsAftTmp[l][pId])
            {
                this->pRepsAftIds[l][pId].push_back(r.id);
                this->pRepsAftTimestamps[l][pId].push_back(make_pair(r.start, r.end));

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
    
    // Free auxiliary memory.
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


void HINT_M_SubsSortByRecordId_CM::getStats()
{
//    size_t sum = 0;
//    for (auto l = 0; l < this->height; l++)
//    {
//        auto cnt = pow(2, this->numBits-l);
//
//        this->numPartitions += cnt;
//        for (int pid = 0; pid < cnt; pid++)
//        {
//            this->numOriginalsIn  += this->pOrgsInIds[l][pid].size();
//            this->numOriginalsAft += this->pOrgsAftIds[l][pid].size();
//            this->numReplicasIn   += this->pRepsInIds[l][pid].size();
//            this->numReplicasAft  += this->pRepsAftIds[l][pid].size();
//            if ((this->pOrgsInIds[l][pid].empty()) && (this->pOrgsAftIds[l][pid].empty()) && (this->pRepsInIds[l][pid].empty()) && (this->pRepsAftIds[l][pid].empty()))
//                this->numEmptyPartitions++;
//        }
//    }
//
//    this->avgPartitionSize = (float)(this->numIndexedRecords+this->numReplicasIn+this->numReplicasAft)/(this->numPartitions-numEmptyPartitions);
}


size_t HINT_M_SubsSortByRecordId_CM::getSize()
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
        }
    }
    
    return size;
}


HINT_M_SubsSortByRecordId_CM::~HINT_M_SubsSortByRecordId_CM()
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
    }
    
    delete[] this->pOrgsInIds;
    delete[] this->pOrgsInTimestamps;
    delete[] this->pOrgsAftIds;
    delete[] this->pOrgsAftTimestamps;
    delete[] this->pRepsInIds;
    delete[] this->pRepsInTimestamps;
    delete[] this->pRepsAftIds;
}



// Updating
void HINT_M_SubsSortByRecordId_CM::insert(const Record &r)
{
    int level = 0;
    Timestamp a = (max(r.start,this->gstart)-this->gstart) >> (this->maxBits-this->numBits);
    Timestamp b = (min(r.end,this->gend)-this->gstart)     >> (this->maxBits-this->numBits);
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
                    this->pRepsInIds[level][a].push_back(r.id);
                    this->pRepsInTimestamps[level][a].push_back(make_pair(r.start, r.end));
                    lastfound = 1;
                }
                else
                {
                    this->pRepsAftIds[level][a].push_back(r.id);
                    this->pRepsAftTimestamps[level][a].push_back(make_pair(r.start, r.end));
                }
            }
            else
            {
                if ((a == b) && (!lastfound))
                {
                    this->pOrgsInIds[level][a].push_back(r.id);
                    this->pOrgsInTimestamps[level][a].push_back(make_pair(r.start, r.end));
                }
                else
                {
                    this->pOrgsAftIds[level][a].push_back(r.id);
                    this->pOrgsAftTimestamps[level][a].push_back(make_pair(r.start, r.end));
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
                    this->pOrgsInTimestamps[level][prevb].push_back(make_pair(r.start, r.end));

                }
                else
                {
                    this->pOrgsAftIds[level][prevb].push_back(r.id);
                    this->pOrgsAftTimestamps[level][prevb].push_back(make_pair(r.start, r.end));
                }
            }
            else
            {
                if (!lastfound)
                {
                    this->pRepsInIds[level][prevb].push_back(r.id);
                    this->pRepsInTimestamps[level][prevb].push_back(make_pair(r.start, r.end));
                    lastfound = 1;
                }
                else
                {
                    this->pRepsAftIds[level][prevb].push_back(r.id);
                    this->pRepsAftTimestamps[level][prevb].push_back(make_pair(r.start, r.end));
                }
            }
        }
        a >>= 1; // a = a div 2
        b >>= 1; // b = b div 2
        level++;
    }
}



// Querying
inline void HINT_M_SubsSortByRecordId_CM::scanPartition_CheckBoth_moveOut_gOverlaps(unsigned int level, Timestamp t, RelationId **ids, vector<pair<Timestamp, Timestamp>> **timestamps, const RangeQuery &q, RelationId &result)
{
    auto iterI = ids[level][t].begin();
    auto iterBegin = timestamps[level][t].begin();
    auto iterEnd = timestamps[level][t].end();
    
    for (auto iter = iterBegin; iter != iterEnd; iter++)
    {
        if ((iter->first <= q.end) && (q.start <= iter->second))
            result.push_back(*iterI);
        iterI++;
    }
}


inline void HINT_M_SubsSortByRecordId_CM::scanPartition_CheckStart_moveOut_gOverlaps(unsigned int level, Timestamp t, RelationId **ids, vector<pair<Timestamp, Timestamp>> **timestamps, const RangeQuery &q, RelationId &result)
{
    auto iterI = ids[level][t].begin();
    auto iterBegin = timestamps[level][t].begin();
    auto iterEnd = timestamps[level][t].end();
    
    for (auto iter = iterBegin; iter != iterEnd; iter++)
    {
        if (iter->first <= q.end)
            result.push_back(*iterI);
        iterI++;
    }
}


inline void HINT_M_SubsSortByRecordId_CM::scanPartition_CheckEnd_moveOut_gOverlaps(unsigned int level, Timestamp t, RelationId **ids, vector<pair<Timestamp, Timestamp>> **timestamps, const RangeQuery &q, RelationId &result)
{
    auto iterI = ids[level][t].begin();
    auto iterBegin = timestamps[level][t].begin();
    auto iterEnd = timestamps[level][t].end();
    
    for (auto iter = iterBegin; iter != iterEnd; iter++)
    {
        if (q.start <= iter->second)
            result.push_back(*iterI);
        iterI++;
    }
}


inline void HINT_M_SubsSortByRecordId_CM::scanPartition_NoChecks_moveOut_gOverlaps(unsigned int level, Timestamp t, RelationId **ids, RelationId &result)
{
    auto iterIBegin = ids[level][t].begin();
    auto iterIEnd = ids[level][t].end();

//    for (auto iterI = iterIBegin; iterI != iterIEnd; iterI++)
//        result.push_back(*iterI);
    result.insert(result.end(), iterIBegin, iterIEnd);
}


inline void HINT_M_SubsSortByRecordId_CM::scanPartitions_NoChecks_moveOut_gOverlaps(unsigned int level, Timestamp ts, Timestamp te, RelationId **ids, RelationId &result)
{
    for (auto j = ts; j <= te; j++)
    {
        auto iterIBegin = ids[level][j].begin();
        auto iterIEnd = ids[level][j].end();
        
//        for (auto iterI = iterIBegin; iterI != iterIEnd; iterI++)
//            result.push_back(*iterI);
        result.insert(result.end(), iterIBegin, iterIEnd);
    }
}


void HINT_M_SubsSortByRecordId_CM::moveOut_gOverlaps(const RangeQuery &q, RelationId &candidates)
{
//    cout<<"edw"<<endl;
    if (q.start > this->gend || q.end < this->gstart)
        return;

    Timestamp a = (max(q.start,this->gstart)-this->gstart) >> (this->maxBits-this->numBits);
    Timestamp b = (min(q.end,this->gend)-this->gstart)     >> (this->maxBits-this->numBits);
    bool foundzero = false;
    bool foundone = false;
    
    
    for (auto l = 0; l < this->numBits; l++)
    {
        if (foundone && foundzero)
        {
            // Partition totally covers lowest-level partition range that includes query range
            // all contents are guaranteed to be results
            
            // Handle the partition that contains a: consider both originals and replicas
            this->scanPartition_NoChecks_moveOut_gOverlaps(l, a, this->pRepsInIds, candidates);
            this->scanPartition_NoChecks_moveOut_gOverlaps(l, a, this->pRepsAftIds, candidates);
            
            // Handle rest: consider only originals
            this->scanPartitions_NoChecks_moveOut_gOverlaps(l, a, b, this->pOrgsInIds, candidates);
            this->scanPartitions_NoChecks_moveOut_gOverlaps(l, a, b, this->pOrgsAftIds, candidates);
        }
        else
        {
            // Comparisons needed
            
            // Handle the partition that contains a: consider both originals and replicas, comparisons needed
            if (a == b)
            {
                this->scanPartition_CheckBoth_moveOut_gOverlaps(l, a, this->pOrgsInIds, this->pOrgsInTimestamps, q, candidates);
                this->scanPartition_CheckStart_moveOut_gOverlaps(l, a, this->pOrgsAftIds, this->pOrgsAftTimestamps, q, candidates);
            }
            else
            {
                // Lemma 1
                this->scanPartition_CheckEnd_moveOut_gOverlaps(l, a, this->pOrgsInIds, this->pOrgsInTimestamps, q, candidates);
                this->scanPartition_NoChecks_moveOut_gOverlaps(l, a, this->pOrgsAftIds, candidates);
            }

            // Lemma 1, 3
            this->scanPartition_CheckEnd_moveOut_gOverlaps(l, a, this->pRepsInIds, this->pRepsInTimestamps, q, candidates);
            this->scanPartition_NoChecks_moveOut_gOverlaps(l, a, this->pRepsAftIds, candidates);

            if (a < b)
            {
                // Handle the rest before the partition that contains b: consider only originals, no comparisons needed
                this->scanPartitions_NoChecks_moveOut_gOverlaps(l, a+1, b-1, this->pOrgsInIds, candidates);
                this->scanPartitions_NoChecks_moveOut_gOverlaps(l, a+1, b-1, this->pOrgsAftIds, candidates);

                // Handle the partition that contains b: consider only originals, comparisons needed
                this->scanPartition_CheckStart_moveOut_gOverlaps(l, b, this->pOrgsInIds, this->pOrgsInTimestamps, q, candidates);
                this->scanPartition_CheckStart_moveOut_gOverlaps(l, b, this->pOrgsAftIds, this->pOrgsAftTimestamps, q, candidates);
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
        auto iterIBegin = this->pOrgsInIds[this->numBits][0].begin();
        auto iterIEnd = this->pOrgsInIds[this->numBits][0].end();
        
//        for (auto iterI = iterIBegin; iterI != iterIEnd; iterI++)
//            candidates.push_back(*iterI);
        candidates.insert(candidates.end(), iterIBegin, iterIEnd);
    }
    else
    {
        // Comparisons needed
        auto iterI = this->pOrgsInIds[this->numBits][0].begin();
        auto iterBegin = this->pOrgsInTimestamps[this->numBits][0].begin();
        auto iterEnd = this->pOrgsInTimestamps[this->numBits][0].end();
        for (auto iter = iterBegin; iter != iterEnd; iter++)
        {
            if ((iter->first <= q.end) && (q.start <= iter->second))
                candidates.push_back(*iterI);
            iterI++;
        }
    }
}

inline void HINT_M_SubsSortByRecordId_CM::scanPartition_NoChecks_intersect_gOverlaps(unsigned int level, Timestamp t, RelationId **ids, RelationId &candidates, RelationId &result)
{
    auto iterIBegin = ids[level][t].begin();
    auto iterIEnd = ids[level][t].end();

#ifdef PROFILING
        cout << "\t|C| = " << candidates.size() << "\tINTERSECT\t|P[" << level << "," << t << "]| = " << (iterIEnd-iterIBegin);
#endif
    if (iterIBegin != iterIEnd)
        mergeSort(iterIBegin, iterIEnd, candidates, result);
#ifdef PROFILING
        cout << " ==> " << result.size() << endl;
#endif
}


void HINT_M_SubsSortByRecordId_CM::intersect_gOverlaps(const RangeQuery &q, RelationId &candidates)
{
    if (q.start > this->gend || q.end < this->gstart)
    {
        candidates.clear();
        return;
    }

    RelationId result;
    RelationIdIterator iterIBegin, iterIEnd;
    Timestamp a = (max(q.start,this->gstart)-this->gstart) >> (this->maxBits-this->numBits);
    Timestamp b = (min(q.end,this->gend)-this->gstart)     >> (this->maxBits-this->numBits);
    bool foundzero = false;
    bool foundone = false;
    
    
    for (auto l = 0; l < this->numBits; l++)
    {
        // Handle the partition that contains a: consider both originals and replicas
        this->scanPartition_NoChecks_intersect_gOverlaps(l, a, this->pRepsInIds, candidates, result);
        this->scanPartition_NoChecks_intersect_gOverlaps(l, a, this->pRepsAftIds, candidates, result);

        // Handle rest: consider only originals
        for (auto i = a; i <= b; i++)
        {
            this->scanPartition_NoChecks_intersect_gOverlaps(l, i, this->pOrgsInIds, candidates, result);
            this->scanPartition_NoChecks_intersect_gOverlaps(l, i, this->pOrgsAftIds, candidates, result);
        }

        a >>= 1; // a = a div 2
        b >>= 1; // b = b div 2
    }
    
    // Handle root.
    iterIBegin = this->pOrgsInIds[this->numBits][0].begin();
    iterIEnd = this->pOrgsInIds[this->numBits][0].end();
    if (iterIBegin != iterIEnd)
        mergeSort(iterIBegin, iterIEnd, candidates, result);
    
    candidates.swap(result);
}


void HINT_M_SubsSortByRecordId_CM::intersectAndOutput_gOverlaps(const RangeQuery &q, RelationId &candidates, RelationId &result)
{
    if (q.start > this->gend || q.end < this->gstart)
    {
        result.clear();
        return;
    }

    RelationIdIterator iterIBegin, iterIEnd;
    Timestamp a = (max(q.start,this->gstart)-this->gstart) >> (this->maxBits-this->numBits);
    Timestamp b = (min(q.end,this->gend)-this->gstart)     >> (this->maxBits-this->numBits);
    bool foundzero = false;
    bool foundone = false;
    
    
    for (auto l = 0; l < this->numBits; l++)
    {
        // Handle the partition that contains a: consider both originals and replicas
        this->scanPartition_NoChecks_intersect_gOverlaps(l, a, this->pRepsInIds, candidates, result);
        this->scanPartition_NoChecks_intersect_gOverlaps(l, a, this->pRepsAftIds, candidates, result);
        
        // Handle rest: consider only originals
        for (auto i = a; i <= b; i++)
        {
            this->scanPartition_NoChecks_intersect_gOverlaps(l, i, this->pOrgsInIds, candidates, result);
            this->scanPartition_NoChecks_intersect_gOverlaps(l, i, this->pOrgsAftIds, candidates, result);
        }
        
        a >>= 1; // a = a div 2
        b >>= 1; // b = b div 2
    }
    
    // Handle root.
    iterIBegin = this->pOrgsInIds[this->numBits][0].begin();
    iterIEnd = this->pOrgsInIds[this->numBits][0].end();
    if (iterIBegin != iterIEnd)
        mergeSort(iterIBegin, iterIEnd, candidates, result);
}



// M-way
inline void HINT_M_SubsSortByRecordId_CM::scanPartition_CheckBoth_moveOut_gOverlaps(unsigned int level, Timestamp t, RelationId **ids, vector<pair<Timestamp, Timestamp>> **timestamps, const RangeQuery &q, vector<RelationId> &vec_result)
{
    auto iterI = ids[level][t].begin();
    auto iterBegin = timestamps[level][t].begin();
    auto iterEnd = timestamps[level][t].end();
    RelationId tmp;
    
    for (auto iter = iterBegin; iter != iterEnd; iter++)
    {
        if ((iter->first <= q.end) && (q.start <= iter->second))
            tmp.push_back(*iterI);
        iterI++;
    }
    
    if (!tmp.empty())
        vec_result.push_back(tmp);
}


inline void HINT_M_SubsSortByRecordId_CM::scanPartition_CheckEnd_moveOut_gOverlaps(unsigned int level, Timestamp t, RelationId **ids, vector<pair<Timestamp, Timestamp>> **timestamps, const RangeQuery &q, vector<RelationId> &vec_result)
{
    auto iterI = ids[level][t].begin();
    auto iterBegin = timestamps[level][t].begin();
    auto iterEnd = timestamps[level][t].end();
    RelationId tmp;

    for (auto iter = iterBegin; iter != iterEnd; iter++)
    {
        if (q.start <= iter->second)
            tmp.push_back(*iterI);
        iterI++;
    }
    
    if (!tmp.empty())
        vec_result.push_back(tmp);
}


inline void HINT_M_SubsSortByRecordId_CM::scanPartition_CheckStart_moveOut_gOverlaps(unsigned int level, Timestamp t, RelationId **ids, vector<pair<Timestamp, Timestamp>> **timestamps, const RangeQuery &q, vector<RelationId> &vec_result)
{
    auto iterI = ids[level][t].begin();
    auto iterBegin = timestamps[level][t].begin();
    auto iterEnd = timestamps[level][t].end();
    RelationId tmp;
    
    for (auto iter = iterBegin; iter != iterEnd; iter++)
    {
        if (iter->first <= q.end)
            tmp.push_back(*iterI);
        iterI++;
    }
    
    if (!tmp.empty())
        vec_result.push_back(tmp);
}


inline void HINT_M_SubsSortByRecordId_CM::scanPartition_NoChecks_moveOut_gOverlaps(unsigned int level, Timestamp t, RelationId **ids, vector<RelationId> &vec_result)
{
    auto iterIBegin = ids[level][t].begin();
    auto iterIEnd = ids[level][t].end();

    if (iterIBegin != iterIEnd)
        vec_result.push_back(RelationId(iterIBegin, iterIEnd));
}


inline void HINT_M_SubsSortByRecordId_CM::scanPartitions_NoChecks_moveOut_gOverlaps(unsigned int level, Timestamp ts, Timestamp te, RelationId **ids, vector<RelationId> &vec_result)
{
    for (auto j = ts; j <= te; j++)
    {
        auto iterIBegin = ids[level][j].begin();
        auto iterIEnd = ids[level][j].end();

        if (iterIBegin != iterIEnd)
            vec_result.push_back(RelationId(iterIBegin, iterIEnd));
    }
}


void HINT_M_SubsSortByRecordId_CM::moveOut_gOverlaps(const RangeQuery &q, vector<RelationId> &vec_candidates)
{
    if (q.start > this->gend || q.end < this->gstart)
        return;

    RelationIdIterator iterI, iterIBegin, iterIEnd;
    Timestamp a = (max(q.start,this->gstart)-this->gstart) >> (this->maxBits-this->numBits);
    Timestamp b = (min(q.end,this->gend)-this->gstart)     >> (this->maxBits-this->numBits);
    bool foundzero = false;
    bool foundone = false;
    
    
    for (auto l = 0; l < this->numBits; l++)
    {
        if (foundone && foundzero)
        {
            // Partition totally covers lowest-level partition range that includes query range
            // all contents are guaranteed to be results
            
            // Handle the partition that contains a: consider both originals and replicas
            this->scanPartition_NoChecks_moveOut_gOverlaps(l, a, this->pRepsInIds, vec_candidates);
            this->scanPartition_NoChecks_moveOut_gOverlaps(l, a, this->pRepsAftIds, vec_candidates);
            
            // Handle rest: consider only originals
            this->scanPartitions_NoChecks_moveOut_gOverlaps(l, a, b, this->pOrgsInIds, vec_candidates);
            this->scanPartitions_NoChecks_moveOut_gOverlaps(l, a, b, this->pOrgsAftIds, vec_candidates);
        }
        else
        {
            // Comparisons needed
            
            // Handle the partition that contains a: consider both originals and replicas, comparisons needed
            if (a == b)
            {
                this->scanPartition_CheckBoth_moveOut_gOverlaps(l, a, this->pOrgsInIds, this->pOrgsInTimestamps, q, vec_candidates);
                this->scanPartition_CheckStart_moveOut_gOverlaps(l, a, this->pOrgsAftIds, this->pOrgsAftTimestamps, q, vec_candidates);
            }

            else
            {
                // Lemma 1
                this->scanPartition_CheckEnd_moveOut_gOverlaps(l, a, this->pOrgsInIds, this->pOrgsInTimestamps, q, vec_candidates);
                this->scanPartition_NoChecks_moveOut_gOverlaps(l, a, this->pOrgsAftIds, vec_candidates);
            }

            // Lemma 1, 3
            this->scanPartition_CheckEnd_moveOut_gOverlaps(l, a, this->pRepsInIds, this->pRepsInTimestamps, q, vec_candidates);
            this->scanPartition_NoChecks_moveOut_gOverlaps(l, a, this->pRepsAftIds, vec_candidates);

            if (a < b)
            {
                // Handle the rest before the partition that contains b: consider only originals, no comparisons needed
                this->scanPartitions_NoChecks_moveOut_gOverlaps(l, a+1, b-1, this->pOrgsInIds, vec_candidates);
                this->scanPartitions_NoChecks_moveOut_gOverlaps(l, a+1, b-1, this->pOrgsAftIds, vec_candidates);

                // Handle the partition that contains b: consider only originals, comparisons needed
                this->scanPartition_CheckStart_moveOut_gOverlaps(l, b, this->pOrgsInIds, this->pOrgsInTimestamps, q, vec_candidates);
                this->scanPartition_CheckStart_moveOut_gOverlaps(l, b, this->pOrgsAftIds, this->pOrgsAftTimestamps, q, vec_candidates);
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
        iterIBegin = this->pOrgsInIds[this->numBits][0].begin();
        iterIEnd = this->pOrgsInIds[this->numBits][0].end();
        if (iterIBegin != iterIEnd)
            vec_candidates.push_back(RelationId(iterIBegin, iterIEnd));
    }
    else
    {
        // Comparisons needed
        RelationId tmp;
        auto iterI = this->pOrgsInIds[this->numBits][0].begin();
        auto iterBegin = this->pOrgsInTimestamps[this->numBits][0].begin();
        auto iterEnd = this->pOrgsInTimestamps[this->numBits][0].end();
        for (auto iter = iterBegin; iter != iterEnd; iter++)
        {
            if ((iter->first <= q.end) && (q.start <= iter->second))
                tmp.push_back(*iterI);
            iterI++;
        }

        if (!tmp.empty())
            vec_candidates.push_back(tmp);
    }
}


void HINT_M_SubsSortByRecordId_CM::moveOut_NoChecks_gOverlaps(const RangeQuery &q, vector<RelationId> &vec_candidates)
{
    if (q.start > this->gend || q.end < this->gstart) return;

    RelationIdIterator iterI, iterIBegin, iterIEnd;
    Timestamp a = (max(q.start,this->gstart)-this->gstart) >> (this->maxBits-this->numBits);
    Timestamp b = (min(q.end,this->gend)-this->gstart)     >> (this->maxBits-this->numBits);
    bool foundzero = false;
    bool foundone = false;
    
    
    for (auto l = 0; l < this->numBits; l++)
    {
        // Handle the partition that contains a: consider both originals and replicas
        this->scanPartition_NoChecks_moveOut_gOverlaps(l, a, this->pRepsInIds, vec_candidates);
        this->scanPartition_NoChecks_moveOut_gOverlaps(l, a, this->pRepsAftIds, vec_candidates);
        
        // Handle rest: consider only originals
        this->scanPartitions_NoChecks_moveOut_gOverlaps(l, a, b, this->pOrgsInIds, vec_candidates);
        this->scanPartitions_NoChecks_moveOut_gOverlaps(l, a, b, this->pOrgsAftIds, vec_candidates);

        
        a >>= 1; // a = a div 2
        b >>= 1; // b = b div 2
    }
    
    // Handle root.
    iterIBegin = this->pOrgsInIds[this->numBits][0].begin();
    iterIEnd = this->pOrgsInIds[this->numBits][0].end();
    if (iterIBegin != iterIEnd)
        vec_candidates.push_back(RelationId(iterIBegin, iterIEnd));
}


inline void HINT_M_SubsSortByRecordId_CM::scanPartition_NoChecks_intersect_gOverlaps(unsigned int level, Timestamp t, RelationId **ids, RelationId &candidates, vector<RelationId> &vec_result)
{
    auto iterIBegin = ids[level][t].begin();
    auto iterIEnd = ids[level][t].end();


#ifdef PROFILING
    cout << "\t|C| = " << candidates.size() << "\tINTERSECT\t|P[" << level << "," << t << "]| = " << (iterIEnd-iterIBegin);
#endif
    if (iterIBegin != iterIEnd)
        mergeSort(iterIBegin, iterIEnd, candidates, vec_result);
#ifdef PROFILING
    cout << " ==> " << (vec_result.end()-1)->size() << endl;
#endif
}


void HINT_M_SubsSortByRecordId_CM::intersect_gOverlaps(const RangeQuery &q, RelationId &candidates, vector<RelationId> &vec_candidates)
{
    if (q.start > this->gend || q.end < this->gstart)
    {
        candidates.clear();
        return;
    }

    RelationId result;
    RelationIdIterator iterIBegin, iterIEnd;
    Timestamp a = (max(q.start,this->gstart)-this->gstart) >> (this->maxBits-this->numBits);
    Timestamp b = (min(q.end,this->gend)-this->gstart)     >> (this->maxBits-this->numBits);
    bool foundzero = false;
    bool foundone = false;
    
    
    for (auto l = 0; l < this->numBits; l++)
    {
        // Handle the partition that contains a: consider both originals and replicas
        this->scanPartition_NoChecks_intersect_gOverlaps(l, a, this->pRepsInIds, candidates, vec_candidates);
        this->scanPartition_NoChecks_intersect_gOverlaps(l, a, this->pRepsAftIds, candidates, vec_candidates);

        // Handle rest: consider only originals
        for (auto i = a; i <= b; i++)
        {
            this->scanPartition_NoChecks_intersect_gOverlaps(l, i, this->pOrgsInIds, candidates, vec_candidates);
            this->scanPartition_NoChecks_intersect_gOverlaps(l, i, this->pOrgsAftIds, candidates, vec_candidates);
        }

        a >>= 1; // a = a div 2
        b >>= 1; // b = b div 2
    }
    
    // Handle root.
    iterIBegin = this->pOrgsInIds[this->numBits][0].begin();
    iterIEnd = this->pOrgsInIds[this->numBits][0].end();
    if (iterIBegin != iterIEnd)
        mergeSort(iterIBegin, iterIEnd, candidates, vec_candidates);
}

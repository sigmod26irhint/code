#include "relations.h"
#include <unordered_set>



// Record class
Record::Record()
{
}


Record::Record(RecordId id, Timestamp start, Timestamp end)
{
    this->id = id;
    this->start = start;
    this->end = end;
}


bool Record::operator < (const Record& rhs) const
{
    if (this->start == rhs.start)
        return this->end < rhs.end;
    else
        return this->start < rhs.start;
}


bool Record::operator >= (const Record& rhs) const
{
    return !((*this) < rhs);
}


void Record::print(const char c) const
{
    cout << c << this->id << ": [" << this->start << ".." << this->end << "]" << endl;
}


Record::~Record()
{
}


// RecordStart class
RecordStart::RecordStart()
{
}


RecordStart::RecordStart(RecordId id, Timestamp start)
{
    this->id = id;
    this->start = start;
}


bool RecordStart::operator < (const RecordStart& rhs) const
{
    if (this->start == rhs.start)
        return this->id < rhs.id;
    else
        return this->start < rhs.start;
}


bool RecordStart::operator >= (const RecordStart& rhs) const
{
    return !((*this) < rhs);
}


void RecordStart::print(const char c) const
{
    cout << c << this->id << ": " << this->start << endl;
}


RecordStart::~RecordStart()
{
}


// IRecord class
IRecord::IRecord()
{
}


IRecord::IRecord(RecordId id, Timestamp start, Timestamp end) : Record(id, start, end)
{
}


// For now only a binary search approach is implemented
bool IRecord::containsTerms(const vector<TermId> &terms, const unsigned int offset) const
{
    vector<TermId>::const_iterator iterTBegin = this->terms.begin();
    vector<TermId>::const_iterator iterTEnd = this->terms.end();

    for (auto i = offset; i < terms.size(); i++)
    {
        if (!binary_search(iterTBegin, iterTEnd, terms[i], std::greater<TermId>()))
            return false;
    }
    
    return true;
}


void IRecord::print(const char c) const
{
    size_t tsize = this->terms.size();
    
    cout << c << this->id << ": [" << this->start << ".." << this->end << "]\t{";
    for (auto i = 0; i < tsize-1; i++)
        cout << this->terms[i] << ",";
    cout << this->terms[tsize-1] << "}" << endl;
}



// Relation class
Relation::Relation()
{
    this->init();
}


Relation::Relation(const Relation &R) : vector<Record>(R)
{
    this->gstart          = R.gstart;
    this->gend            = R.gend;
    this->domainSize      = R.domainSize;
    this->longestRecord   = R.longestRecord;
    this->avgRecordExtent = R.avgRecordExtent;
}


Relation::Relation(const char *filename)
{
    Timestamp rstart, rend;
    ifstream inp(filename);
    size_t sum = 0;
    RecordId numRecords = 0;

 
    if (!inp)
    {
        cerr << endl << "Error - cannot open data file \"" << filename << "\"" << endl << endl;
        exit(1);
    }

    this->init();
    while (inp >> rstart >> rend)
    {
        if (rstart > rend)
        {
            cerr << endl << "Error - start is after end for interval [" << rstart << ".." << rend << "]" << endl << endl;
            exit(1);
        }
        
        this->emplace_back(numRecords, rstart, rend);
        numRecords++;

        this->gstart = std::min(this->gstart, rstart);
        this->gend   = std::max(this->gend  , rend);
        this->longestRecord = std::max(this->longestRecord, rend-rstart+1);
        sum += rend-rstart;
    }
    inp.close();
    
    this->avgRecordExtent = (float)sum/this->size();
    this->domainSize      = this->gend-this->gstart;
}


void Relation::init()
{
    this->gstart          = std::numeric_limits<Timestamp>::max();
    this->gend            = std::numeric_limits<Timestamp>::min();
    this->domainSize      = 0;
    this->longestRecord   = std::numeric_limits<Timestamp>::min();
    this->avgRecordExtent = 0;
}


void Relation::sortById()
{
    sort(this->begin(), this->end(), compareRecordsById);
}


void Relation::sortByStart()
{
    sort(this->begin(), this->end());
}


void Relation::sortByEnd()
{
    sort(this->begin(), this->end(), compareRecordsByEnd);
}


void Relation::print(char c) const
{
    for (const Record& rec : (*this))
        rec.print(c);
}


Relation::~Relation()
{
}



// RelationStart class
RelationStart::RelationStart()
{
    this->init();
}


RelationStart::RelationStart(const RelationStart &R) : vector<RecordStart>(R)
{
    this->gstart          = R.gstart;
    this->domainSize      = R.domainSize;
    this->longestRecord   = R.longestRecord;
    this->avgRecordExtent = R.avgRecordExtent;
}


void RelationStart::init()
{
    this->gstart          = std::numeric_limits<Timestamp>::max();
    this->domainSize      = 0;
    this->longestRecord   = std::numeric_limits<Timestamp>::min();
    this->avgRecordExtent = 0;
}


//void RelationStart::sortById()
//{
//    sort(this->begin(), this->end(), compareRecordsById);
//}


void RelationStart::sortByStart()
{
    sort(this->begin(), this->end());
}


void RelationStart::print(char c) const
{
    for (const RecordStart& rec : (*this))
        rec.print(c);
}


RelationStart::~RelationStart()
{
}



// IRelation class
IRelation::IRelation()
{
    this->init();
}


IRelation::IRelation(const IRelation &iR) : vector<IRecord>(iR)
{
    this->gstart          = iR.gstart;
    this->gend            = iR.gend;
    this->domainSize      = iR.domainSize;
    this->dictionarySize  = iR.dictionarySize;
    this->longestRecord   = iR.longestRecord;
    this->longestTermsSet = iR.longestTermsSet;
    this->avgRecordExtent = iR.avgRecordExtent;
    this->avgTermsSetSize = iR.avgTermsSetSize;
}


IRelation::IRelation(const char *filename)
{
    Timestamp rstart, rend;
    string rterms, rt;
    ifstream inp(filename);
    size_t sumI = 0, sumT = 0;
    RecordId numRecords = 0;
//    unordered_set<TermId> dict;

    
    if (!inp)
    {
        cerr << endl << "Error - cannot open data file \"" << filename << "\"" << endl << endl;
        exit(1);
    }

//    TermId maxTID = 0;
    this->init();
    while (inp >> rstart >> rend >> rterms)
    {
        if (rstart > rend)
        {
            cerr << endl << "Error - start is after end for interval [" << rstart << ".." << rend << "]" << endl << endl;
            exit(1);
        }

        IRecord r(numRecords, rstart, rend);
        stringstream ss(rterms);
        while (getline(ss, rt, ','))
        {
            TermId tid = stoi(rt);

            r.terms.push_back(tid);
//            dict.insert(tid);
            this->histo[tid]++;
            
//            maxTID = max(maxTID, tid);
        }

        this->push_back(r);
        numRecords++;

        this->gstart = std::min(this->gstart, rstart);
        this->gend   = std::max(this->gend  , rend);
        this->longestRecord = std::max(this->longestRecord, rend-rstart);
        this->longestTermsSet = std::max(this->longestTermsSet, (TermId)r.terms.size());
        sumI += rend-rstart;
        sumT += r.terms.size();
    }
    inp.close();
    
    this->domainSize      = this->gend-this->gstart;
//    this->dictionarySize  = dict.size();
    this->dictionarySize  = histo.size();
    this->avgRecordExtent = (float)sumI/this->size();
    this->avgTermsSetSize = (float)sumT/this->size();
    
//    cout<<"maxTID = " << maxTID << endl;
}


void IRelation::init()
{
    this->gstart          = std::numeric_limits<Timestamp>::max();
    this->gend            = std::numeric_limits<Timestamp>::min();
    this->domainSize      = 0;
    this->dictionarySize  = 0;
    this->longestRecord   = std::numeric_limits<Timestamp>::min();
    this->longestTermsSet = std::numeric_limits<TermId>::min();
    this->avgRecordExtent = 0;
    this->avgTermsSetSize = 0;
}


RecordId IRelation::getFrequency(const TermId tid)
{
    return this->histo[tid];
}


void IRelation::sortById()
{
    sort(this->begin(), this->end(), compareRecordsById);
}


void IRelation::sortByStart()
{
    sort(this->begin(), this->end());
}


void IRelation::sortByEnd()
{
    sort(this->begin(), this->end(), compareRecordsByEnd);
}


void IRelation::print(char c) const
{
    for (const IRecord& rec : (*this))
        rec.print(c);
}


void IRelation::executeContainment(const RangeIRQuery &q, RelationId &result)
{
    for (const IRecord &r : (*this))
    {
        if ((r.start <= q.end) && (q.start <= r.end) && (r.containsTerms(q.terms, 0)))
            result.push_back(r.id);
    }
}


IRelation::~IRelation()
{
}

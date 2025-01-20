#ifndef _INVERTED_FILE_H_
#define _INVERTED_FILE_H_

#include <unordered_map>
#include "../containers/relations.h"
#include "../def_global.h"



class InvertedFileTemplate
{
public:
    // Statistics
    RecordId numIndexedRecords;
    TermId numEmptyLists;
    TermId avgListSize;

    // Constructing
    InvertedFileTemplate()          { this->numEmptyLists = this->avgListSize = 0; };
    virtual void getStats()         { };
    virtual size_t getSize()        { return 0; };
    virtual void print(char c)      { };
    virtual ~InvertedFileTemplate() { };
    
    // Updating
    virtual void insert(IRelation &U) { };
    
    // Querying
    virtual void executeContainment(const RangeIRQuery &q, RelationId &result) { };
    virtual void executeContainment(const RangeIRQuery &q, vector<RelationId> &vec_result) {};
};



class InvertedFile : public InvertedFileTemplate
{
private:
    // Indexed relation
    IRelation R;

    // Posting lists
    unordered_map<TermId, RelationId> lists;
    
public:

    // Constructing
    InvertedFile();
    InvertedFile(const IRelation &R);
    void index(const IRecord &r);
    void getStats();
    size_t getSize();
    void print(char c);
    ~InvertedFile();

    // Querying
    bool moveOut(const RangeIRQuery &q, RelationId &candidates);
    bool intersect(const RangeIRQuery &q, const unsigned int termoff, RelationId &candidates);
    void intersectAndOutput(const RangeIRQuery &q, const unsigned int off, RelationId &candidates, RelationId &result);
};
#endif // _INVERTED_FILE_H_

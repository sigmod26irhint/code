#ifndef _HIERARCHICAL_INDEX_H_
#define _HIERARCHICAL_INDEX_H_

#include "../def_global.h"
#include "../containers/relations.h"



// Frameworks
class HierarchicalIndex
{
protected:
    // Indexed relation
    const Relation *R;

    // Index parameters
    Timestamp gstart;
    Timestamp gend;
    unsigned int maxBits;
    unsigned int height;
    unsigned int numBits_cutoff;
    
    // Construction
    virtual inline void updateCounters(const Record &r) {};
    virtual inline void updatePartitions(const Record &r) {};

public:
    // Index parameters
    unsigned int numBits;

    // Statistics
    size_t numPartitions;
    size_t numEmptyPartitions;
    float avgPartitionSize;
    size_t numOriginals, numReplicas;
    size_t numOriginalsIn, numOriginalsAft, numReplicasIn, numReplicasAft;

    // Construction
    HierarchicalIndex(const Relation &R, const unsigned numBits);
    HierarchicalIndex(const Relation &R, const Relation &U, const unsigned numBits);    // For updates only
    virtual void print(const char c) {};
    virtual void getStats() {};
    virtual size_t getSize() {};
    virtual ~HierarchicalIndex() {};
    
    // Querying
    virtual size_t executeBottomUp_gOverlaps(RangeQuery Q) {return 0;};
};



class HierarchicalIRIndex
{
protected:
    // Indexed relation
    const IRelation *R;

    // Index parameters
    Timestamp gstart;
    Timestamp gend;
    unsigned int maxBits;
    unsigned int height;
    
    // Construction
    virtual inline void updateCounters(const IRecord &r) {};
    virtual inline void updatePartitions(const IRecord &r) {};

public:
    // Index parameters
    unsigned int numBits;

    // Statistics
    size_t numPartitions;
    size_t numEmptyPartitions;
    float avgPartitionSize;
    size_t numOriginals, numReplicas;
    size_t numOriginalsIn, numOriginalsAft, numReplicasIn, numReplicasAft;

    // Construction
    HierarchicalIRIndex(const IRelation &R, const unsigned int numBits);
    HierarchicalIRIndex(const IRelation &R, const IRelation &U, const unsigned int numBits);    // For updates only
    virtual void print(const char c) {};
    virtual void getStats() {};
    virtual size_t getSize() {};
    virtual ~HierarchicalIRIndex() {};    

    // Updating
    virtual void insert(IRelation &U) {};

    // Querying
    virtual void executeContainment(const RangeIRQuery &q, RelationId &result) { };
};
#endif // _HIERARCHICAL_INDEX_H_

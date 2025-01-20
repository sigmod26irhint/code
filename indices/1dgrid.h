#ifndef _1D_GRID_H_
#define _1D_GRID_H_

#include "../def_global.h"
#include "../containers/relations.h"



class OneDimensionalGrid
{
private:
    PartitionId numPartitions;
    PartitionId numPartitionsMinus1;
    Timestamp gstart;
    Timestamp gend;
    Timestamp partitionExtent;
    RecordId numIndexedRecords;

    Relation *pRecs;
    size_t *pRecs_sizes;

    // Construction
    inline void updateCounters(const Record &r);
    inline void updatePartitions(const Record &r);

public:
    // Statistics
    PartitionId numEmptyPartitions;
    float avgPartitionSize;
    size_t numReplicas;
    
    // Construction
    OneDimensionalGrid();
    OneDimensionalGrid(const Relation &R, const PartitionId numPartitions);
    OneDimensionalGrid(const Relation &R, const Relation &U, const PartitionId numPartitions);  // For updates only
    void print(char c);
    void getStats();
    size_t getSize();
    ~OneDimensionalGrid();

    // Updating
    void insert(const Record &r);

    // Querying
    void moveOut_checkBoth_gOverlaps(const RangeQuery &q, RelationId &candidates);
    void interesect_gOverlaps(const RangeQuery &q, RelationId &candidates);
    void interesectAndOutput_gOverlaps(const RangeQuery &q, RelationId &candidates, RelationId &result);
};



class OneDimensionalGrid_RecordStart
{
private:
    PartitionId numPartitions;
    PartitionId numPartitionsMinus1;
    Timestamp gstart;
    Timestamp gend;
    Timestamp partitionExtent;
    RecordId numIndexedRecords;

    RelationStart *pRecs;
    size_t *pRecs_sizes;

    // Construction
    inline void updateCounters(const Record &r);
    inline void updatePartitions(const Record &r);

public:
    // Statistics
    PartitionId numEmptyPartitions;
    float avgPartitionSize;
    size_t numReplicas;
    
    // Construction
    OneDimensionalGrid_RecordStart();
    OneDimensionalGrid_RecordStart(const Relation &R, const PartitionId numPartitions);
    OneDimensionalGrid_RecordStart(const Relation &R, const Relation &U, const PartitionId numPartitions);
    void print(char c);
    void getStats();
    size_t getSize();
    ~OneDimensionalGrid_RecordStart();

    // Updating
    void insert(const Record &r);

    // Querying
    void interesect_gOverlaps(const RangeQuery &q, RelationId &candidates);
    void interesectAndOutput_gOverlaps(const RangeQuery &q, RelationId &candidates, RelationId &result);    // For updates only
};
#endif // _1D_GRID_H_

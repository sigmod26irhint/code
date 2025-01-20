#ifndef _HINT_M_H_
#define _HINT_M_H_

#include "../def_global.h"
#include "../containers/relations.h"
#include "../containers/offsets.h"
#include "../indices/hierarchical_index.h"
#include <boost/dynamic_bitset.hpp>



// HINT^m with subs+sort and cash misses optimizations activated
class HINT_M_SubsSort_CM : public HierarchicalIndex
{
private:
    Relation      **pOrgsInTmp;
    Relation      **pOrgsAftTmp;
    Relation      **pRepsInTmp;
    Relation      **pRepsAftTmp;
    
    RelationId    **pOrgsInIds;
    RelationId    **pOrgsAftIds;
    RelationId    **pRepsInIds;
    RelationId    **pRepsAftIds;
    vector<pair<Timestamp, Timestamp> > **pOrgsInTimestamps;
    vector<pair<Timestamp, Timestamp> > **pOrgsAftTimestamps;
    vector<pair<Timestamp, Timestamp> > **pRepsInTimestamps;
    vector<pair<Timestamp, Timestamp> > **pRepsAftTimestamps;
    
    RecordId      **pOrgsIn_sizes, **pOrgsAft_sizes;
    size_t        **pRepsIn_sizes, **pRepsAft_sizes;
    
    // Construction
    inline void updateCounters(const Record &r);
    inline void updatePartitions(const Record &r);
    
    // Querying
    // Auxiliary functions to scan a partition.
    inline void scanPartition_CheckBoth_moveOut_gOverlaps(unsigned int level, Timestamp t, RelationId **ids, vector<pair<Timestamp, Timestamp>> **timestamps, bool (*compare)(const pair<Timestamp, Timestamp>&, const pair<Timestamp, Timestamp>&), const RangeQuery &Q, RelationId &result);
    inline void scanPartition_CheckEnd_moveOut_gOverlaps(unsigned int level, Timestamp t, RelationId **ids, vector<pair<Timestamp, Timestamp>> **timestamps, const RangeQuery &Q, RelationId &result);
    inline void scanPartition_CheckEnd_moveOut_gOverlaps(unsigned int level, Timestamp t, RelationId **ids, vector<pair<Timestamp, Timestamp>> **timestamps, bool (*compare)(const pair<Timestamp, Timestamp>&, const pair<Timestamp, Timestamp>&), const RangeQuery &Q, RelationId &result);
    inline void scanPartition_CheckStart_moveOut_gOverlaps(unsigned int level, Timestamp t, RelationId **ids, vector<pair<Timestamp, Timestamp>> **timestamps, bool (*compare)(const pair<Timestamp, Timestamp>&, const pair<Timestamp, Timestamp>&), const RangeQuery &Q, RelationId &result);
    inline void scanPartition_NoChecks_moveOut_gOverlaps(unsigned int level, Timestamp t, RelationId **ids, RelationId &result);
    inline void scanPartitions_NoChecks_moveOut_gOverlaps(unsigned int level, Timestamp ts, Timestamp te, RelationId **ids, RelationId &result);

    inline void scanPartition_CheckBoth_intersect_gOverlaps(unsigned int level, Timestamp t, RelationId **ids, vector<pair<Timestamp, Timestamp>> **timestamps, bool (*compare)(const pair<Timestamp, Timestamp>&, const pair<Timestamp, Timestamp>&), const RangeQuery &Q, RelationId &candidates, RelationId &result);
    inline void scanPartition_CheckEnd_intersect_gOverlaps(unsigned int level, Timestamp t, RelationId **ids, vector<pair<Timestamp, Timestamp>> **timestamps, const RangeQuery &Q, RelationId &candidates, RelationId &result);
    inline void scanPartition_CheckEnd_intersect_gOverlaps(unsigned int level, Timestamp t, RelationId **ids, vector<pair<Timestamp, Timestamp>> **timestamps, bool (*compare)(const pair<Timestamp, Timestamp>&, const pair<Timestamp, Timestamp>&), const RangeQuery &Q, RelationId &candidates, RelationId &result);
    inline void scanPartition_CheckStart_intersect_gOverlaps(unsigned int level, Timestamp t, RelationId **ids, vector<pair<Timestamp, Timestamp>> **timestamps, bool (*compare)(const pair<Timestamp, Timestamp>&, const pair<Timestamp, Timestamp>&), const RangeQuery &Q, RelationId &candidates, RelationId &result);
    inline void scanPartition_NoChecks_intersect_gOverlaps(unsigned int level, Timestamp t, RelationId **ids, RelationId &candidates, RelationId &result);
    inline void scanPartitions_NoChecks_intersect_gOverlaps(unsigned int level, Timestamp ts, Timestamp te, RelationId **ids, RelationId &candidates, RelationId &result);

public:
    // Construction
    HINT_M_SubsSort_CM(const Relation &R, const unsigned int numBits);
    HINT_M_SubsSort_CM(const Relation &R, const Relation &U, const unsigned int numBits);
    void getStats();
    size_t getSize();
    ~HINT_M_SubsSort_CM();
    
    // Updating
    void insert(const Record &r);

    // Querying
    void moveOut_gOverlaps(const RangeQuery &q, RelationId &candidates);
    void intersect_gOverlaps(const RangeQuery &q, RelationId &candidates);
    void intersectAndOutput_gOverlaps(const RangeQuery &q, RelationId &candidates, RelationId &result);
};



class HINT_M_SubsSortByRecordId_CM : public HierarchicalIndex
{
private:
    Relation      **pOrgsInTmp;
    Relation      **pOrgsAftTmp;
    Relation      **pRepsInTmp;
    Relation      **pRepsAftTmp;
    
    RelationId    **pOrgsInIds;
    RelationId    **pOrgsAftIds;
    RelationId    **pRepsInIds;
    RelationId    **pRepsAftIds;
    vector<pair<Timestamp, Timestamp> > **pOrgsInTimestamps;
    vector<pair<Timestamp, Timestamp> > **pOrgsAftTimestamps;
    vector<pair<Timestamp, Timestamp> > **pRepsInTimestamps;
    vector<pair<Timestamp, Timestamp> > **pRepsAftTimestamps;
    
    RecordId      **pOrgsIn_sizes, **pOrgsAft_sizes;
    size_t        **pRepsIn_sizes, **pRepsAft_sizes;
    
    // Construction
    inline void updateCounters(const Record &r);
    inline void updatePartitions(const Record &r);
    
    // Querying
    // Auxiliary functions to scan a partition.
    inline void scanPartition_CheckBoth_moveOut_gOverlaps(unsigned int level, Timestamp t, RelationId **ids, vector<pair<Timestamp, Timestamp>> **timestamps, const RangeQuery &Q, RelationId &result);
    inline void scanPartition_CheckEnd_moveOut_gOverlaps(unsigned int level, Timestamp t, RelationId **ids, vector<pair<Timestamp, Timestamp>> **timestamps, const RangeQuery &Q, RelationId &result);
    inline void scanPartition_CheckStart_moveOut_gOverlaps(unsigned int level, Timestamp t, RelationId **ids, vector<pair<Timestamp, Timestamp>> **timestamps, const RangeQuery &Q, RelationId &result);
    inline void scanPartition_NoChecks_moveOut_gOverlaps(unsigned int level, Timestamp t, RelationId **ids, RelationId &result);
    inline void scanPartitions_NoChecks_moveOut_gOverlaps(unsigned int level, Timestamp ts, Timestamp te, RelationId **ids, RelationId &result);
    inline void scanPartition_NoChecks_intersect_gOverlaps(unsigned int level, Timestamp t, RelationId **ids, RelationId &candidates, RelationId &result);
    //    inline void scanPartitions_NoChecks_intersect_gOverlaps(unsigned int level, Timestamp ts, Timestamp te, RelationId **ids, RelationId &candidates, RelationId &result);
    
    inline void scanPartition_CheckBoth_moveOut_gOverlaps(unsigned int level, Timestamp t, RelationId **ids, vector<pair<Timestamp, Timestamp>> **timestamps, const RangeQuery &q, vector<RelationId> &vec_result);
    inline void scanPartition_CheckEnd_moveOut_gOverlaps(unsigned int level, Timestamp t, RelationId **ids, vector<pair<Timestamp, Timestamp>> **timestamps, const RangeQuery &q, vector<RelationId> &vec_result);
    inline void scanPartition_CheckStart_moveOut_gOverlaps(unsigned int level, Timestamp t, RelationId **ids, vector<pair<Timestamp, Timestamp>> **timestamps, const RangeQuery &q, vector<RelationId> &vec_result);
    inline void scanPartition_NoChecks_moveOut_gOverlaps(unsigned int level, Timestamp t, RelationId **ids, vector<RelationId> &vec_result);
    inline void scanPartitions_NoChecks_moveOut_gOverlaps(unsigned int level, Timestamp ts, Timestamp te, RelationId **ids, vector<RelationId> &vec_result);
    inline void scanPartition_NoChecks_intersect_gOverlaps(unsigned int level, Timestamp t, RelationId **ids, RelationId &candidates, vector<RelationId> &vec_result);
//    inline void scanPartition_CheckBoth_intersect_gOverlaps(unsigned int level, Timestamp t, RelationId **ids, vector<pair<Timestamp, Timestamp>> **timestamps, const RangeQuery &Q, RelationId &candidates, RelationId &result);
//    inline void scanPartition_CheckEnd_intersect_gOverlaps(unsigned int level, Timestamp t, RelationId **ids, vector<pair<Timestamp, Timestamp>> **timestamps, const RangeQuery &Q, RelationId &candidates, RelationId &result);
//    inline void scanPartition_CheckStart_intersect_gOverlaps(unsigned int level, Timestamp t, RelationId **ids, vector<pair<Timestamp, Timestamp>> **timestamps, const RangeQuery &Q, RelationId &candidates, RelationId &result);
//    inline void scanPartition_NoChecks_intersect_gOverlaps(unsigned int level, Timestamp t, RelationId **ids, RelationId &candidates, RelationId &result);
//    inline void scanPartitions_NoChecks_intersect_gOverlaps(unsigned int level, Timestamp ts, Timestamp te, RelationId **ids, RelationId &candidates, RelationId &result);
    
public:
    // Construction
    HINT_M_SubsSortByRecordId_CM(const Relation &R, const unsigned int numBits);
    HINT_M_SubsSortByRecordId_CM(const Relation &R, const Relation &U, const unsigned int numBits);
    void getStats();
    size_t getSize();
    ~HINT_M_SubsSortByRecordId_CM();
    
    // Updating
    void insert(const Record &r);

    // Querying
    void moveOut_gOverlaps(const RangeQuery &q, RelationId &candidates);
    void intersect_gOverlaps(const RangeQuery &q, RelationId &candidates);
    void intersectAndOutput_gOverlaps(const RangeQuery &q, RelationId &candidates, RelationId &result);
    
    void moveOut_gOverlaps(const RangeQuery &q, vector<RelationId> &candidates);
    void moveOut_NoChecks_gOverlaps(const RangeQuery &q, vector<RelationId> &candidates);
    void intersect_gOverlaps(const RangeQuery &q, RelationId &candidates, vector<RelationId> &vec_candidates);
};



// HINT^m with subs+sort, skewness & sparsity optimizations and cash misses activated, from VLDB Journal
class HINT_M_SubsSort_SS_CM : public HierarchicalIndex
{
private:
    // Temporary structures
    Relation      *pOrgsInTmp;
    Relation      *pOrgsAftTmp;
    Relation      *pRepsInTmp;
    Relation      *pRepsAftTmp;
    
    // Index structure
    RelationId    *pOrgsInIds;
    vector<pair<Timestamp, Timestamp> > *pOrgsInTimestamps;
    RelationId    *pOrgsAftIds;
    vector<pair<Timestamp, Timestamp> > *pOrgsAftTimestamps;
    RelationId    *pRepsInIds;
    vector<pair<Timestamp, Timestamp> > *pRepsInTimestamps;
    RelationId    *pRepsAftIds;
    vector<pair<Timestamp, Timestamp> > *pRepsAftTimestamps;
    
    RecordId      **pOrgsIn_sizes, **pOrgsAft_sizes;
    size_t        **pRepsIn_sizes, **pRepsAft_sizes;
    RecordId      **pOrgsIn_offsets, **pOrgsAft_offsets;
    size_t        **pRepsIn_offsets, **pRepsAft_offsets;
    Offsets_SS_CM *pOrgsIn_ioffsets;
    Offsets_SS_CM *pOrgsAft_ioffsets;
    Offsets_SS_CM *pRepsIn_ioffsets;
    Offsets_SS_CM *pRepsAft_ioffsets;
    
    
    // Construction
    inline void updateCounters(const Record &r);
    inline void updatePartitions(const Record &r);
    
    // Querying
    // Auxiliary functions to determine exactly how to scan a partition.
    inline bool getBounds(unsigned int level, Timestamp t, PartitionId &next_from, Offsets_SS_CM *ioffsets, vector<pair<Timestamp, Timestamp> > *timestamps, vector<pair<Timestamp, Timestamp> >::iterator &iterStart, vector<pair<Timestamp, Timestamp> >::iterator &iterEnd, RelationIdIterator &iterI);
    inline bool getBounds(unsigned int level, Timestamp t, PartitionId &next_from, Offsets_SS_CM *ioffsets, RelationId *ids, RelationIdIterator &iterIStart, RelationIdIterator &iterIEnd);
    inline bool getBounds(unsigned int level, Timestamp ts, Timestamp te, PartitionId &next_from, PartitionId &next_to, Offsets_SS_CM *ioffsets, RelationId *ids, RelationIdIterator &iterIStart, RelationIdIterator &iterIEnd);
    
    // Auxiliary functions to scan a partition.
    inline void scanPartition_CheckBoth_moveOut_gOverlaps(const unsigned int level, const Timestamp t, Offsets_SS_CM *ioffsets, vector<pair<Timestamp, Timestamp> > *timestamps, bool (*compare)(const pair<Timestamp, Timestamp>&, const pair<Timestamp, Timestamp>&), const RangeQuery &q, PartitionId &next_from, RelationId &result);
    inline void scanPartition_CheckEnd_moveOut_gOverlaps(const unsigned int level, const Timestamp t, Offsets_SS_CM *ioffsets, vector<pair<Timestamp, Timestamp> > *timestamps, const RangeQuery &q, PartitionId &next_from, RelationId &result);
    inline void scanPartition_CheckEnd_moveOut_gOverlaps(const unsigned int level, Timestamp t, Offsets_SS_CM *ioffsets, vector<pair<Timestamp, Timestamp> > *timestamps, bool (*compare)(const pair<Timestamp, Timestamp>&, const pair<Timestamp, Timestamp>&), const RangeQuery &q, PartitionId &next_from, RelationId &result);
    inline void scanPartition_CheckStart_moveOut_gOverlaps(const unsigned int level, const Timestamp t, Offsets_SS_CM *ioffsets, vector<pair<Timestamp, Timestamp> > *timestamps, bool (*compare)(const pair<Timestamp, Timestamp>&, const pair<Timestamp, Timestamp>&), const RangeQuery &q, PartitionId &next_from, RelationId &result);
    inline void scanPartition_NoChecks_moveOut_gOverlaps(const unsigned int level, const Timestamp t, Offsets_SS_CM *ioffsets, RelationId *ids, PartitionId &next_from, RelationId &result);
    inline void scanPartitions_NoChecks_moveOut_gOverlaps(const unsigned int level, const Timestamp ts, Timestamp te, Offsets_SS_CM *ioffsets, RelationId *ids, PartitionId &next_from, PartitionId &next_to, RelationId &result);

    inline void scanPartition_CheckBoth_intersect_gOverlaps(const unsigned int level, const Timestamp t, Offsets_SS_CM *ioffsets, vector<pair<Timestamp, Timestamp> > *timestamps, bool (*compare)(const pair<Timestamp, Timestamp>&, const pair<Timestamp, Timestamp>&), const RangeQuery &q, PartitionId &next_from, RelationId &candidates, RelationId &result);
    inline void scanPartition_CheckEnd_intersect_gOverlaps(const unsigned int level, const Timestamp t, Offsets_SS_CM *ioffsets, vector<pair<Timestamp, Timestamp> > *timestamps, const RangeQuery &q, PartitionId &next_from, RelationId &candidates, RelationId &result);
    inline void scanPartition_CheckEnd_intersect_gOverlaps(const unsigned int level, Timestamp t, Offsets_SS_CM *ioffsets, vector<pair<Timestamp, Timestamp> > *timestamps, bool (*compare)(const pair<Timestamp, Timestamp>&, const pair<Timestamp, Timestamp>&), const RangeQuery &q, PartitionId &next_from, RelationId &candidates, RelationId &result);
    inline void scanPartition_CheckStart_intersect_gOverlaps(const unsigned int level, const Timestamp t, Offsets_SS_CM *ioffsets, vector<pair<Timestamp, Timestamp> > *timestamps, bool (*compare)(const pair<Timestamp, Timestamp>&, const pair<Timestamp, Timestamp>&), const RangeQuery &q, PartitionId &next_from, RelationId &candidates, RelationId &result);
    inline void scanPartition_NoChecks_intersect_gOverlaps(const unsigned int level, const Timestamp t, Offsets_SS_CM *ioffsets, RelationId *ids, PartitionId &next_from, RelationId &candidates, RelationId &result);
    inline void scanPartitions_NoChecks_intersect_gOverlaps(const unsigned int level, const Timestamp ts, Timestamp te, Offsets_SS_CM *ioffsets, RelationId *ids, PartitionId &next_from, PartitionId &next_to, RelationId &candidates, RelationId &result);

public:
    // Construction
    HINT_M_SubsSort_SS_CM(const Relation &R, const unsigned int numBits);
    void getStats();
    size_t getSize();
    ~HINT_M_SubsSort_SS_CM();
    
    // Querying
    void moveOut_gOverlaps(const RangeQuery &q, RelationId &candidates);
    void intersect_gOverlaps(const RangeQuery &q, RelationId &candidates);
    void intersectAndOutput_gOverlaps(const RangeQuery &q, RelationId &candidates, RelationId &result);
};



class HINT_M_SubsSortByRecordId_SS_CM : public HierarchicalIndex
{
private:
    // Temporary structures
    Relation      *pOrgsInTmp;
    Relation      *pOrgsAftTmp;
    Relation      *pRepsInTmp;
    Relation      *pRepsAftTmp;
    
    // Index structure
    RelationId    *pOrgsInIds;
    vector<pair<Timestamp, Timestamp> > *pOrgsInTimestamps;
    RelationId    *pOrgsAftIds;
    vector<pair<Timestamp, Timestamp> > *pOrgsAftTimestamps;
    RelationId    *pRepsInIds;
    vector<pair<Timestamp, Timestamp> > *pRepsInTimestamps;
    RelationId    *pRepsAftIds;
    vector<pair<Timestamp, Timestamp> > *pRepsAftTimestamps;
    
    RecordId      **pOrgsIn_sizes, **pOrgsAft_sizes;
    size_t        **pRepsIn_sizes, **pRepsAft_sizes;
    RecordId      **pOrgsIn_offsets, **pOrgsAft_offsets;
    size_t        **pRepsIn_offsets, **pRepsAft_offsets;
    Offsets_SS_CM *pOrgsIn_ioffsets;
    Offsets_SS_CM *pOrgsAft_ioffsets;
    Offsets_SS_CM *pRepsIn_ioffsets;
    Offsets_SS_CM *pRepsAft_ioffsets;
    
    
    // Construction
    inline void updateCounters(const Record &r);
    inline void updatePartitions(const Record &r);
    
    // Querying
    // Auxiliary functions to determine exactly how to scan a partition.
    inline bool getBounds(unsigned int level, Timestamp t, PartitionId &next_from, Offsets_SS_CM *ioffsets, vector<pair<Timestamp, Timestamp> > *timestamps, vector<pair<Timestamp, Timestamp> >::iterator &iterStart, vector<pair<Timestamp, Timestamp> >::iterator &iterEnd, RelationIdIterator &iterI);
    inline bool getBounds(unsigned int level, Timestamp t, PartitionId &next_from, Offsets_SS_CM *ioffsets, RelationId *ids, RelationIdIterator &iterIStart, RelationIdIterator &iterIEnd);
    inline bool getBounds(unsigned int level, Timestamp ts, Timestamp te, PartitionId &next_from, PartitionId &next_to, Offsets_SS_CM *ioffsets, RelationId *ids, RelationIdIterator &iterIStart, RelationIdIterator &iterIEnd);
    
    // Auxiliary functions to scan a partition.
    inline void scanPartition_CheckBoth_moveOut_gOverlaps(const unsigned int level, const Timestamp t, Offsets_SS_CM *ioffsets, vector<pair<Timestamp, Timestamp> > *timestamps, const RangeQuery &q, PartitionId &next_from, RelationId &result);
    inline void scanPartition_CheckEnd_moveOut_gOverlaps(const unsigned int level, const Timestamp t, Offsets_SS_CM *ioffsets, vector<pair<Timestamp, Timestamp> > *timestamps, const RangeQuery &q, PartitionId &next_from, RelationId &result);
    inline void scanPartition_CheckStart_moveOut_gOverlaps(const unsigned int level, const Timestamp t, Offsets_SS_CM *ioffsets, vector<pair<Timestamp, Timestamp> > *timestamps, const RangeQuery &q, PartitionId &next_from, RelationId &result);
    inline void scanPartition_NoChecks_moveOut_gOverlaps(const unsigned int level, const Timestamp t, Offsets_SS_CM *ioffsets, RelationId *ids, PartitionId &next_from, RelationId &result);
    inline void scanPartitions_NoChecks_moveOut_gOverlaps(const unsigned int level, const Timestamp ts, Timestamp te, Offsets_SS_CM *ioffsets, RelationId *ids, PartitionId &next_from, PartitionId &next_to, RelationId &result);
    inline void scanPartition_NoChecks_intersect_gOverlaps(const unsigned int level, const Timestamp t, Offsets_SS_CM *ioffsets, RelationId *ids, PartitionId &next_from, RelationId &candidates, RelationId &result);
    
    inline void scanPartition_CheckBoth_moveOut_gOverlaps(const unsigned int level, const Timestamp t, Offsets_SS_CM *ioffsets, vector<pair<Timestamp, Timestamp> > *timestamps, const RangeQuery &q, PartitionId &next_from, vector<RelationId> &vec_candidates);
    inline void scanPartition_CheckEnd_moveOut_gOverlaps(const unsigned int level, const Timestamp t, Offsets_SS_CM *ioffsets, vector<pair<Timestamp, Timestamp> > *timestamps, const RangeQuery &q, PartitionId &next_from, vector<RelationId> &vec_candidates);
    inline void scanPartition_CheckStart_moveOut_gOverlaps(const unsigned int level, const Timestamp t, Offsets_SS_CM *ioffsets, vector<pair<Timestamp, Timestamp> > *timestamps, const RangeQuery &q, PartitionId &next_from, vector<RelationId> &vec_candidates);
    inline void scanPartition_NoChecks_moveOut_gOverlaps(const unsigned int level, const Timestamp t, Offsets_SS_CM *ioffsets, RelationId *ids, PartitionId &next_from, vector<RelationId> &vec_candidates);
    inline void scanPartition_NoChecks_intersect_gOverlaps(const unsigned int level, const Timestamp t, Offsets_SS_CM *ioffsets, RelationId *ids, PartitionId &next_from, RelationId &candidates, vector<RelationId> &vect_result);
    inline void scanPartition_NoChecks_intersect_gOverlaps(const unsigned int level, const Timestamp t, Offsets_SS_CM *ioffsets, RelationId *ids, PartitionId &next_from, vector<RelationId> &vec_candidates, vector<RelationId> &vect_result);

    
    inline void scanPartition_NoChecks_intersect2_mway_gOverlaps(const unsigned int level, const Timestamp t, Offsets_SS_CM *ioffsets, RelationId *ids, PartitionId &next_from, vector<RelationId> &vect_partitions);

public:
    // Construction
    HINT_M_SubsSortByRecordId_SS_CM(const Relation &R, const unsigned int numBits);
    void getStats();
    size_t getSize();
    ~HINT_M_SubsSortByRecordId_SS_CM();
    
    // Querying
    void moveOut_gOverlaps(const RangeQuery &q, RelationId &candidates);
    void intersect_gOverlaps(const RangeQuery &q, RelationId &candidates);
    void intersectAndOutput_gOverlaps(const RangeQuery &q, RelationId &candidates, RelationId &result);
    
    void moveOut_gOverlaps(const RangeQuery &q, vector<RelationId> &candidates);
    void moveOut_NoChecks_gOverlaps(const RangeQuery &q, vector<RelationId> &candidates);
    void intersect_gOverlaps(const RangeQuery &q, RelationId &candidates, vector<RelationId> &vec_candidates);
    
    
    void intersect_mway_gOverlaps(const RangeQuery &q, vector<RelationId> &vec_candidates);
    void intersectAndOutput_mway_gOverlaps(const RangeQuery &q, vector<RelationId> &vec_candidates, vector<RelationId>  &vec_result);
    
    
    void intersect2_gOverlaps(const RangeQuery &q, RelationId &candidates);
    void intersect2AndOutput_gOverlaps(const RangeQuery &q, RelationId &candidates, RelationId &result);
};



// Comparators
inline bool CompareTimestampPairsByStart(const pair<Timestamp, Timestamp> &lhs, const pair<Timestamp, Timestamp> &rhs)
{
    return (lhs.first < rhs.first);
}

inline bool CompareTimestampPairsByEnd(const pair<Timestamp, Timestamp> &lhs, const pair<Timestamp, Timestamp> &rhs)
{
    return (lhs.second < rhs.second);
}
#endif // _HINT_M_H_

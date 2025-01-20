#ifndef _HINT_IF_H_
#define _HINT_IF_H_

#include "hierarchical_index.h"
#include "inverted_file.h"
#include "temporal_inverted_files.h"
#include "../containers/offsets.h"



// Base variant; every hint partition is stored as an inverted file
class irHINTa : public HierarchicalIRIndex
{
private:
    TemporalInvertedFile **pOrgsIn;
    TemporalInvertedFile **pOrgsAft;
    TemporalInvertedFile **pRepsIn;
    TemporalInvertedFile **pRepsAft;
    
    // Temporary counter
    TermId ***pOrgsIn_lsizes;
    TermId ***pOrgsAft_lsizes;
    TermId ***pRepsIn_lsizes;
    TermId ***pRepsAft_lsizes;
    
    // Construction
    inline void updateCounters(const IRecord &r);
    inline void updatePartitions(const IRecord &r);
    
    // Querying
    inline void scanPartitionContainment_CheckBoth_gOverlaps(TemporalInvertedFile &IF, const RangeIRQuery &q, RelationId &result);
    inline void scanPartitionContainment_CheckStart_gOverlaps(TemporalInvertedFile &IF, const RangeIRQuery &q, RelationId &result);
    inline void scanPartitionContainment_CheckEnd_gOverlaps(TemporalInvertedFile &IF, const RangeIRQuery &q, RelationId &result);
    inline void scanPartitionContainment_NoChecks_gOverlaps(TemporalInvertedFile &IF, const RangeIRQuery &q, RelationId &result);
    
public:
    // Construction
    irHINTa(const IRelation &R, const unsigned int numBits);
    irHINTa(const IRelation &R, const IRelation &U, const TermId dictionarySize, const unsigned int numBits);    // For updates only
    void getStats();
    size_t getSize();
    ~irHINTa();
    
    // Updating
    void insert(IRelation &U);
    
    // Querying
    void executeContainment(const RangeIRQuery &q, RelationId &result);
};



#ifdef USE_SS
class irHINTb : public HierarchicalIRIndex
{
private:
    Relation      *pOrgsInTmp;
    Relation      *pOrgsAftTmp;
    Relation      *pRepsInTmp;
    Relation      *pRepsAftTmp;
    
    RelationId    *pOrgsInIds;
    vector<pair<Timestamp, Timestamp> > *pOrgsInTimestamps;
    RelationId    *pOrgsAftIds;
    vector<pair<Timestamp, Timestamp> > *pOrgsAftTimestamps;
    RelationId    *pRepsInIds;
    vector<pair<Timestamp, Timestamp> > *pRepsInTimestamps;
    RelationId    *pRepsAftIds;
    vector<pair<Timestamp, Timestamp> > *pRepsAftTimestamps;
    
    RecordId      **pOrgsIn_sizes, **pOrgsAft_sizes;
    RecordId      **pRepsIn_sizes, **pRepsAft_sizes;
    RecordId      **pOrgsIn_offsets, **pOrgsAft_offsets;
    RecordId      **pRepsIn_offsets, **pRepsAft_offsets;
    Offsets_SS_CM *pOrgsIn_ioffsets;
    Offsets_SS_CM *pOrgsAft_ioffsets;
    Offsets_SS_CM *pRepsIn_ioffsets;
    Offsets_SS_CM *pRepsAft_ioffsets;

    InvertedFile **pOrgsIn;
    InvertedFile **pOrgsAft;
    InvertedFile **pRepsIn;
    InvertedFile **pRepsAft;

    
    // Construction
    inline void updateCounters(const IRecord &r);
    inline void updatePartitions(const IRecord &r);
    
    // Querying
    // Auxiliary functions to determine exactly how to scan a partition.
    inline bool getBounds(unsigned int level, Timestamp t, PartitionId &next_from, Offsets_SS_CM *ioffsets, vector<pair<Timestamp, Timestamp> > *timestamps, vector<pair<Timestamp, Timestamp> >::iterator &iterStart, vector<pair<Timestamp, Timestamp> >::iterator &iterEnd, RelationIdIterator &iterI);
    inline bool getBounds(unsigned int level, Timestamp t, PartitionId &next_from, Offsets_SS_CM *ioffsets, RelationId *ids, RelationIdIterator &iterIStart, RelationIdIterator &iterIEnd);
    inline bool getBounds(unsigned int level, Timestamp ts, Timestamp te, PartitionId &next_from, PartitionId &next_to, Offsets_SS_CM *ioffsets, vector<pair<Timestamp, Timestamp> > *timestamps, vector<pair<Timestamp, Timestamp> >::iterator &iterStart, vector<pair<Timestamp, Timestamp> >::iterator &iterEnd, RelationIdIterator &iterI);
    inline bool getBounds(unsigned int level, Timestamp ts, Timestamp te, PartitionId &next_from, PartitionId &next_to, Offsets_SS_CM *ioffsets, RelationId *ids, RelationIdIterator &iterIStart, RelationIdIterator &iterIEnd);
    
//    // Auxiliary functions to scan a partition.
    inline void scanPartition_CheckBoth_gOverlaps(const unsigned int level, const Timestamp t, Offsets_SS_CM *ioffsets, vector<pair<Timestamp, Timestamp> > *timestamps, InvertedFile **ifs, bool (*compare)(const pair<Timestamp, Timestamp>&, const pair<Timestamp, Timestamp>&), const RangeIRQuery &q, PartitionId &next_from, RelationId &result);
    inline void scanPartition_CheckStart_gOverlaps(const unsigned int level, const Timestamp t, Offsets_SS_CM *ioffsets, vector<pair<Timestamp, Timestamp> > *timestamps, InvertedFile **ifs, bool (*compare)(const pair<Timestamp, Timestamp>&, const pair<Timestamp, Timestamp>&), const RangeIRQuery &q, PartitionId &next_from, RelationId &result);
    inline void scanPartition_CheckEnd_gOverlaps(const unsigned int level, const Timestamp t, Offsets_SS_CM *ioffsets, vector<pair<Timestamp, Timestamp> > *timestamps, InvertedFile **ifs, const RangeIRQuery &q, PartitionId &next_from, RelationId &result);
    inline void scanPartition_CheckEnd_gOverlaps(const unsigned int level, const Timestamp t, Offsets_SS_CM *ioffsets, vector<pair<Timestamp, Timestamp> > *timestamps, InvertedFile **ifs, bool (*compare)(const pair<Timestamp, Timestamp>&, const pair<Timestamp, Timestamp>&), const RangeIRQuery &q, PartitionId &next_from, RelationId &result);
    inline void scanPartition_NoChecks_gOverlaps(const unsigned int level, const Timestamp t, Offsets_SS_CM *xioffsets, RelationId *ids, InvertedFile **ifs, const RangeIRQuery &q, PartitionId &next_from, RelationId &result);
//    inline void scanPartitions_NoChecks_gOverlaps(const unsigned int level, const Timestamp ts, Timestamp te, Offsets_SS_CM *ioffsets, RelationId *ids, InvertedFile **ifs, const RangeIRQuery &q, PartitionId &next_from, PartitionId &next_to, RelationId &result);

public:
    // Construction
    irHINTb(const IRelation &R, const unsigned int numBits);
    void getStats();
    size_t getSize();
    ~irHINTb();
    
    // Querying
    void executeContainment(const RangeIRQuery &q, RelationId &result);
};
#else
class irHINTb : public HierarchicalIRIndex
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
    RecordId      **pRepsIn_sizes, **pRepsAft_sizes;

    InvertedFile **pOrgsIn;
    InvertedFile **pOrgsAft;
    InvertedFile **pRepsIn;
    InvertedFile **pRepsAft;

    
    // Construction
    inline void updateCounters(const IRecord &r);
    inline void updatePartitions(const IRecord &r);
    
    // Querying
//    // Auxiliary functions to scan a partition.
    inline void scanPartition_CheckBoth_gOverlaps(const unsigned int level, const Timestamp t, RelationId **ids, vector<pair<Timestamp, Timestamp> > **timestamps, InvertedFile **ifs, bool (*compare)(const pair<Timestamp, Timestamp>&, const pair<Timestamp, Timestamp>&), const RangeIRQuery &q, RelationId &result);
    inline void scanPartition_CheckStart_gOverlaps(const unsigned int level, const Timestamp t, RelationId **ids, vector<pair<Timestamp, Timestamp> > **timestamps, InvertedFile **ifs, bool (*compare)(const pair<Timestamp, Timestamp>&, const pair<Timestamp, Timestamp>&), const RangeIRQuery &q, RelationId &result);
    inline void scanPartition_CheckEnd_gOverlaps(const unsigned int level, const Timestamp t, RelationId **ids, vector<pair<Timestamp, Timestamp> > **timestamps, InvertedFile **ifs, const RangeIRQuery &q, RelationId &result);
    inline void scanPartition_CheckEnd_gOverlaps(const unsigned int level, const Timestamp t, RelationId **ids, vector<pair<Timestamp, Timestamp> > **timestamps, InvertedFile **ifs, bool (*compare)(const pair<Timestamp, Timestamp>&, const pair<Timestamp, Timestamp>&), const RangeIRQuery &q, RelationId &result);
    inline void scanPartition_NoChecks_gOverlaps(const unsigned int level, const Timestamp t, RelationId **ids, InvertedFile **ifs, const RangeIRQuery &q, RelationId &result);
//    inline void scanPartitions_NoChecks_gOverlaps(const unsigned int level, const Timestamp ts, Timestamp te, Offsets_SS_CM *ioffsets, RelationId *ids, InvertedFile **ifs, const RangeIRQuery &q, PartitionId &next_from, PartitionId &next_to, RelationId &result);

public:
    // Construction
    irHINTb(const IRelation &R, const unsigned int numBits);
    irHINTb(const IRelation &R, const IRelation &U, const unsigned int numBits);    // For updates only
    void getStats();
    size_t getSize();
    ~irHINTb();
    
    // Updating
    void insert(IRelation &U);
    
    // Querying
    void executeContainment(const RangeIRQuery &q, RelationId &result);
};
#endif
#endif // _HINT_IF_H_

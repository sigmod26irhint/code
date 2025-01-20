#ifndef _TEMPORAL_INVERTED_FILES_H_
#define _TEMPORAL_INVERTED_FILES_H_

#include <unordered_map>
#include <queue>
#include "../containers/relations.h"
#include "../indices/inverted_file.h"
#include "../indices/1dgrid.h"
#include "../indices/sharded_postinglist.h"
#include "../indices/hint_m.h"
#include "../def_global.h"



class TemporalInvertedFile : public InvertedFileTemplate
{
private:
    // Indexed relation
    const IRelation *R;
        
    
    // Querying
    
public:
    // Posting lists
    unordered_map<TermId, Relation> lists;

    // Constructing
    TemporalInvertedFile();
    TemporalInvertedFile(const IRelation &R);
    TemporalInvertedFile(const IRelation &R, const IRelation &U, const TermId dictionarySize);   // For updates only
    void index(const IRecord &r);
    void getStats();
    size_t getSize();
    void print(char c);
    ~TemporalInvertedFile();
    
    // Updating
    void insert(IRelation &U);

    // Querying
    bool moveOut_NoChecks_gOverlaps(const RangeIRQuery &q, RelationId &candidates);
    bool moveOut_CheckBoth_gOverlaps(const RangeIRQuery &q, RelationId &candidates);
    bool moveOut_CheckStart_gOverlaps(const RangeIRQuery &q, RelationId &candidates);
    bool moveOut_CheckEnd_gOverlaps(const RangeIRQuery &q, RelationId &candidates);
    bool intersect_gOverlaps(const RangeIRQuery &q, const unsigned int termoff, RelationId &candidates);
    bool intersectAndOutput_gOverlaps(const RangeIRQuery &q, const unsigned int off, RelationId &candidates, RelationId &result);

    void executeContainment(const RangeIRQuery &q, RelationId &result);
};



class TemporalInvertedFile_Slicing : public InvertedFileTemplate
{
private:
    // Indexed relation
    const IRelation *R;

    // Posting lists
    unordered_map<TermId, OneDimensionalGrid*> lists;
    
public:
    // Constructing
    TemporalInvertedFile_Slicing();
    TemporalInvertedFile_Slicing(const IRelation &R, const PartitionId numPartitions);
    TemporalInvertedFile_Slicing(const IRelation &R, const IRelation &U, const TermId dictionarySize, const PartitionId numPartitions);    // For updates only
    void getStats();
    size_t getSize();
    void print(char c);
    ~TemporalInvertedFile_Slicing();

    // Updating
    void insert(IRelation &U);

    // Querying
    bool moveOut_gOverlaps_checkBoth(const RangeIRQuery &q, RelationId &candidates);
    bool intersect_gOverlaps(const RangeIRQuery &q, const unsigned int termoff, RelationId &candidates);
    bool intersectAndOutput_gOverlaps(const RangeIRQuery &q, const unsigned int off, RelationId &candidates, RelationId &result);
    
    void executeContainment(const RangeIRQuery &q, RelationId &result);
};



class TemporalInvertedFile_Sharding : public InvertedFileTemplate
{
private:
    // Indexed relation
//    const IRelation *R;

    // Posting lists
    Timestamp tolerance;
    unsigned int impactListGap;
    unordered_map<TermId, ShardedPostingList*> lists;
    
public:
    // Constructing
    TemporalInvertedFile_Sharding();
    TemporalInvertedFile_Sharding(IRelation &R, const unsigned int impactListGap, const float relaxation);
    TemporalInvertedFile_Sharding(IRelation &R, IRelation &U, const unsigned int impactListGap, const float relaxation);
    void getStats();
    size_t getSize();
    void print(char c);
    ~TemporalInvertedFile_Sharding();
    
    // Updating
    void insert(IRelation &U);

    // Querying
    void moveOut(const TermId eid, RelationId &candidates, const Timestamp& qstart, const Timestamp& qend);
    void intersect(const TermId eid, RelationId &candidates, const Timestamp& qstart, const Timestamp& qend);

    void executeContainment(const RangeIRQuery &q, RelationId &result);
};



class TemporalInvertedFile_HINTAlpha : public InvertedFileTemplate
{
private:
    // Indexed relation
    const IRelation *R;

    // Posting lists
    unordered_map<TermId, HINT_M_SubsSort_SS_CM*> lists;
    
    // Querying
    bool moveOut_gOverlaps(const RangeIRQuery &q, RelationId &candidates);
    bool verify_gOverlaps(const RangeIRQuery &q, const unsigned int termoff, RelationId &candidates, RelationId &result);

public:
    // Constructing
    TemporalInvertedFile_HINTAlpha();
    TemporalInvertedFile_HINTAlpha(const IRelation &R, const unsigned int numBits);
    void getStats();
    size_t getSize();
    void print(char c);
    ~TemporalInvertedFile_HINTAlpha();
    
    // Updating
//    void insert(IRelation &U);

    // Querying
    void executeContainment(const RangeIRQuery &q, RelationId &result);
};



class TemporalInvertedFile_HINTBeta : public InvertedFileTemplate
{
private:
    // Indexed relation
    const IRelation *R;

    // Posting lists
#ifdef USE_SS
    unordered_map<TermId, HINT_M_SubsSort_SS_CM*> hlists;
#else
    unordered_map<TermId, HINT_M_SubsSort_CM*> hlists;
#endif
    
    // Querying
    bool moveOut_gOverlaps(const RangeIRQuery &q, RelationId &candidates);
    bool intersect_gOverlaps(const RangeIRQuery &q, const unsigned int termoff, RelationId &candidates);
    bool intersectAndOutput_gOverlaps(const RangeIRQuery &q, const unsigned int off, RelationId &candidates, RelationId &result);
//    bool verify_gOverlaps(const RangeIRQuery &q, const unsigned int off, RelationId &candidates, RelationId &result);

public:
    // Constructing
    TemporalInvertedFile_HINTBeta();
    TemporalInvertedFile_HINTBeta(const IRelation &R, const unsigned int numBits);
    TemporalInvertedFile_HINTBeta(const IRelation &R, const IRelation &U, const TermId dictionarySize, const unsigned int numBits);
    void getStats();
    size_t getSize();
    void print(char c);
    ~TemporalInvertedFile_HINTBeta();

    // Updating
    void insert(IRelation &U);

    // Querying
    void executeContainment(const RangeIRQuery &q, RelationId &result);
};



class TemporalInvertedFile_HINTGamma : public InvertedFileTemplate
{
private:
    // Indexed relation
    const IRelation *R;

    // Posting lists
#ifdef USE_SS
    unordered_map<TermId, HINT_M_SubsSortByRecordId_SS_CM*> hlists;
#else
    unordered_map<TermId, HINT_M_SubsSortByRecordId_CM*> hlists;
#endif
    
    // Querying
    bool moveOut_gOverlaps(const RangeIRQuery &q, RelationId &candidates);
    bool intersect_gOverlaps(const RangeIRQuery &q, const unsigned int termoff, RelationId &candidates);
    bool intersectAndOutput_gOverlaps(const RangeIRQuery &q, const unsigned int off, RelationId &candidates, RelationId &result);

    bool moveOut_gOverlaps(const RangeIRQuery &q, vector<RelationId> &vec_candidates);
    bool moveOut_NoChecks_gOverlaps(const RangeIRQuery &q, const unsigned int termoff, vector<RelationId> &vec_candidates);
    bool intersect_gOverlaps(const RangeIRQuery &q, const unsigned int termoff, RelationId &candidates, vector<RelationId> &vec_candidates);
//    bool intersect_mway_gOverlaps(const RangeIRQuery &q, const unsigned int termoff, vector<RelationId> &vec_candidates);
//    bool intersectAndOutput_mway_gOverlaps(const RangeIRQuery &q, const unsigned int termoff, vector<RelationId> &vec_candidates, vector<RelationId> &vec_result);
//    bool verify_gOverlaps(const RangeIRQuery &q, const unsigned int off, RelationId &candidates, RelationId &result);
    
    
//    bool intersect2_gOverlaps(const RangeIRQuery &q, const unsigned int termoff, RelationId &candidates);
//    bool intersect2AndOutput_mway_gOverlaps(const RangeIRQuery &q, const unsigned int termoff, RelationId &candidates, RelationId &result);

public:
    // Constructing
    TemporalInvertedFile_HINTGamma();
    TemporalInvertedFile_HINTGamma(const IRelation &R, const unsigned int numBits);
    TemporalInvertedFile_HINTGamma(const IRelation &R, const IRelation &U, const TermId dictionarySize, const unsigned int numBits);
    void getStats();
    size_t getSize();
    void print(char c);
    ~TemporalInvertedFile_HINTGamma();

    // Updating
    void insert(IRelation &U);

    // Querying
    void executeContainment(const RangeIRQuery &q, RelationId &result);
//    void executeContainment(const RangeIRQuery &q, vector<RelationId> &vec_result);
};



class TemporalInvertedFile_HINTGammaH : public InvertedFileTemplate
{
private:
    // Indexed relation
    const IRelation *R;

    // Posting lists
    TemporalInvertedFile *tif;
    unordered_map<TermId, HINT_M_SubsSortByRecordId_SS_CM*> hints;
    
    // Querying
    bool moveOut_gOverlaps(const RangeIRQuery &q, RelationId &candidates);
    bool intersect_gOverlaps(const RangeIRQuery &q, const unsigned int termoff, RelationId &candidates);
    bool intersectAndOutput_gOverlaps(const RangeIRQuery &q, const unsigned int off, RelationId &candidates, RelationId &result);

    bool moveOut_gOverlaps(const RangeIRQuery &q, vector<RelationId> &vec_candidates);
    bool moveOut_NoChecks_gOverlaps(const RangeIRQuery &q, const unsigned int termoff, vector<RelationId> &vec_candidates);
    bool intersect_gOverlaps(const RangeIRQuery &q, const unsigned int termoff, RelationId &candidates, vector<RelationId> &vec_candidates);
    bool intersect_mway_gOverlaps(const RangeIRQuery &q, const unsigned int termoff, vector<RelationId> &vec_candidates);
    bool intersectAndOutput_mway_gOverlaps(const RangeIRQuery &q, const unsigned int termoff, vector<RelationId> &vec_candidates, vector<RelationId> &vec_result);
    bool verify_gOverlaps(const RangeIRQuery &q, const unsigned int off, RelationId &candidates, RelationId &result);
    
    
    bool intersect2_gOverlaps(const RangeIRQuery &q, const unsigned int termoff, RelationId &candidates);
    bool intersect2AndOutput_mway_gOverlaps(const RangeIRQuery &q, const unsigned int termoff, RelationId &candidates, RelationId &result);

public:
    // Constructing
    TemporalInvertedFile_HINTGammaH();
    TemporalInvertedFile_HINTGammaH(const IRelation &R, const unsigned int numBits);
    void getStats();
    size_t getSize();
    void print(char c);
    ~TemporalInvertedFile_HINTGammaH();

    // Updating
//    void insert(IRelation &U);

    // Querying
    void executeContainment(const RangeIRQuery &q, RelationId &result);
    void executeContainment(const RangeIRQuery &q, vector<RelationId> &vec_result);
};



class TemporalInvertedFile_HINTSlicing : public InvertedFileTemplate
{
private:
    // Indexed relation
    const IRelation *R;

    // Posting lists
//    unordered_map<TermId, OneDimensionalGrid*>              glists;
    unordered_map<TermId, OneDimensionalGrid_RecordStart*>  glists;
#ifdef USE_SS
    unordered_map<TermId, HINT_M_SubsSortByRecordId_SS_CM*> hlists;
#else
    unordered_map<TermId, HINT_M_SubsSortByRecordId_CM*>    hlists;
#endif
    
    // Querying
    bool moveOut_gOverlaps(const RangeIRQuery &q, RelationId &candidates);
    bool intersect_gOverlaps(const RangeIRQuery &q, const unsigned int termoff, RelationId &candidates);
    bool intersectAndOutput_gOverlaps(const RangeIRQuery &q, const unsigned int off, RelationId &candidates, RelationId &result);

    bool moveOut_gOverlaps(const RangeIRQuery &q, vector<RelationId> &vec_candidates);

public:
    // Constructing
    TemporalInvertedFile_HINTSlicing();
    TemporalInvertedFile_HINTSlicing(const IRelation &R, const unsigned numPartitions, const unsigned int numBits);
    TemporalInvertedFile_HINTSlicing(const IRelation &R, const IRelation &U, const TermId dictionarySize, const unsigned numPartitions, const unsigned int numBits);
    void getStats();
    size_t getSize();
    void print(char c);
    ~TemporalInvertedFile_HINTSlicing();

    // Updating
    void insert(IRelation &U);

    // Querying
    void executeContainment(const RangeIRQuery &q, RelationId &result);
};



class TemporalInvertedFile_HINTDelta : public InvertedFileTemplate
{
private:
    // Indexed relation
    const IRelation *R;

    InvertedFile *iidx;
    unordered_map<TermId, HINT_M_SubsSort_SS_CM*> hidxs;
    
    // Querying
    bool moveOut_gOverlaps(const RangeIRQuery &q, RelationId &candidates);
    bool intersect_gOverlaps(const RangeIRQuery &q, const unsigned int termoff, RelationId &candidates);
    void intersectAndOutput_gOverlaps(const RangeIRQuery &q, const unsigned int off, RelationId &candidates, RelationId &result);

public:
    // Constructing
    TemporalInvertedFile_HINTDelta();
    TemporalInvertedFile_HINTDelta(const IRelation &R, const unsigned int numBits);
    void getStats();
    size_t getSize();
    void print(char c);
    ~TemporalInvertedFile_HINTDelta();

    // Updating
//    void insert(IRelation &U);

    // Querying
    void executeContainment(const RangeIRQuery &q, RelationId &result);
};
#endif // _TEMPORAL_INVERTED_FILES_H_

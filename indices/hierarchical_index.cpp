#include "hierarchical_index.h"



// Import
unsigned int determineOptimalNumBitsForHINT_M(const RecordId numRecords, const Timestamp domainSize, Timestamp avgRecordExtent, const float qe_precentage);


// HierarchicalIndex class
HierarchicalIndex::HierarchicalIndex(const Relation &R, const unsigned int numBits = 0)
{
    // Set indexed relation
    this->R = &R;
    
    // Set bits and height
    this->gstart = R.gstart;
    this->gend = R.gend;
    this->numBits            = ((numBits != 0)? numBits: determineOptimalNumBitsForHINT_M(R.size(), R.domainSize, R.avgRecordExtent, 0.1));    // Using SIGMOD'22 model for m
    this->maxBits            = int(log2(this->gend-this->gstart))+1;
    this->height             = this->numBits+1;
    this->numBits_cutoff     = this->numBits;

    // Initialize statistics
    this->numPartitions      = 0;
    this->numEmptyPartitions = 0;
    this->avgPartitionSize   = 0;
    this->numOriginals       = 0;
    this->numReplicas        = 0;
    this->numOriginalsIn     = 0;
    this->numOriginalsAft    = 0;
    this->numReplicasIn      = 0;
    this->numReplicasAft     = 0;
}


HierarchicalIndex::HierarchicalIndex(const Relation &R, const Relation &U, const unsigned int numBits = 0)
{
    // Set indexed relation
    this->R = &R;

    // Set bits and height
    this->gstart = min(R.gstart, U.gstart);
    this->gend = max(R.gend, U.gend);
    this->numBits            = ((numBits != 0)? numBits: determineOptimalNumBitsForHINT_M(R.size(), R.domainSize, R.avgRecordExtent, 0.1));    // Using SIGMOD'22 model for m
    this->maxBits            = int(log2(this->gend-this->gstart))+1;
    this->height             = this->numBits+1;
    this->numBits_cutoff     = this->numBits;

    // Initialize statistics
    this->numPartitions      = 0;
    this->numEmptyPartitions = 0;
    this->avgPartitionSize   = 0;
    this->numOriginals       = 0;
    this->numReplicas        = 0;
    this->numOriginalsIn     = 0;
    this->numOriginalsAft    = 0;
    this->numReplicasIn      = 0;
    this->numReplicasAft     = 0;
}



// HierarchicalIRIndex class
HierarchicalIRIndex::HierarchicalIRIndex(const IRelation &R, const unsigned int numBits = 0)
{
    // Set indexed relation
    this->R = &R;

    // Set bits and height
    this->numBits            = ((numBits != 0)? numBits: determineOptimalNumBitsForHINT_M(R.size(), R.domainSize, R.avgRecordExtent, 0.1));    // Using SIGMOD'22 model for m
    this->maxBits            = int(log2(R.gend-R.gstart))+1;
    this->height             = this->numBits+1;

    // Initialize statistics
    this->numPartitions      = 0;
    this->numEmptyPartitions = 0;
    this->avgPartitionSize   = 0;
    this->numOriginals       = 0;
    this->numReplicas        = 0;
    this->numOriginalsIn     = 0;
    this->numOriginalsAft    = 0;
    this->numReplicasIn      = 0;
    this->numReplicasAft     = 0;
}


HierarchicalIRIndex::HierarchicalIRIndex(const IRelation &R, const IRelation &U, const unsigned int numBits = 0)
{
    // Set indexed relation
    this->R = &R;

    // Set bits and height
    this->gstart = min(R.gstart, U.gstart);
    this->gend = max(R.gend, U.gend);
    this->numBits            = ((numBits != 0)? numBits: determineOptimalNumBitsForHINT_M(R.size(), R.domainSize, R.avgRecordExtent, 0.1));    // Using SIGMOD'22 model for m
    this->maxBits            = int(log2(this->gend-this->gstart))+1;
    this->height             = this->numBits+1;

    // Initialize statistics
    this->numPartitions      = 0;
    this->numEmptyPartitions = 0;
    this->avgPartitionSize   = 0;
    this->numOriginals       = 0;
    this->numReplicas        = 0;
    this->numOriginalsIn     = 0;
    this->numOriginalsAft    = 0;
    this->numReplicasIn      = 0;
    this->numReplicasAft     = 0;
}

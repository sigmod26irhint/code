#ifndef _OFFSETS_H_
#define _OFFSETS_H_

#include "relations.h"



class OffsetEntry_SS_CM
{
public:
    Timestamp tstamp;
    RelationIdIterator iterI;
    vector<pair<Timestamp, Timestamp> >::iterator iterT;
    PartitionId pid;
    
    OffsetEntry_SS_CM();
    OffsetEntry_SS_CM(Timestamp tstamp, RelationIdIterator iterI, vector<pair<Timestamp, Timestamp> >::iterator iterT, PartitionId pid);
    bool operator < (const OffsetEntry_SS_CM &rhs) const;
    bool operator >= (const OffsetEntry_SS_CM &rhs) const;
    ~OffsetEntry_SS_CM();
};

typedef vector<OffsetEntry_SS_CM> Offsets_SS_CM;
typedef Offsets_SS_CM::const_iterator Offsets_SS_CM_Iterator;
#endif //_OFFSETS_H_

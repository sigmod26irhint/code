#include "offsets.h"



OffsetEntry_SS_CM::OffsetEntry_SS_CM()
{
}
    

OffsetEntry_SS_CM::OffsetEntry_SS_CM(Timestamp tstamp, RelationIdIterator iterI, vector<pair<Timestamp, Timestamp> >::iterator iterT, PartitionId pid)
{
    this->tstamp = tstamp;
    this->iterI  = iterI;
    this->iterT  = iterT;
    this->pid    = pid;
}


bool OffsetEntry_SS_CM::operator < (const OffsetEntry_SS_CM &rhs) const
{
    return this->tstamp < rhs.tstamp;
}


bool OffsetEntry_SS_CM::operator >= (const OffsetEntry_SS_CM &rhs) const
{
    return this->tstamp >= rhs.tstamp;
}


OffsetEntry_SS_CM::~OffsetEntry_SS_CM()
{
}

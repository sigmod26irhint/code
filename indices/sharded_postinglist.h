#ifndef _SHARDEDPOSTINGLIST_H_
#define _SHARDEDPOSTINGLIST_H_


#include <map>
#include <string>

#include "../containers/relations.h"
#include "../def_global.h"


// Stores the offsets in a shard
typedef map<Timestamp, size_t> ImpactList;


class Shard : public vector<Record>
{
public:

	Timestamp sstart = 0;
	Timestamp sstart_last = 0;
	Timestamp send = 0;
	ImpactList impactList;

	void getCandidates(
		const Timestamp& qstart, 
		const Timestamp& qend, 
		RelationId& candidates) const;

	size_t getSize();

	void print() const;
};


class ShardedPostingList : public vector<Shard>
{
public:

	void addRecord(
		const RecordId id,
		const Timestamp start,
		const Timestamp end,
		const RecordId impact_list_gap,
		const Timestamp relaxation);

	void getCandidates(
		const Timestamp& qstart, 
		const Timestamp& qend, 
		RelationId& candidates) const;

	size_t getSize();

	void print() const;
};



#endif //_SHARDEDPOSTINGLIST_H_
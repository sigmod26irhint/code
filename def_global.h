#pragma once
#ifndef _GLOBAL_DEF_H_
#define _GLOBAL_DEF_H_

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <iostream>
#include <limits>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <chrono>
#include <unistd.h>
#include <tuple>
using namespace std;


//#define THRESHOLD_VERIFY    10000

//#define PROFILING

// Comment out the following for XOR workload
#define WORKLOAD_COUNT

// Basic predicates of Allen's algebra
#define PREDICATE_EQUALS     1
#define PREDICATE_STARTS     2
#define PREDICATE_STARTED    3
#define PREDICATE_FINISHES   4
#define PREDICATE_FINISHED   5
#define PREDICATE_MEETS      6
#define PREDICATE_MET        7
#define PREDICATE_OVERLAPS   8
#define PREDICATE_OVERLAPPED 9
#define PREDICATE_CONTAINS   10
#define PREDICATE_CONTAINED  11
#define PREDICATE_PRECEDES   12
#define PREDICATE_PRECEDED   13

// Generalized predicates, ACM SIGMOD'22 gOverlaps
#define PREDICATE_GOVERLAPS  14


//#define USE_SS

//#define CONSTRUCTION_TWO_PASSES



#define HINT_OPTIMIZATIONS_NO          0
#define HINT_OPTIMIZATIONS_SS          1

#define HINT_M_OPTIMIZATIONS_NO                   0
#define HINT_M_OPTIMIZATIONS_SUBS                 1
#define HINT_M_OPTIMIZATIONS_SUBS_SORT            2
#define HINT_M_OPTIMIZATIONS_SUBS_SOPT            3
#define HINT_M_OPTIMIZATIONS_SUBS_SORT_SOPT       4
#define HINT_M_OPTIMIZATIONS_SUBS_SORT_SOPT_SS    5
#define HINT_M_OPTIMIZATIONS_SUBS_SORT_SOPT_CM    6
#define HINT_M_OPTIMIZATIONS_SUBS_SORT_SS_CM      7
#define HINT_M_OPTIMIZATIONS_ALL                  8

#define IF_HINT_OPTIMIZATIONS_NO                  0
#define IF_HINT_OPTIMIZATIONS_IDS                 1

#define IRHINT_OPTIMIZATION_ALPHA                      0
#define IRHINT_OPTIMIZATION_BETA                       1

#define TIF_OPTIMIZATION_NO           0
#define TIF_OPTIMIZATION_SLICING      1
#define TIF_OPTIMIZATION_SHARDING     2
#define TIF_OPTIMIZATION_HINT_ALPHA   3
#define TIF_OPTIMIZATION_HINT_BETA    4
#define TIF_OPTIMIZATION_HINT_GAMMA   5
#define TIF_OPTIMIZATION_HINT_GAMMA_HYBRID 6
#define TIF_OPTIMIZATION_HINT_DELTA   7
#define TIF_OPTIMIZATION_HINT_SLICING 8

typedef int PartitionId;
typedef int RecordId;
typedef int Timestamp;
typedef int TermId;


struct RunSettings
{
	string       method;
	const char   *dataFile;
	const char   *queryFile;
	bool         verbose;
	unsigned int typeQuery;
	unsigned int typePredicate;
	unsigned int numPartitions;
	unsigned int numBits;
	unsigned int maxBits;
	unsigned int shardImpactListGap;
	float        shardRelaxation;
	bool         topDown;
	bool         isAutoTuned;
	unsigned int numRuns;
	unsigned int typeOptimizations;
	unsigned int optimizationTIF;
	unsigned int optimizationIRHINT;

	void init()
	{
		verbose	           = false;
		topDown            = false;
		isAutoTuned        = false;
		numRuns            = 1;
		typeOptimizations  = 0;
		optimizationTIF    = TIF_OPTIMIZATION_NO;
		numPartitions      = 0;
		numBits            = 0;
		shardImpactListGap = 0; // 250
		shardRelaxation    = 0; // 10
	};

	string paramsstr()
	{
		vector<string> params;

		if (this->topDown)                 params.push_back("top-down");
		if (this->isAutoTuned)             params.push_back("auto-tuned");
		if (this->numBits != 0)            params.push_back("bits=" + to_string(this->numBits));
		if (this->numPartitions != 0)      params.push_back("partitions=" + to_string(this->numPartitions));
		if (this->shardRelaxation != 0)    params.push_back("relaxation=" + to_string(this->shardRelaxation));
		if (this->shardImpactListGap != 0) params.push_back("impact-list-gap=" + to_string(this->shardImpactListGap));

		if (params.empty()) return "default";

		string str;
		for (auto iter = params.begin(); iter != params.end(); ++iter) 
		{
			str += *iter;
			if (iter != params.end() - 1) str += ", ";
		}

		return str;
	}
};


struct StabbingQuery
{
	size_t id;
	Timestamp point;
    
    StabbingQuery()
    {
        
    };
    StabbingQuery(size_t i, Timestamp p)
    {
        id = i;
        point = p;
    };
};

struct RangeQuery
{
	size_t id;
	Timestamp start, end;

    RangeQuery()
    {
        
    };
    RangeQuery(size_t i, Timestamp s, Timestamp e)
    {
        id = i;
        start = s;
        end = e;
    };
};

struct RangeIRQuery
{
    size_t id;
    Timestamp start, end;
    vector<TermId> terms;

    RangeIRQuery()
    {
        
    };
    RangeIRQuery(size_t i, Timestamp s, Timestamp e)
    {
        id = i;
        start = s;
        end = e;
    };
    RangeIRQuery(size_t i, Timestamp s, Timestamp e, string ts)
    {
        stringstream ss(ts);
        string t;
        
        id = i;
        start = s;
        end = e;
        while (getline(ss, t, ','))
            terms.push_back(stoi(t));
    };
};



class Timer
{
private:
	using Clock = std::chrono::high_resolution_clock;
	Clock::time_point start_time, stop_time;
	
public:
	Timer()
	{
		start();
	}
	
	void start()
	{
		start_time = Clock::now();
	}
	
	
	double getElapsedTimeInSeconds()
	{
		return std::chrono::duration<double>(stop_time - start_time).count();
	}
	
	
	double stop()
	{
		stop_time = Clock::now();
		return getElapsedTimeInSeconds();
	}
};


// Imports from utils
string toUpperCase(char *buf);
bool checkPredicate(string strPredicate, RunSettings &settings);
bool checkOptimizationTIF(string strOptimization, RunSettings &settings);
bool checkOptimizationIRHINT(string strOptimization, RunSettings &settings);
bool checkOptimizations(string strOptimizations, RunSettings &settings);
//bool checktIFHINTVersion(string strVersion, RunSettings &settings);
void process_mem_usage(double& vm_usage, double& resident_set);
#endif // _GLOBAL_DEF_H_

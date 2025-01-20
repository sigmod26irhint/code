#include "def_global.h"
#include "containers/relations.h"



string toUpperCase(char *buf)
{
    auto i = 0;
    while (buf[i])
    {
        buf[i] = toupper(buf[i]);
        i++;
    }
    
    return string(buf);
}


bool checkPredicate(string strPredicate, RunSettings &settings)
{
    if (strPredicate == "EQUALS")
    {
        settings.typePredicate = PREDICATE_EQUALS;
        return true;
    }
    else if (strPredicate == "STARTS")
    {
        settings.typePredicate = PREDICATE_STARTS;
        return true;
    }
    else if (strPredicate == "STARTED")
    {
        settings.typePredicate = PREDICATE_STARTED;
        return true;
    }
    else if (strPredicate == "FINISHES")
    {
        settings.typePredicate = PREDICATE_FINISHES;
        return true;
    }
    else if (strPredicate == "FINISHED")
    {
        settings.typePredicate = PREDICATE_FINISHED;
        return true;
    }
    else if (strPredicate == "MEETS")
    {
        settings.typePredicate = PREDICATE_MEETS;
        return true;
    }
    else if (strPredicate == "MET")
    {
        settings.typePredicate = PREDICATE_MET;
        return true;
    }
    else if (strPredicate == "OVERLAPS")
    {
        settings.typePredicate = PREDICATE_OVERLAPS;
        return true;
    }
    else if (strPredicate == "OVERLAPPED")
    {
        settings.typePredicate = PREDICATE_OVERLAPPED;
        return true;
    }
    else if (strPredicate == "CONTAINS")
    {
        settings.typePredicate = PREDICATE_CONTAINS;
        return true;
    }
    else if (strPredicate == "CONTAINED")
    {
        settings.typePredicate = PREDICATE_CONTAINED;
        return true;
    }
    else if (strPredicate == "BEFORE")
    {
        settings.typePredicate = PREDICATE_PRECEDES;
        return true;
    }
    else if (strPredicate == "AFTER")
    {
        settings.typePredicate = PREDICATE_PRECEDED;
        return true;
    }
    if (strPredicate == "GOVERLAPS")
    {
        settings.typePredicate = PREDICATE_GOVERLAPS;
        return true;
    }

    return false;
}


bool checkOptimizations(string strOptimizations, RunSettings &settings)
{
    if (strOptimizations == "")
    {
        settings.typeOptimizations = HINT_M_OPTIMIZATIONS_NO;
        return true;
    }
    else if (strOptimizations == "SUBS+SORT")
    {
        settings.typeOptimizations = HINT_M_OPTIMIZATIONS_SUBS_SORT;
        return true;
    }
    else if (strOptimizations == "SUBS+SOPT")
    {
        settings.typeOptimizations = HINT_M_OPTIMIZATIONS_SUBS_SOPT;
        return true;
    }
    else if (strOptimizations == "SUBS+SORT+SOPT")
    {
        settings.typeOptimizations = HINT_M_OPTIMIZATIONS_SUBS_SORT_SOPT;
        return true;
    }
    else if (strOptimizations == "SUBS+SORT+SOPT+SS")
    {
        settings.typeOptimizations = HINT_M_OPTIMIZATIONS_SUBS_SORT_SOPT_SS;
        return true;
    }
    else if (strOptimizations == "SUBS+SORT+SOPT+CM")
    {
        settings.typeOptimizations = HINT_M_OPTIMIZATIONS_SUBS_SORT_SOPT_CM;
        return true;
    }
    else if (strOptimizations == "SUBS+SORT+SS+CM")
    {
        settings.typeOptimizations = HINT_M_OPTIMIZATIONS_SUBS_SORT_SS_CM;
        return true;
    }
    else if (strOptimizations == "ALL")
    {
        settings.typeOptimizations = HINT_M_OPTIMIZATIONS_ALL;
        return true;
    }
    else if (strOptimizations == "SS")
    {
        settings.typeOptimizations = HINT_OPTIMIZATIONS_SS;
        return true;
    }


    return false;
}


bool checkOptimizationTIF(string strOptimization, RunSettings &settings)
{
    if (strOptimization == "")
    {
        settings.optimizationTIF = TIF_OPTIMIZATION_NO;
        return true;
    }
    else if (strOptimization == "SLICING")
    {
        settings.optimizationTIF = TIF_OPTIMIZATION_SLICING;
        return true;
    }
    else if (strOptimization == "SHARDING")
    {
        settings.optimizationTIF = TIF_OPTIMIZATION_SHARDING;
        return true;
    }
    else if (strOptimization == "HINTA")
    {
        settings.optimizationTIF = TIF_OPTIMIZATION_HINT_ALPHA;
        return true;
    }
    else if (strOptimization == "HINTB")
    {
        settings.optimizationTIF = TIF_OPTIMIZATION_HINT_BETA;
        return true;
    }
    else if (strOptimization == "HINTG")
    {
        settings.optimizationTIF = TIF_OPTIMIZATION_HINT_GAMMA;
        return true;
    }
    else if (strOptimization == "HINTGH")
    {
        settings.optimizationTIF = TIF_OPTIMIZATION_HINT_GAMMA_HYBRID;
        return true;
    }
    else if (strOptimization == "HINTD")
    {
        settings.optimizationTIF = TIF_OPTIMIZATION_HINT_DELTA;
        return true;
    }
    else if (strOptimization == "HINTSLICING")
    {
        settings.optimizationTIF = TIF_OPTIMIZATION_HINT_SLICING;
        return true;
    }

    return false;
}


bool checkOptimizationIRHINT(string strOptimization, RunSettings &settings)
{
    if (strOptimization == "A")
    {
        settings.optimizationIRHINT = IRHINT_OPTIMIZATION_ALPHA;
        return true;
    }
    else if (strOptimization == "B")
    {
        settings.optimizationIRHINT = IRHINT_OPTIMIZATION_BETA;
        return true;
    }

    return false;
}


void process_mem_usage(double& vm_usage, double& resident_set)
{
    vm_usage     = 0.0;
    resident_set = 0.0;
    
    // the two fields we want
    unsigned long vsize;
    long rss;
    {
        std::string ignore;
        std::ifstream ifs("/proc/self/stat", std::ios_base::in);
        ifs >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore
        >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore
        >> ignore >> ignore >> vsize >> rss;
    }
    
    long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
    vm_usage = vsize / 1024.0;
    resident_set = rss * page_size_kb;
}


#define C_cmp(n,m,b)    (b*n)/pow(2,m)
#define C_acc(n,q,m,b)    b*(q - (2*n)/pow(2,m))
unsigned int determineOptimalNumBitsForHINT_M(const RecordId numRecords, const Timestamp domainSize, Timestamp avgRecordExtent, const float qe_precentage)
{
    // Gives BOOKS 10, WEBKIT 10, TAXIS 16, GREEND 16
    const double beta_cmp = 0.0000025245;
    const double beta_acc = 0.0000006846;
    // Gives BOOKS 8, WEBKIT 8, TAXIS 14, GREEND 14
//    const double beta_cmp = 0.0000016597;
//    const double beta_acc = 0.0000007023;
    const float threshold = 0.03;
    size_t n = numRecords;
    const Timestamp Lambda = domainSize;
    const double lambda_s = avgRecordExtent;
    const double lambda_q = (Lambda*qe_precentage/100);
    const double Q = n * (lambda_s + lambda_q) / Lambda;
    unsigned int m = 1, maxBits = int(log2(Lambda)+1);

    // Compute total cost for m_max
    const double C_min = C_cmp(n, maxBits, beta_cmp) + C_acc(n, Q, maxBits, beta_acc);
    
//    printf("C_cmp(%d) =\t%.10lf\n", maxBits, C_cmp(n, maxBits, beta_cmp));
//    printf("C_acc(%d) =\t%.10lf\n", maxBits, C_acc(n, Q, maxBits, beta_acc));
//    printf("C(%d) = C_cmp(%d) + C_acc(%d) =\t%.10lf\n", maxBits, maxBits, maxBits, C_min);
//    cout << endl;
    
    for (m = 1; m < maxBits; m++)
    {
        // Compute total cost for m
        double C = C_cmp(n, m, beta_cmp) + C_acc(n, Q, m, beta_acc);
    
//        printf("C_cmp(%d) =\t%.10lf\n", m, C_cmp(n, m, beta_cmp));
//        printf("C_acc(%d) =\t%.10lf\n", m, C_acc(n, Q, m, beta_acc));
//        printf("C(%d) = C_cmp(%d) + C_acc(%d) =\t%.10lf\t%f\n", m, m, m, C, (C-C_min)/C_min);
        if ((C > 0) && ((C-C_min)/C_min <= threshold))
            break;
    }
        

    return m;
}

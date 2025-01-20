#include "def_global.h"
#include "containers/relations.h"
#include "indices/inverted_file.h"
#include "indices/temporal_inverted_files.h"



void usage()
{
    cerr << endl;
    cerr << "PROJECT" << endl;
    cerr << "       irHINT: A Generic Index for Temporal Information Retrieval" << endl << endl;
    cerr << "USAGE" << endl;
    cerr << "       ./query_tif.exec [OPTION]... [DATA] [QUERIES]" << endl << endl;
    cerr << "DESCRIPTION" << endl;
    cerr << "       -? or -h" << endl;
    cerr << "              display this help message and exit" << endl;
    cerr << "       -v" << endl;
    cerr << "              activate verbose mode; print the trace for every query; otherwise only the final report" << endl;
    cerr << "       -o optimization" << endl;
    cerr << "              select optimization for temporal IF: \"SLICING\" from Berberich et al. @ SIGIR 2007 or \"SHARDING\" from Anand et al. @ SIGIR 2011 or \"HINTA\", \"HINTB\", \"HINTG\" from our paper; omit option for default basic temporal IF method" << endl;
    cerr << "       -i recs" << endl;
    cerr << "              sharding impact list gap; add an impact-list entry every recs records in a shard" << endl;
    cerr << "       -x relax" << endl;
    cerr << "              sharding relaxation; relax the stair-case property by relax percent of the domain" << endl;
//    cerr << "       -q predicate" << endl;
//    cerr << "              set predicate type: \"EQUALS\" or \"STARTS\" or \"STARTED\" or \"FINISHES\" or \"FINISHED\" or \"MEETS\" or \"MET\" or \"OVERLAPS\" or \"OVERLAPPED\" or \"CONTAINS\" or \"CONTAINED\" or \"BEFORE\" or \"AFTER\" or \"GOVERLAPS\""  << endl;
    cerr << "       -r runs" << endl;
    cerr << "              set the number of runs per query; by default 1" << endl << endl;
    cerr << "EXAMPLES" << endl;
//    cerr << "       ./query_hint_m.exec -b 10 -q gOVERLAPS -r 10 samples/AARHUS-BOOKS_2013.dat samples/AARHUS-BOOKS_2013_20k.qry" << endl;
//    cerr << "       ./query_hint_m.exec -b 10 -o subs+sort -q gOVERLAPS -v samples/AARHUS-BOOKS_2013.dat samples/AARHUS-BOOKS_2013_20k.qry" << endl;
//    cerr << "       ./query_hint_m.exec -b 10 -o subs+sort+ss+cm -q gOVERLAPS -v samples/AARHUS-BOOKS_2013.dat samples/AARHUS-BOOKS_2013_20k.qry" << endl;
//    cerr << "       ./query_hint_m.exec -b 10 -o all -q gOVERLAPS -v samples/AARHUS-BOOKS_2013.dat samples/AARHUS-BOOKS_2013_20k.qry" << endl << endl;
}

#define V1


int main(int argc, char **argv)
{
    Timer tim;
    size_t totalResultCNT = 0, totalResultXOR = 0, queryresultCNT = 0, queryresultXOR = 0, numQueries = 0;
    double totalIndexTime = 0, totalQueryTime = 0, querytime = 0, avgQueryTime = 0;
    Timestamp qstart, qend;
    string qterms;
    RunSettings settings;
    char c;
    string strQuery = "", strTimePredicate = "gOVERLAPS", strIRPredicate = "CONTAINMENT", strOptimization = "";
    RelationId qresult;
    vector<RelationId> qresult2;
    InvertedFileTemplate *idxIR;

    
    // Parse command line input
    settings.init();
    settings.method = "tIF";
//    while ((c = getopt(argc, argv, "?hvq:m:to:r:")) != -1)
    while ((c = getopt(argc, argv, "?ho:p:m:vr:i:x:")) != -1)
    {
        switch (c)
        {
            case '?':
            case 'h':
                usage();
                return 0;
                
            case 'o':
                strOptimization = toUpperCase((char*)optarg);
                break;
                
            case 'p':
                settings.numPartitions = atoi(optarg);
                break;

            case 'i':
                settings.shardImpactListGap = atoi(optarg);
                break;

            case 'x':
                settings.shardRelaxation = atof(optarg);
                break;

            case 'm':
                settings.numBits = atoi(optarg);
                break;

            case 'v':
                settings.verbose = true;
                break;
                
//            case 'q':
//                strPredicate = toUpperCase((char*)optarg);
//                break;
//
                
            case 'r':
                settings.numRuns = atoi(optarg);
                break;
                
            default:
                cerr << endl << "Error - unknown option '" << c << "'" << endl << endl;
                usage();
                return 1;
        }
    }
    
    
    // Sanity check
    if (argc-optind != 2)
    {
        usage();
        return 1;
    }
//    if (!checkPredicate(strPredicate, settings))
//    {
//        if (strPredicate == "")
//            cerr << endl << "Error - predicate type not defined" << endl << endl;
//        else
//            cerr << endl << "Error - unknown predicate type \"" << strPredicate << "\"" << endl << endl;
//        usage();
//        return 1;
//    }
    if (!checkOptimizationTIF(strOptimization, settings))
    {
        cerr << endl << "Error - unknown Temporal IF optimization \"" << strOptimization << "\"" << endl << endl;
        usage();
        return 1;
    }
    if ((settings.optimizationTIF != TIF_OPTIMIZATION_SLICING) && (settings.optimizationTIF != TIF_OPTIMIZATION_HINT_SLICING) && (settings.numPartitions != 0))
    {
        cerr << endl << "Error - setting number of partitions compatible only with a slicing Temporal IF optimization" << endl << endl;
        usage();
        return 1;
    }
    if ((settings.optimizationTIF != TIF_OPTIMIZATION_SHARDING) && (settings.shardImpactListGap != 0))
    {
        cerr << endl << "Error - setting impact-list gap compatible only with a sharding Temporal IF optimization" << endl << endl;
        usage();
        return 1;
    }
    if ((settings.optimizationTIF != TIF_OPTIMIZATION_SHARDING) && (settings.shardRelaxation != 0))
    {
        cerr << endl << "Error - setting staircase-property relaxation compatible only with a sharding Temporal IF optimization" << endl << endl;
        usage();
        return 1;
    }
    if ((settings.optimizationTIF != TIF_OPTIMIZATION_HINT_ALPHA) && (settings.optimizationTIF != TIF_OPTIMIZATION_HINT_BETA) && (settings.optimizationTIF != TIF_OPTIMIZATION_HINT_GAMMA) && (settings.optimizationTIF != TIF_OPTIMIZATION_HINT_GAMMA_HYBRID) && (settings.optimizationTIF != TIF_OPTIMIZATION_HINT_DELTA) && (settings.optimizationTIF != TIF_OPTIMIZATION_HINT_SLICING) && (settings.numBits != 0))
    {
        cerr << endl << "Error - setting number of bits compatible only with a HINT Temporal IF optimization" << endl << endl;
        usage();
        return 1;
    }
    settings.dataFile = argv[optind];
    settings.queryFile = argv[optind+1];
    
    
    // Load data and queries
    IRelation iR(settings.dataFile);
    
    ifstream fQ(settings.queryFile);
    if (!fQ)
    {
        cerr << endl << "Error - cannot open query file \"" << settings.queryFile << "\"" << endl << endl;
        return 1;
    }

    
    // Build index
    switch (settings.optimizationTIF)
    {
        case TIF_OPTIMIZATION_NO:
            tim.start();
            idxIR = new TemporalInvertedFile(iR);
            totalIndexTime = tim.stop();
            break;

        case TIF_OPTIMIZATION_SLICING:
            tim.start();
            idxIR = new TemporalInvertedFile_Slicing(iR, settings.numPartitions);
            totalIndexTime = tim.stop();
            break;

        case TIF_OPTIMIZATION_SHARDING:
            tim.start();
            idxIR = new TemporalInvertedFile_Sharding(iR, settings.shardImpactListGap, settings.shardRelaxation);
            totalIndexTime = tim.stop();
            break;
            
        case TIF_OPTIMIZATION_HINT_ALPHA:
            tim.start();
            idxIR = new TemporalInvertedFile_HINTAlpha(iR, settings.numBits);
            totalIndexTime = tim.stop();
            break;

        case TIF_OPTIMIZATION_HINT_BETA:
            tim.start();
            idxIR = new TemporalInvertedFile_HINTBeta(iR, settings.numBits);
            totalIndexTime = tim.stop();
            break;

        case TIF_OPTIMIZATION_HINT_GAMMA:
            tim.start();
            idxIR = new TemporalInvertedFile_HINTGamma(iR, settings.numBits);
            totalIndexTime = tim.stop();
            break;

        case TIF_OPTIMIZATION_HINT_GAMMA_HYBRID:
            tim.start();
            idxIR = new TemporalInvertedFile_HINTGammaH(iR, settings.numBits);
            totalIndexTime = tim.stop();
            break;

        case TIF_OPTIMIZATION_HINT_DELTA:
            tim.start();
            idxIR = new TemporalInvertedFile_HINTDelta(iR, settings.numBits);
            totalIndexTime = tim.stop();
            break;

        case TIF_OPTIMIZATION_HINT_SLICING:
            tim.start();
            idxIR = new TemporalInvertedFile_HINTSlicing(iR, settings.numPartitions, settings.numBits);
            totalIndexTime = tim.stop();
            break;
    }
//    cout<<"indexed";getchar();

    // Execute queries
    size_t sumQ = 0;

    if (settings.verbose)
        cout << "Query interval\tQuery terms\tPredicate\tMethod\tOptimizations\tResultCNT\tResultXOR\tTime" << endl;
    while (fQ >> qstart >> qend >> qterms)
    {
        sumQ += qend-qstart;
        numQueries++;
        
        double sumT = 0;
        for (auto r = 0; r < settings.numRuns; r++)
        {
            qresult.clear();
            qresult2.clear();
            qresult.reserve(iR.getFrequency(qterms[0]));

            tim.start();
#ifdef V1
            idxIR->executeContainment(RangeIRQuery(numQueries, qstart, qend, qterms), qresult);
#else
            idxIR->executeContainment(RangeIRQuery(numQueries, qstart, qend, qterms), qresult2);
#endif
            querytime = tim.stop();
            
            sumT += querytime;
            totalQueryTime += querytime;
        }

#ifdef V1
        queryresultCNT = qresult.size();
        queryresultXOR = 0;
        for (const RecordId &rid: qresult)
        {
            queryresultXOR ^= rid;
//            cout << rid << endl;
        }
#else
        queryresultCNT = 0;
        queryresultXOR = 0;
        for (auto i = 0; i < qresult2.size(); i++)
        {
            qresult = qresult2[i];
            queryresultCNT += qresult.size();
            queryresultXOR += 0;
            for (const RecordId &rid: qresult)
            {
                queryresultXOR ^= rid;
//                cout << rid << endl;
            }
        }
#endif

        if (settings.verbose)
        {
            cout << "[" << qstart << "," << qend << "]\t" << qterms << "\t" << strTimePredicate << "\t" << strIRPredicate << "\t" << settings.method << "\t" << queryresultCNT << "\t" << queryresultXOR << "\t";
            printf("%0.10lf\n", querytime);
        }
        
        totalResultCNT += queryresultCNT;
        totalResultXOR += queryresultXOR;
        avgQueryTime += sumT/settings.numRuns;
    }
    fQ.close();
    
    
    // Report
    idxIR->getStats();
    cout << endl;
    cout << "Temporal IF" << endl;
    cout << "===========" << endl;
    cout << "Input" << endl;
    cout << "  Data file                    : " << settings.dataFile << endl;
    cout << "  Num of records               : " << iR.size() << endl;
    cout << "  Interval domain              : [" << iR.gstart << ".." << iR.gend << "]" << endl;
    cout << "  Terms dictionary             : [0," << iR.dictionarySize-1 << "]" << endl;
    cout << "  Avg interval extent [%]      : "; printf("%f\n", iR.avgRecordExtent*100/(iR.gend-iR.gstart));
    cout << "  Avg terms set size           : "; printf("%f\n", iR.avgTermsSetSize);
    cout << endl;
    cout << "Index" << endl;
    cout << "  Type                         : " << ((settings.optimizationTIF == TIF_OPTIMIZATION_NO)? "basic": strOptimization) << endl;
    cout << "  Parameters                   : " << settings.paramsstr() << endl;
    printf( "  Indexing time [secs]         : %f\n", totalIndexTime);
    cout << "  Size [Bytes]                 : " << idxIR->getSize() << endl;
    cout << endl;
    cout << "Queries" << endl;
    cout << "  Query file                   : " << settings.queryFile << endl;
    cout << "  Type                         : " << "time-travel containment" << endl;
    cout << "  Num of queries               : " << numQueries << endl;
    cout << "  Avg query interval extent [%]: "; printf("%f\n", (((float)sumQ/numQueries)*100)/(iR.gend-iR.gstart));
    cout << "  Avg query terms set size  [%]: TODO" << endl;
    cout << "  Num of runs per query        : " << settings.numRuns << endl;
    cout << endl;
    cout << "Output" << endl;
    cout << "  Total result [COUNT]         : " << totalResultCNT << endl;
    cout << "  Total result [XOR]           : " << totalResultXOR << endl;
    printf( "  Total querying time [secs]   : %f\n", totalQueryTime/settings.numRuns);
    printf( "  Avg querying time [secs]     : %f\n", avgQueryTime/numQueries);
    printf( "  Throughput [queries/sec]     : %f\n", numQueries/(totalQueryTime/settings.numRuns));
    cout << endl;
    
    delete idxIR;


    return 0;
}

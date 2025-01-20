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
    cerr << "       ./query_tif.exec [OPTION]... [DATA] [UPDATES]" << endl << endl;
    cerr << "DESCRIPTION" << endl;
    cerr << "       -? or -h" << endl;
    cerr << "              display this help message and exit" << endl;
    cerr << "       -o optimization" << endl;
    cerr << "              select optimization for temporal IF: \"SLICING\" from Berberich et al. @ SIGIR 2007 or \"SHARDING\" from Anand et al. @ SIGIR 2011 or \"HINTA\", \"HINTB\", \"HINTG\" from our paper; omit option for default basic temporal IF method" << endl;
    cerr << "       -i recs" << endl;
    cerr << "              sharding impact list gap; add an impact-list entry every recs records in a shard" << endl;
    cerr << "       -x relax" << endl;
    cerr << "              sharding relaxation; relax the stair-case property by relax percent of the domain" << endl;
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
    double totalIndexTime = 0, totalQueryTime = 0, querytime = 0, avgQueryTime = 0, totalInsertionTime = 0;
    Timestamp qstart, qend;
    string qterms;
    RunSettings settings;
    char c;
    string strQuery = "", strTimePredicate = "gOVERLAPS", strIRPredicate = "CONTAINMENT", strOptimization = "";
    RelationId qresult;
    vector<RelationId> qresult2;
    InvertedFileTemplate *idxIR;
    TermId dictionarySize = 0;
    size_t sumQ = 0;

    
    // Parse command line input
    settings.init();
    settings.method = "tIF";
//    while ((c = getopt(argc, argv, "?hvq:m:to:r:")) != -1)
    while ((c = getopt(argc, argv, "?hd:o:p:m:i:x:r:")) != -1)
    {
        switch (c)
        {
            case '?':
            case 'h':
                usage();
                return 0;
                
            case 'd':
                dictionarySize = atoi(optarg);
                break;

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
    if (argc-optind < 2)
    {
        usage();
        return 1;
    }
    if (dictionarySize == 0)
    {
        cerr << endl << "Error - dictionary size must be provided" << endl << endl;
        usage();
        return 1;
    }
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
    
    
    // Load data and updates
    IRelation iR(settings.dataFile);
    IRelation iU(settings.queryFile);
    for (IRecord &r: iU)
        r.id += iR.size();

    
    // Build index
    switch (settings.optimizationTIF)
    {
        case TIF_OPTIMIZATION_NO:
            tim.start();
            idxIR = new TemporalInvertedFile(iR, iU, dictionarySize);
            totalIndexTime = tim.stop();
            break;

        case TIF_OPTIMIZATION_SLICING:
            tim.start();
            idxIR = new TemporalInvertedFile_Slicing(iR, iU, dictionarySize, settings.numPartitions);
            totalIndexTime = tim.stop();
            break;

        case TIF_OPTIMIZATION_SHARDING:
            tim.start();
            idxIR = new TemporalInvertedFile_Sharding(iR, iU, settings.shardImpactListGap, settings.shardRelaxation);
            totalIndexTime = tim.stop();
            break;
            
//        case TIF_OPTIMIZATION_HINT_ALPHA:
//            tim.start();
//            idxIR = new TemporalInvertedFile_HINTAlpha(iR, iU, settings.numBits);
//            totalIndexTime = tim.stop();
//            break;

        // TODO: Need to maintain sorting, but due to using CM, it's complicated
        case TIF_OPTIMIZATION_HINT_BETA:
            tim.start();
            idxIR = new TemporalInvertedFile_HINTBeta(iR, iU, dictionarySize, settings.numBits);
            totalIndexTime = tim.stop();
            break;

        case TIF_OPTIMIZATION_HINT_GAMMA:
            tim.start();
            idxIR = new TemporalInvertedFile_HINTGamma(iR, iU, dictionarySize, settings.numBits);
            totalIndexTime = tim.stop();
            break;

//        case TIF_OPTIMIZATION_HINT_GAMMA_HYBRID:
//            tim.start();
//            idxIR = new TemporalInvertedFile_HINTGammaH(iR, iU, settings.numBits);
//            totalIndexTime = tim.stop();
//            break;

//        case TIF_OPTIMIZATION_HINT_DELTA:
//            tim.start();
//            idxIR = new TemporalInvertedFile_HINTDelta(iR, iU, settings.numBits);
//            totalIndexTime = tim.stop();
//            break;

        case TIF_OPTIMIZATION_HINT_SLICING:
            tim.start();
            idxIR = new TemporalInvertedFile_HINTSlicing(iR, iU, dictionarySize, settings.numPartitions, settings.numBits);
            totalIndexTime = tim.stop();
            break;
    }
//    cout<<"indexed";getchar();

    cout << "  Size [Bytes]                 : " << idxIR->getSize() << endl;

    tim.start();
    idxIR->insert(iU);
    totalInsertionTime = tim.stop();
   

    // Execute queries
    if (argc-optind > 2)
    {
        settings.queryFile = argv[optind+2];
        ifstream fQ(settings.queryFile);
        
        if (!fQ)
        {
            cerr << endl << "Error - cannot open query file \"" << settings.queryFile << "\"" << endl << endl;
            return 1;
        }
        
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
    }
    
    
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
    cout << "  Optimization                 : " << ((settings.optimizationTIF == TIF_OPTIMIZATION_NO)? "basic": strOptimization) << endl;
    cout << "  Parameters                   : " << settings.paramsstr() << endl;
    printf( "  Indexing time [secs]         : %f\n", totalIndexTime);
    cout << "  Size [Bytes]                 : " << idxIR->getSize() << endl;
    cout << endl;
    if (argc-optind > 2)
    {
        cout << "Queries" << endl;
        cout << "  Query file                   : " << argv[optind+2] << endl;
        cout << "  Type                         : " << "time-travel containment" << endl;
        cout << "  Num of queries               : " << numQueries << endl;
        cout << "  Avg query interval extent [%]: "; printf("%f\n", (((float)sumQ/numQueries)*100)/(iR.gend-iR.gstart));
        cout << "  Avg query terms set size  [%]: TODO" << endl;
        cout << "  Num of runs per query        : " << settings.numRuns << endl;
        cout << endl;
    }
    cout << "Updates" << endl;
    cout << "  Batch file                   : " << argv[optind+1] << endl;
    cout << "  Num of insertions            : " << iU.size() << endl;
    cout << endl;
    cout << "Output" << endl;
    if (argc-optind > 2)
    {
        cout << "  Total result [COUNT]         : " << totalResultCNT << endl;
        cout << "  Total result [XOR]           : " << totalResultXOR << endl;
        printf( "  Total querying time [secs]   : %f\n", totalQueryTime/settings.numRuns);
        printf( "  Avg querying time [secs]     : %f\n", avgQueryTime/numQueries);
        printf( "  Throughput [queries/sec]     : %f\n", numQueries/(totalQueryTime/settings.numRuns));
    }
    printf( "  Total insertion time [secs]  : %f\n", totalInsertionTime);
    cout << endl;
    
    delete idxIR;


    return 0;
}

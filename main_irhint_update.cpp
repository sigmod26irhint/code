#include "def_global.h"
#include "containers/relations.h"
#include "indices/irhint.h"



void usage()
{
    cerr << endl;
    cerr << "PROJECT" << endl;
    cerr << "       irHINT: A Generic Index for Temporal Information Retrieval" << endl << endl;
    cerr << "USAGE" << endl;
    cerr << "       ./query_irhint.exec [OPTION]... [DATA] [QUERIES]" << endl << endl;
    cerr << "DESCRIPTION" << endl;
    cerr << "       -? or -h" << endl;
    cerr << "              display this help message and exit" << endl;
    cerr << "       -v" << endl;
    cerr << "              activate verbose mode; print the trace for every query; otherwise only the final report" << endl;
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
    HierarchicalIRIndex *idxIR;
    TermId dictionarySize = 0;
    size_t sumQ = 0;

    
    // Parse command line input
    settings.init();
    settings.method = "irHINT";
//    while ((c = getopt(argc, argv, "?hvq:m:to:r:")) != -1)
    while ((c = getopt(argc, argv, "?hd:o:m:r:")) != -1)
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
    if (!checkOptimizationIRHINT(strOptimization, settings))
    {
        if (strOptimization == "")
            cerr << endl << "Error - irHINT optimization not defined" << endl << endl;
        else
            cerr << endl << "Error - unknown irHINT optimization \"" << strOptimization << "\"" << endl << endl;
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
    // Build index
    if (settings.optimizationIRHINT == IRHINT_OPTIMIZATION_ALPHA)
    {
        tim.start();
        idxIR = new irHINTa(iR, iU, dictionarySize, settings.numBits);
        totalIndexTime = tim.stop();
    }
    else if (settings.optimizationIRHINT == IRHINT_OPTIMIZATION_BETA)
    {
        tim.start();
        idxIR = new irHINTb(iR, iU, settings.numBits);
        totalIndexTime = tim.stop();
    }

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
                qresult.reserve(iR.getFrequency(qterms[0]));

                tim.start();
                idxIR->executeContainment(RangeIRQuery(numQueries, qstart, qend, qterms), qresult);
                querytime = tim.stop();

                sumT += querytime;
                totalQueryTime += querytime;
            }

            queryresultCNT = qresult.size();
            queryresultXOR = 0;
            for (const RecordId &rid: qresult)
                queryresultXOR ^= rid;
            
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
    cout << "  Optimization                 : " << strOptimization << endl;
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

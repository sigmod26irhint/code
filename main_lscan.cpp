#include "def_global.h"
#include "containers/relations.h"



void usage()
{
    cerr << endl;
    cerr << "PROJECT" << endl;
    cerr << "       irHINT: A Generic Index for Temporal Information Retrieval" << endl << endl;
    cerr << "USAGE" << endl;
    cerr << "       ./query_lscan.exec [OPTION]... [DATA] [QUERIES]" << endl << endl;
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
    double totalIndexTime = 0, totalQueryTime = 0, querytime = 0, avgQueryTime = 0;
    Timestamp qstart, qend;
    string qterms;
    RunSettings settings;
    char c;
    string strQuery = "", strTimePredicate = "gOVERLAPS", strIRPredicate = "CONTAINMENT";
    RelationId qresult;

    
    // Parse command line input
    settings.init();
    settings.method = "lscan";
//    while ((c = getopt(argc, argv, "?hvq:m:to:r:")) != -1)
    while ((c = getopt(argc, argv, "?hvr:")) != -1)
    {
        switch (c)
        {
            case '?':
            case 'h':
                usage();
                return 0;
                
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
            qresult.reserve(iR.getFrequency(qterms[0]));

            tim.start();
            iR.executeContainment(RangeIRQuery(numQueries, qstart, qend, qterms), qresult);
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
    
    
    // Report
    cout << endl;
    cout << "Linear scan" << endl;
    cout << "===========" << endl;
    cout << "Input" << endl;
    cout << "  Num of records               : " << iR.size() << endl;
    cout << "  Interval domain              : [" << iR.gstart << ".." << iR.gend << "]" << endl;
    cout << "  Terms dictionary             : [0," << iR.dictionarySize-1 << "]" << endl;
    cout << "  Avg interval extent [%]      : "; printf("%f\n", iR.avgRecordExtent*100/(iR.gend-iR.gstart));
    cout << "  Avg terms set size           : "; printf("%f\n", iR.avgTermsSetSize);
    cout << endl;
    cout << "Index" << endl;
    cout << endl;
    cout << "Queries" << endl;
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

    
    return 0;
}

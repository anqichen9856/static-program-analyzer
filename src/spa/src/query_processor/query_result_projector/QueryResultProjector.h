#ifndef AUTOTESTER_QUERYRESULTPROJECTOR_H
#define AUTOTESTER_QUERYRESULTPROJECTOR_H

#include <string>
#include <vector>
#include <list>
#include <iterator>
#include "query_processor/query_evaluator/QueryResult.h"
#include "query_processor/Query.h"

using namespace std;

class QueryResultProjector {
public:
    static QueryResultProjector *getInstance();

    void formatResults(list<string> &results, QueryResult* query_res, Query query, bool res);

private:
    static QueryResultProjector *instance;
};


#endif //AUTOTESTER_QUERYRESULTPROJECTOR_H

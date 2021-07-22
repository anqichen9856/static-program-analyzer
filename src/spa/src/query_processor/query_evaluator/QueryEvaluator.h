#ifndef AUTOTESTER_QUERYEVALUATOR_H
#define AUTOTESTER_QUERYEVALUATOR_H

#include <vector>
#include <string>
#include "query_processor/Query.h"
#include "QueryResult.h"
#include "query_processor/query_preprocessor/QueryParser.h"
#include "query_processor/query_optimizer/QueryOptimizer.h"

class QueryEvaluator {
public:
    static QueryEvaluator *getInstance();

    bool evaluateQuery(Query query, QueryResult* query_res);
    bool evaluateWithOptimization(Query query, QueryResult* query_res);

private:
    static QueryEvaluator *instance;
    bool evaluateSelectClause(Query* query, QueryResult* query_res);
};


#endif //AUTOTESTER_QUERYEVALUATOR_H
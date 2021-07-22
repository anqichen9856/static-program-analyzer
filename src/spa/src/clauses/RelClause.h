#ifndef TEAM21_CP_SPA_20S1_RELCLAUSE_H
#define TEAM21_CP_SPA_20S1_RELCLAUSE_H

#include "clauses/Clause.h"
#include "query_processor/Query.h"
#include "exceptions/SPAException.h"
#include "query_processor/query_evaluator/QueryResult.h"

class RelClause : public Clause {
public:
    RelClause(string left, string right);

    bool evaluate(Query* query, QueryResult* query_res) override;

    static void insertPair(unordered_map<string,
        vector<pair<string, string>>> &common_res, string left, string right, string i, string j);

    static void insertPairs(unordered_map<string,
        vector<pair<string, string>>> &common_res, string left, string right, string i, vector<string> vars);


};


#endif //TEAM21_CP_SPA_20S1_RELCLAUSE_H

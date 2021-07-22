#ifndef AUTOTESTER_QUERYOPTIMIZER_H
#define AUTOTESTER_QUERYOPTIMIZER_H

#include "query_processor/Query.h"
#include "query_processor/query_preprocessor/QueryParser.h"
#include <unordered_map>
#include <set>

class QueryOptimizer {
public:
    QueryOptimizer(Query *query);
    void optimizeQuery();
    void rankByClauseType(vector<Clause*> &lst);

private:
    Query *query;
    set<string> selected_synonym;
    list<Clause*> no_syn;
    list<Clause*> with_one_syn;
    list<Clause*> with_two_syns;

    // for grouping of clauses based on synonyms
    unordered_map<int, list<Clause*>> groups;
    unordered_map<int, set<string>> syn_in_groups;

    vector<list<Clause*>> with_select;
    vector<list<Clause*>> no_select;

    void groupSynNoSyn();

    void sortGroups();

    void groupByConnectedSyn();
};


#endif //AUTOTESTER_QUERYOPTIMIZER_H

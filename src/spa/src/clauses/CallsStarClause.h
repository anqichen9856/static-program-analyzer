#ifndef TEAM21_CP_SPA_20S1_CALLSSTARCLAUSE_H
#define TEAM21_CP_SPA_20S1_CALLSSTARCLAUSE_H

#include "RelClause.h"

class CallsStarClause : public RelClause {

public:
    CallsStarClause(string left, string right);

    void validate(unordered_map<string, DesignEntity> map) override;

    bool evaluate(Query *query, QueryResult* query_res) override;

    string getClauseName() override;

    int getRank() override;

};

#endif //TEAM21_CP_SPA_20S1_CALLSSTARCLAUSE_H

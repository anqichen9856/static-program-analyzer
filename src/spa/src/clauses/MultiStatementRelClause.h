#ifndef AUTOTESTER_MULTISTATEMENTRELCLAUSE_H
#define AUTOTESTER_MULTISTATEMENTRELCLAUSE_H


#include "RelClause.h"

class MultiStatementRelClause : public RelClause {
public:
    MultiStatementRelClause(string left, string right);

    void validate(unordered_map<string, DesignEntity> map) override;

    bool evaluate(Query *query, QueryResult* query_res) override;

private:
    virtual vector<int> getRel(int statement) = 0;

    virtual vector<int> getInvRel(int statement) = 0;

    virtual bool isRel(int before, int after) = 0;

    virtual unsigned int getRelListSize() = 0;

    virtual bool evalSynSyn(string left, QueryResult* query_res, unordered_map<string, DesignEntity> syns) = 0;
};

#endif //AUTOTESTER_MULTISTATEMENTRELCLAUSE_H

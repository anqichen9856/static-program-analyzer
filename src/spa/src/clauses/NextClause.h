#ifndef TEAM21_CP_SPA_20S1_NEXTCLAUSE_H
#define TEAM21_CP_SPA_20S1_NEXTCLAUSE_H


#include "MultiStatementRelClause.h"

class NextClause : public MultiStatementRelClause {
public:
    NextClause(string left, string right);

    string getClauseName() override;

    void validate(unordered_map<string, DesignEntity> map) override;

    int getRank() override;

private:

    vector<int> getRel(int statement) override;

    vector<int> getInvRel(int statement) override;

    bool isRel(int before, int after) override;

    unsigned int getRelListSize() override;

    bool evalSynSyn(string left, QueryResult* query_res, unordered_map<string, DesignEntity> syns) override;
};


#endif //TEAM21_CP_SPA_20S1_NEXTCLAUSE_H

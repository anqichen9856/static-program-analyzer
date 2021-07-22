#ifndef TEAM21_CP_SPA_20S1_NEXTSTARCLAUSE_H
#define TEAM21_CP_SPA_20S1_NEXTSTARCLAUSE_H


#include "MultiStatementRelClause.h"

class NextStarClause : public MultiStatementRelClause {
public:
    NextStarClause(string left, string right);
    
    void validate(unordered_map<string, DesignEntity> map) override;

    string getClauseName() override;

    int getRank() override;

private:

    vector<int> getRel(int statement) override;

    vector<int> getInvRel(int statement) override;

    bool isRel(int before, int after) override;

    unsigned int getRelListSize() override;

    bool evalSynSyn(string left, QueryResult* query_res, unordered_map<string, DesignEntity> syns) override;
};


#endif //TEAM21_CP_SPA_20S1_NEXTSTARCLAUSE_H

#ifndef TEAM21_CP_SPA_20S1_FOLLOWSCLAUSE_H
#define TEAM21_CP_SPA_20S1_FOLLOWSCLAUSE_H

#include "IntRelClause.h"

class FollowsClause : public IntRelClause {

public:
    FollowsClause(string left, string right);

    void validate(unordered_map<string, DesignEntity> map) override;

    string getClauseName() override;

    int getRank() override;

private:
    int getIntRelSize(int statement) override;

    int getInvIntRel(int statement) override;

    bool isIntRel(int before, int after) override;

    unsigned int getIntRelListSize() override;

    bool evalIntSyn(string left, string right, QueryResult* query_res, unordered_map<string, DesignEntity> syns) override;

    bool evalIntWild(string left, string right) override;

    bool evalSynInt(string left, string right, QueryResult* query_res, unordered_map<string, DesignEntity> syns) override;

    bool evalSynSynHelper(string left, string right, QueryResult* query_res, unordered_map<string, DesignEntity> syns) override;

};

#endif //TEAM21_CP_SPA_20S1_FOLLOWSCLAUSE_H

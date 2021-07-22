
#ifndef AUTOTESTER_AFFECTSSTARCLAUSE_H
#define AUTOTESTER_AFFECTSSTARCLAUSE_H
#include "NextStarClause.h"

class AffectsStarClause : public NextStarClause {
public:
    AffectsStarClause(string left, string right);

    string getClauseName() override;

    void validate(unordered_map<string, DesignEntity> map) override;

    int getRank() override;

private:

    vector<int> getRel(int statement) override;

    vector<int> getInvRel(int statement) override;

    bool isRel(int before, int after) override;

    unsigned int getRelListSize() override;
};


#endif //TEAM21_CP_SPA_20S1_AFFECTSSTARCLAUSE_H

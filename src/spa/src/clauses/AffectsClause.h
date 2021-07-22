
#ifndef AUTOTESTER_AFFECTSCLAUSE_H
#define AUTOTESTER_AFFECTSCLAUSE_H
#include "NextStarClause.h"

class AffectsClause : public NextStarClause {
public:
    AffectsClause(string left, string right);

    string getClauseName() override;

    void validate(unordered_map<string, DesignEntity> map) override;

    int getRank() override;

private:

    vector<int> getRel(int statement) override;

    vector<int> getInvRel(int statement) override;

    bool isRel(int before, int after) override;

    unsigned int getRelListSize() override;
};


#endif //TEAM21_CP_SPA_20S1_AFFECTSCLAUSE_H

#ifndef TEAM21_CP_SPA_20S1_MODIFIESCLAUSE_H
#define TEAM21_CP_SPA_20S1_MODIFIESCLAUSE_H


#include "NamesRelClause.h"

class ModifiesClause : public NamesRelClause {
public:
    ModifiesClause(string left, string right);

    void validate(unordered_map<string, DesignEntity> map) override;

    string getClauseName() override;

    int getRank() override;
    
private:
    bool isNamesRel(int s, string v) override;

    bool isNamesRel(string p, string v) override;

    vector<string> getAllNamesRel(int s) override;

    vector<string> getAllNamesRel(string p) override;
};


#endif //TEAM21_CP_SPA_20S1_MODIFIESCLAUSE_H

#ifndef AUTOTESTER_PATTERNIF_H
#define AUTOTESTER_PATTERNIF_H

#include "clauses/PatternClause.h"

class PatternIf : public PatternClause {
public:
    PatternIf(string pattern_syn, string left, string mid, string right);

    void validate(unordered_map<string, DesignEntity> map) override;

    bool evaluate(Query *query, QueryResult* query_res) override;

    bool equals(PatternClause *other) override;

    string getClauseName() override;

    string getMidParam();

    int getRank() override;

private:
    string left;
    string mid;
    string right;
    string pattern_syn;
};

#endif //AUTOTESTER_PATTERNIF_H

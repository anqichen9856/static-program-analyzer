#ifndef AUTOTESTER_PATTERNWHILE_H
#define AUTOTESTER_PATTERNWHILE_H

#include "clauses/PatternClause.h"

class PatternWhile : public PatternClause {
public:
    PatternWhile(string pattern_syn, string left, string right);

    void validate(unordered_map<string, DesignEntity> map) override;

    bool evaluate(Query *query, QueryResult* query_res) override;

    string getClauseName() override;

    int getRank() override;

private:
    string left;
    string right;
    string pattern_syn;


};


#endif //AUTOTESTER_PATTERNWHILE_H

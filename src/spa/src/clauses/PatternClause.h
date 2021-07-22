#ifndef AUTOTESTER_PATTERNCLAUSE_H
#define AUTOTESTER_PATTERNCLAUSE_H

#include "clauses/Clause.h"
#include "query_processor/Query.h"

class PatternClause : public Clause {
public:
    // PatternClause(string left, string right);

    PatternClause(string pattern_syn, string left, string right);
    virtual string getPatternSyn();
    virtual bool equals(PatternClause* other);
    int getNumOfSyn() override;
    set<string> getSynonyms() override;

private:
    string left;
    string right;
    string pattern_syn;

};

#endif //AUTOTESTER_PATTERNCLAUSE_H

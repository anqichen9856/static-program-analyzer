#include "PatternClause.h"

PatternClause::PatternClause(string pattern_syn, string left, string right) : Clause(left, right) {
    this->left = Clause::getLeftParam();
    this->right = Clause::getRightParam();
    this->pattern_syn = pattern_syn;
}

string PatternClause::getPatternSyn() {
    return pattern_syn;;
}

bool PatternClause::equals(PatternClause* p) {
    return this->getClauseName() == p->getClauseName()
           && this->getLeftParam() ==p->getLeftParam()
           && this->getRightParam() == p->getRightParam()
           && this->getPatternSyn() == p->getPatternSyn();
}

int PatternClause::getNumOfSyn() {
    if (isSynonym(left)) {
        return 2;
    }
    return 1;
}

set<string> PatternClause::getSynonyms() {
    set<string> syns = {};
    if (isSynonym(pattern_syn)) {
        syns.insert(pattern_syn);
    }
    if (isSynonym(left)) {
        syns.insert(left);
    }
    return syns;
}


#include "PatternIf.h"

PatternIf::PatternIf(string pattern_syn, string left, string mid, string right) : PatternClause(pattern_syn, left, right) {
    this->left = left;
    this->mid = mid;
    this->right = right;
    this->pattern_syn = pattern_syn;
}

void PatternIf::validate(unordered_map<string,DesignEntity> syn_entity_map) {
    bool var_syn = isSynonym(left) && syn_entity_map[left] == DesignEntity::Variable;
    if (!(var_syn || isName(left) || isWildcard(left))) {
        throw InvalidSemanticException("Invalid left argument in PatternIf.");
    }
    if (!(isWildcard(mid) && isWildcard(right))) {
        throw InvalidQueryException("Last two arguments in PatternIf clause is not Wildcard.");
    }
    //left: syn & variable, name, wildcard
    //mid: wildcard
    //right: wildcard
}

bool PatternIf::evaluate(Query *query, QueryResult* query_res) {

    if (isWildcard(left) || isName(left)) {
        // ifs is the only synonym
        if (!query_res->isInTable(pattern_syn)) {
            auto res = PKB::getInstance()->convertIntToString(PKB::getInstance()->getMatchedIf(strip(left)));
            if (res.empty()) {
                return false;
            }
            return query_res->addOneSyn(pattern_syn, res);
        } else {
            string new_left = strip(left);
            return query_res->filterOneSyn(pattern_syn, [new_left](string s) {
                return PKB::getInstance()->isControlVar(stoi(s), new_left);
            });
        }
    } else {
        // 2 synonyms: ifs, v
        if (!query_res->isInTable(pattern_syn) && !query_res->isInTable(left)) {
            auto res = PKB::getInstance()->getMatchedIfPair();
            if (res.empty()) {
                return false;
            }
            return query_res-> addTwoSyn(pattern_syn, left, res);
        } else if (query_res->isInTable(pattern_syn) && query_res->isInTable(left)) {
            return query_res->filterTwoSyn(pattern_syn, left, [&](string s, string v) {
                return PKB::getInstance()->isControlVar(stoi(s), v);
            });
        } else if (query_res->isInTable(pattern_syn) && !query_res->isInTable(left)){
            return query_res-> filterAndAdd(pattern_syn, left, [&](string s) {
                return PKB::getInstance()->getMatchedIfVar(stoi(s));
            });
        } else {
            return query_res-> filterAndAdd(left, pattern_syn, [&](string s) {
                return PKB::getInstance()->convertIntToString(PKB::getInstance()->getMatchedIf(s));
            });
        }
    }
}

bool PatternIf::equals(PatternClause *other) {
    PatternIf * p = (PatternIf *)other;
    return this->getClauseName() == p->getClauseName()
           && this->getLeftParam() ==p->getLeftParam()
           && this->getMidParam() == p->getMidParam()
           && this->getRightParam() == p->getRightParam()
           && this->getPatternSyn() == p->getPatternSyn();
}

string PatternIf::getMidParam() {
    return mid;
}

string PatternIf::getClauseName() {
    return "PatternIf";
}

int PatternIf::getRank() {
    return 9;
}
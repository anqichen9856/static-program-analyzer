#include "PatternAssign.h"

PatternAssign::PatternAssign(string pattern_syn, string left, string right) : PatternClause(pattern_syn, left, right) {
    this->left = left;
    this->right = right;
    this->pattern_syn = pattern_syn;
}

void PatternAssign::validate(unordered_map<string, DesignEntity> syn_entity_map) {
    bool var_syn = isSynonym(left) && syn_entity_map[left] == DesignEntity::Variable;
    if (!(var_syn || isName(left) || isWildcard(left))) {
        throw InvalidSemanticException("Invalid left argument in PatternWhile.");
    }
    //left: syn & variable, name, wildcard
    //right: wildcard, expr, partial expr
}

bool PatternAssign::evaluate(Query *query, QueryResult* query_res) {

    if (isWildcard(left) || isName(left)) {
        // a is the only synonym
        if (!query_res->isInTable(pattern_syn)) {
            auto res = PKB::getInstance()->convertIntToString(PKB::getInstance()->getMatchedStmt(strip(left), strip(right)));
            if (res.empty()) {
                return false;
            }
            return query_res->addOneSyn(pattern_syn, res);
        } else {
            return query_res->filterOneSyn(pattern_syn, [&](string s) {
                return PKB::getInstance()->isMatch(stoi(s), strip(left), strip(right));
            });
        }
    } else {
        // 2 synonyms: a, v
        if (!query_res->isInTable(pattern_syn) && !query_res->isInTable(left)) {
            auto res = PKB::getInstance()->getMatchedAssignPair(strip(right));
            if (res.empty()) {
                return false;
            }
            return query_res-> addTwoSyn(pattern_syn, left, res);
        } else if (query_res->isInTable(pattern_syn) && query_res->isInTable(left)) {
            return query_res->filterTwoSyn(pattern_syn, left, [&](string s, string v) {
                return PKB::getInstance()->isMatch(stoi(s), v, strip(right));
            });
        } else if (query_res->isInTable(pattern_syn) && !query_res->isInTable(left)){
            return query_res-> filterAndAdd(pattern_syn, left, [&](string s) {
                return PKB::getInstance()->getMatchedAssignVar(stoi(s), strip(right));
            });
        } else {
            return query_res-> filterAndAdd(left, pattern_syn, [&](string s) {
                return PKB::getInstance()->convertIntToString(PKB::getInstance()->getMatchedStmt(s, strip(right)));
            });
        }
    }
}

string PatternAssign::getClauseName() {
    return "PatternAssign";
}

int PatternAssign::getRank() {
    return 10;
}
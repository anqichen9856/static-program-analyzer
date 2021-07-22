#include "PatternWhile.h"

PatternWhile::PatternWhile(string pattern_syn, string left, string right) : PatternClause(pattern_syn, left, right) {
    this->left = left;
    this->right = right;
    this->pattern_syn = pattern_syn;
}

void PatternWhile::validate(unordered_map<string, DesignEntity> syn_entity_map) {
    bool var_syn = isSynonym(left) && syn_entity_map[left] == DesignEntity::Variable;
    if (!(var_syn || isName(left) || isWildcard(left))) {
        throw InvalidSemanticException("Invalid left argument in PatternWhile.");
    }
    if (!isWildcard(right)) {
        throw InvalidQueryException("Right argument in PatternWhile clause is not Wildcard.");
    }
    //left: syn & variable, name, wildcard
    //right: wildcard
}

bool PatternWhile::evaluate(Query *query, QueryResult* query_res) {
    auto syns = query->synonyms;

    if (isWildcard(left) || isName(left)) {
        // w is the only synonym
        if (!query_res->isInTable(pattern_syn)) {
            auto res = PKB::getInstance()->convertIntToString(PKB::getInstance()->getMatchedWhile(strip(left)));
            if (res.empty()) {
                return false;
            }
            return query_res->addOneSyn(pattern_syn, res);
        } else {
            return query_res->filterOneSyn(pattern_syn, [&](string s) {
                //return PKB::getInstance()->isUses(stoi(s), strip(left));
                return PKB::getInstance()->isControlVar(stoi(s), strip(left));
            });
        }
    } else {
        // 2 synonyms: w, v
        if (!query_res->isInTable(pattern_syn) && !query_res->isInTable(left)) {
            auto res = PKB::getInstance()->getMatchedWhilePair();
            if (res.empty()) {
                return false;
            }
            return query_res-> addTwoSyn(pattern_syn, left, res);
        } else if (query_res->isInTable(pattern_syn) && query_res->isInTable(left)) {
            return query_res->filterTwoSyn(pattern_syn, left, [&](string s, string v) {
                return PKB::getInstance()->isControlVar(stoi(s), v);
            });
        } else if (query_res->isInTable(pattern_syn) && !query_res->isInTable(left)) {
            return query_res-> filterAndAdd(pattern_syn, left, [&](string s) {
                return PKB::getInstance()->getMatchedWhileVar(stoi(s));
            });
        } else {
            return query_res-> filterAndAdd(left, pattern_syn, [&](string s) {
                return PKB::getInstance()->convertIntToString(PKB::getInstance()->getMatchedWhile(s));
            });
        }
    }
}

string PatternWhile::getClauseName() {
    return "PatternWhile";
}

int PatternWhile::getRank() {
    return 8;
}


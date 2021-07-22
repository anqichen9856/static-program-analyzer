#include "CallsStarClause.h"

CallsStarClause::CallsStarClause(string left, string right) : RelClause(left, right) {}

void CallsStarClause::validate(unordered_map<string, DesignEntity> map) {
    vector<string> params = {left, right};
    for (string p : params) {
        bool procedure = isSynonym(p) && isProcedure(map[p]);
        if (!(procedure || isName(p) || isWildcard(p))) {
            throw InvalidSemanticException("Invalid param type for Calls clause.");
        }
    }
    if (isSynonym(left) && isSynonym(right) && left == right) {
        throw InvalidSemanticException("Two parameters of Calls clause are the same synonym.");
    }
    //left & right: procedure, int, wildcard
}

bool CallsStarClause::evaluate(Query *query, QueryResult* query_res) {
    unordered_map<string, DesignEntity> syns = query->synonyms;
    bool result = false;

    if (isName(left) && isName(right)) {
        result = PKB::getInstance()->isCallsStar(strip(left), strip(right));
    } else if (isName(left) && isWildcard(right)) {
        vector<string> res = PKB::getInstance()->getCallsStar(strip(left));
        if (res.size() > 0) {
            result = true;
        }
    } else if (isName(left) && isSynonym(right)) {
        vector<string> res = PKB::getInstance()->getCallsStar(strip(left));
        if (query_res->isInTable(right)) {
            result = query_res->filterOneSyn(right, [&](string s) {
                return PKB::getInstance()->isInList(res, s);
            });
        }
        else {
            if (res.size() > 0) {
                result = query_res->addOneSyn(right, res);
            }
        }
    } else if (isWildcard(left) && isName(right)) {
        vector<string> res = PKB::getInstance()->getCalledByStar(strip(right));
        if (res.size() > 0) {
            result = true;
        }
    } else if (isWildcard(left) && isSynonym(right)) { 
        vector<string> syn_procedure = PKB::getInstance()->getAllMatchedEntity(syns[right]);
        vector<string> res;
        for (auto i : syn_procedure) {
            if (PKB::getInstance()->getCalledByStar(i).size() > 0) {
                res.push_back(i);
            }
        }
        if (query_res->isInTable(right)) {
            result = query_res->filterOneSyn(right, [&](string s) {
                return PKB::getInstance()->isInList(res, s);
            });
        }
        else {
            if (res.size() > 0) {
                result = query_res->addOneSyn(right, res);
            }
        }
    } else if (isWildcard(left) && isWildcard(right)) {
        //TODO change direct access to get statement
        if (PKB::getInstance()->calls_list.size() > 0) {
            result = true;
        }
    } else if (isSynonym(left) && isName(right)) {
        vector<string> res = PKB::getInstance()->getCalledByStar(strip(right));
        if (query_res->isInTable(left)) {
            result = query_res->filterOneSyn(left, [&](string s) {
                return PKB::getInstance()->isInList(res, s);
            });
        }
        else {
            if (res.size() > 0) {
                result = query_res->addOneSyn(left, res);
            }
        }
    } else if (isSynonym(left) && isWildcard(right)) {
        vector<string> syn_procedures = PKB::getInstance()->getAllMatchedEntity(syns[left]);
        vector<string> res;
        for (auto i : syn_procedures) {
            if (PKB::getInstance()->getCallsStar(i).size() > 0) {
                res.push_back(i);
            }
        }
        if (query_res->isInTable(left)) {
            result = query_res->filterOneSyn(left, [&](string s) {
                return PKB::getInstance()->isInList(res, s);
            });
        }
        else {
            if (res.size() > 0) {
                result = query_res->addOneSyn(left, res);
            }
        }
    } else if (isSynonym(left) && isSynonym(right)) {
        if (query_res->isInTable(left) && query_res->isInTable(right)) {
            result = query_res->filterTwoSyn(left, right, [&](string s1, string s2) {
                return PKB::getInstance()->isCallsStar(s1, s2);
            });
        }
        else if (query_res->isInTable(left) && !query_res->isInTable(right)) { 
            result = query_res->filterAndAdd(left, right, [&](string s) {
                vector<string> res = PKB::getInstance()->getCallsStar(s);
                return res;
            });
        }
        else if (!query_res->isInTable(left) && query_res->isInTable(right)) {
            result = query_res->filterAndAdd(right, left, [&](string s) {
                vector<string> res = PKB::getInstance()->getCalledByStar(s);
                return res;
            });
        }
        else { 
            vector<string> left_procedures = PKB::getInstance()->getAllMatchedEntity(syns[left]);
            vector<string> right_procedures = PKB::getInstance()->getAllMatchedEntity(syns[right]);
            vector<pair<string, string>> pairs;
            for (auto i : left_procedures) {
                for (auto j : right_procedures) {
                    if (PKB::getInstance()->isCallsStar(i, j)) {
                        pairs.push_back({ i, j });
                    }
                }
            }
            if (pairs.size() > 0) {
                result = query_res->addTwoSyn(left, right, pairs);
            }
        }
    } else {
    }
    return result;
}

string CallsStarClause::getClauseName() {
    return "Calls*";
}

int CallsStarClause::getRank() {
    return 11;
}

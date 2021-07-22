#include "MultiStatementRelClause.h"

MultiStatementRelClause::MultiStatementRelClause(string left, string right) : RelClause(left, right) {}

void MultiStatementRelClause::validate(unordered_map<string, DesignEntity> map) {
    validate(map);
}

bool MultiStatementRelClause::evaluate(Query *query, QueryResult* query_res) {
    unordered_map<string, DesignEntity> syns = query->synonyms;
    bool result = false;

    if (isInteger(left) && isInteger(right)) {
        result = isRel(stoi(left), stoi(right));
    } else if (isInteger(left) && isWildcard(right)) {
        vector<int> res = getRel(stoi(left));
        if (res.size() > 0) {
            result = true;
        }
    } else if (isInteger(left) && isSynonym(right)) {
        vector<int> temp = getRel(stoi(left));
        vector<string> res;
        if (temp.size() > 0) {
            for (auto i : temp) {
                DesignEntity type = PKB::getInstance()->getStmtType(i);
                if (type == syns[right] || syns[right] == DesignEntity::Stmt || syns[right] == DesignEntity::ProgLine) {
                    res.push_back(to_string(i));
                }
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
    } else if (isWildcard(left) && isInteger(right)) { 
        vector<int> res = getInvRel(stoi(right));
        if (res.size() > 0) {
            result = true;
        }
    } else if (isWildcard(left) && isSynonym(right)) {
        vector<string> temp = PKB::getInstance()->getAllMatchedEntity(syns[right]);
        vector<string> res;
        for (auto i : temp) {
            if (getInvRel(stoi(i)).size() > 0) {
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
        if (getRelListSize() > 0) {
            result = true;
        }
    } else if (isSynonym(left) && isInteger(right)) { 
        vector<int> matched = getInvRel(stoi(right));
        vector<string> res;
        if (matched.size() > 0) {
            for (auto i : matched) {
                DesignEntity type = PKB::getInstance()->getStmtType(i);
                if (type == syns[left] || syns[left] == DesignEntity::Stmt || syns[left] == DesignEntity::ProgLine) {
                    res.push_back(to_string(i));
                }
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
    } else if (isSynonym(left) && isWildcard(right)) {
        vector<string> temp = PKB::getInstance()->getAllMatchedEntity(syns[left]);
        vector<string> res;
        for (auto i : temp) {
            if (getRel(stoi(i)).size() > 0) {
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
        if (left == right) { // Next*
            return evalSynSyn(left, query_res, syns);
        }

        if (query_res->isInTable(left) && query_res->isInTable(right)) {
            result = query_res->filterTwoSyn(left, right, [&](string s1, string s2) {
                return isRel(stoi(s1),stoi(s2));
            });
        }
        else if (query_res->isInTable(left) && !query_res->isInTable(right)) { 
            DesignEntity right_type = syns[right];
            result = query_res->filterAndAdd(left, right, [&](string s) {
                vector<int> temp = getRel(stoi(s));
                vector<string> res;
                if (temp.size() > 0) {
                    for (auto i : temp) {
                        DesignEntity type = PKB::getInstance()->getStmtType(i);
                        if (type == right_type || syns[right] == DesignEntity::Stmt || syns[right] == DesignEntity::ProgLine) {
                            res.push_back(to_string(i));
                        }
                    }
                }
                return res;
            });
        }
        else if (!query_res->isInTable(left) && query_res->isInTable(right)) { 
            DesignEntity left_type = syns[left];
            result = query_res->filterAndAdd(right, left, [&](string s) {
                vector<int> temp = getInvRel(stoi(s));
                vector<string> res;
                if (temp.size() > 0) {
                    for (auto i : temp) {
                        DesignEntity type = PKB::getInstance()->getStmtType(i);
                        if (type == left_type || syns[left] == DesignEntity::Stmt || syns[left] == DesignEntity::ProgLine) {
                            res.push_back(to_string(i));
                        }
                    }
                }
                return res;
            });
        }
        else { 
            vector<string> left_stmts = PKB::getInstance()->getAllMatchedEntity(syns[left]);
            vector<string> right_stmts = PKB::getInstance()->getAllMatchedEntity(syns[right]);
            vector<pair<string, string>> pairs;
            for (auto i : left_stmts) {
                for (auto j : right_stmts) {
                    if (isRel(stoi(i), stoi(j))) {
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


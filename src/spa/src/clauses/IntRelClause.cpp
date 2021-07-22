#include "IntRelClause.h"

IntRelClause::IntRelClause(string left, string right) : RelClause(left, right) {}

void IntRelClause::validate(unordered_map<string, DesignEntity> map) {
    validate(map);
}

bool IntRelClause::evaluate(Query* query, QueryResult* query_res) {
    unordered_map<string, DesignEntity> syns = query->synonyms;
    bool result = false;

    if (isInteger(left) && isInteger(right)) {
        result = isIntRel(stoi(left), stoi(right));
    }
    else if (isInteger(left) && isWildcard(right)) {
        result = evalIntWild(left, right);
    }
    else if (isInteger(left) && isSynonym(right)) {
        result = evalIntSyn(left, right, query_res, syns);
    }
    else if (isWildcard(left) && isInteger(right)) {
        int res = getInvIntRel(stoi(right));
        if (res > 0) {
            result = true;
        }
    }
    else if (isWildcard(left) && isSynonym(right)) {
        vector<string> temp = PKB::getInstance()->getAllMatchedEntity(syns[right]);
        vector<string> res;
        for (auto i : temp) {
            if (getInvIntRel(stoi(i)) > 0) {
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
    }
    else if (isSynonym(left) && isInteger(right)) {
        result = evalSynInt(left, right, query_res, syns);
    }
    else if (isWildcard(left) && isWildcard(right)) {
        result = getIntRelListSize() > 0;
    }
    else if (isSynonym(left) && isWildcard(right)) {
        vector<string> temp = PKB::getInstance()->getAllMatchedEntity(syns[left]);
        vector<string> res;
        for (auto i : temp) {
            if (getIntRelSize(stoi(i)) > 0) {
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
    }
    else if (isSynonym(left) && isSynonym(right)) {
        if (query_res->isInTable(left) && query_res->isInTable(right)) {
            result = query_res->filterTwoSyn(left, right, [&](string s1, string s2) {
                return isIntRel(stoi(s1), stoi(s2));
            });
        }
        else if (query_res->isInTable(left) && !query_res->isInTable(right)) {
            result = evalSynSynHelper(left, right, query_res, syns);
        }
        else if (!query_res->isInTable(left) && query_res->isInTable(right)) {
            DesignEntity left_type = syns[left];
            result = query_res->filterAndAdd(right, left , [&](string s) {
                int res = getInvIntRel(stoi(s));
                if (res > 0) {
                    DesignEntity type = PKB::getInstance()->getStmtType(res);
                    if ((type == left_type || left_type == DesignEntity::Stmt || left_type == DesignEntity::ProgLine) && res > 0) {
                        return vector<string>{to_string(res)};
                    }
                }
                return vector<string>();
            });
        }
        else {
            vector<string> left_stmts = PKB::getInstance()->getAllMatchedEntity(syns[left]);
            vector<string> right_stmts = PKB::getInstance()->getAllMatchedEntity(syns[right]);
            vector<pair<string, string>> pairs;;
            for (auto i : left_stmts) {
                for (auto j : right_stmts) {
                    if (isIntRel(stoi(i), stoi(j))) {
                        pairs.push_back({ i, j });
                    }
                }
            }
            if (pairs.size() > 0) {
                result = query_res->addTwoSyn(left, right, pairs);
            }
        }
    }
    else {
    }
    return result;
}

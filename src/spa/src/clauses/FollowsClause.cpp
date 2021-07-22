#include "FollowsClause.h"

FollowsClause::FollowsClause(string left, string right) : IntRelClause(left, right) {}

void FollowsClause::validate(unordered_map<string, DesignEntity> map) {
    vector<string> params = {left, right};
    for (string p : params) {
        bool stmt = isSynonym(p) && isStmt(map[p]);
        if (!(stmt || isInteger(p) || isWildcard(p))) {
            throw InvalidSemanticException("Invalid param type for Follows clause.");
        }
    }
    if (isSynonym(left) && isSynonym(right) && left == right) {
        throw InvalidSemanticException("Two parameters of Follows clause are the same synonym.");
    }
    //left & right: stmt, int, wildcard
}

int FollowsClause::getIntRelSize(int statement) {
    return PKB::getInstance()->getFollows(statement);
}

int FollowsClause::getInvIntRel(int statement) {
    return PKB::getInstance()->getFollowedBy(statement);
}

bool FollowsClause::isIntRel(int before, int after) {
    return PKB::getInstance()->isFollows(before, after);
}

unsigned int FollowsClause::getIntRelListSize() {
    return PKB::getInstance()->follows_list.size();
}

bool FollowsClause::evalIntSyn(string left, string right, QueryResult* query_res, unordered_map<string, DesignEntity> syns) {
    int res = PKB::getInstance()->getFollows(stoi(left));
    if (query_res->isInTable(right)) {
        return query_res->filterOneSyn(right, [&](string s) {
            if (res > 0) {
                DesignEntity type = PKB::getInstance()->getStmtType(res);
                if (type == syns[right] || syns[right] == DesignEntity::Stmt || syns[right] == DesignEntity::ProgLine) {
                    if (to_string(res) == s) {
                        return true;
                    }
                }
            }
            return false;
        });
    }
    else {
        if (res > 0) {
            DesignEntity type = PKB::getInstance()->getStmtType(res);
            if (type == syns[right] || syns[right] == DesignEntity::Stmt || syns[right] == DesignEntity::ProgLine) {
                auto temp = vector<string>{ to_string(res) };
                return query_res->addOneSyn(right, temp);
            }
        }
        return false;
    }
}

bool FollowsClause::evalIntWild(string left, string right) {
    int res = PKB::getInstance()->getFollows(stoi(left));
    if (res > 0) {
        return true;
    }
    return false;
}

bool FollowsClause::evalSynInt(string left, string right, QueryResult* query_res, unordered_map<string, DesignEntity> syns) {
    int res = PKB::getInstance()->getFollowedBy(stoi(right));
    if (query_res->isInTable(left)) {
        return query_res->filterOneSyn(left, [&](string s) {
            if (res > 0) {
                DesignEntity type = PKB::getInstance()->getStmtType(res);
                if (type == syns[left] || syns[left] == DesignEntity::Stmt || syns[left] == DesignEntity::ProgLine) {
                    if (to_string(res) == s) {
                        return true;
                    }
                }
            }
            return false;
        });
    }
    else {
        if (res > 0) {
            DesignEntity type = PKB::getInstance()->getStmtType(res);
            if (type == syns[left] || syns[left] == DesignEntity::Stmt || syns[left] == DesignEntity::ProgLine) {
                auto temp = vector<string>{ to_string(res) };
                return query_res->addOneSyn(left, temp);
            }
        }
        return false;
    }
}

bool FollowsClause::evalSynSynHelper(string left, string right, QueryResult* query_res, unordered_map<string, DesignEntity> syns) {
    DesignEntity right_type = syns[right];
    return query_res->filterAndAdd(left, right, [&](string s) {
        int follows = PKB::getInstance()->getFollows(stoi(s));
        vector<string> res;
        if (follows > 0) {
            DesignEntity type = PKB::getInstance()->getStmtType(follows);
            if (type == right_type || right_type == DesignEntity::Stmt || right_type == DesignEntity::ProgLine) {
                res.push_back(to_string(follows));
            }
        }
        return res;
    });
}

string FollowsClause::getClauseName() {
    return "Follows";
}

int FollowsClause::getRank() {
    return 3;
}
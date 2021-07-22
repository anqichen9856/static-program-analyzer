#include "ParentClause.h"

ParentClause::ParentClause(string left, string right) : IntRelClause(left, right) {}

void ParentClause::validate(unordered_map<string, DesignEntity> map) {
    bool valid_left = (isSynonym(left) && (map[left] == DesignEntity::Stmt || map[left] == DesignEntity::If ||
                                           map[left] == DesignEntity::While || map[left] == DesignEntity::ProgLine)) ||
                      isInteger(left) || isWildcard(left);
    bool valid_right = (isSynonym(right) && isStmt(map[right])) ||
                       isInteger(right) || isWildcard(right);

    //left: syn & stmt/if/while, int, wildcard
    //right: syn & stmttype, int, wildcard

    if (!(valid_left && valid_right)) {
        throw InvalidSemanticException("Invalid param type for Parent clause.");
    }
    if (isSynonym(left) && isSynonym(right) && left == right) {
        throw InvalidSemanticException("Two parameters of Parent clause are the same synonym.");
    }
}

int ParentClause::getIntRelSize(int statement) {
    return PKB::getInstance()->getChildren(statement).size();
}

int ParentClause::getInvIntRel(int statement) {
    int result = PKB::getInstance()->getParent(statement);
    if (result <= 0) {
        return 0;
    }
    return result;
}

bool ParentClause::isIntRel(int before, int after) {
    return PKB::getInstance()->isParent(before, after);
}

unsigned int ParentClause::getIntRelListSize() {
    return PKB::getInstance()->parent_list.size();
}

bool ParentClause::evalIntSyn(string left, string right, QueryResult* query_res, unordered_map<string, DesignEntity> syns) {
    vector<int> res = PKB::getInstance()->getChildren(stoi(left));
    vector<string> temp;
    if (res.size() > 0) {
        for (auto i : res) {
            DesignEntity type = PKB::getInstance()->getStmtType(i);
            if (type == syns[right] || syns[right] == DesignEntity::Stmt || syns[right] == DesignEntity::ProgLine) {
                temp.push_back(to_string(i));
            }
        }
    }
    if (query_res->isInTable(right)) {
        return query_res->filterOneSyn(right, [&](string s) {
            return PKB::getInstance()->isInList(temp, s);
        });
    }
    else {
        if (temp.size() > 0) {
            return query_res->addOneSyn(right, temp);
        }
        return false;
    }
}

bool ParentClause::evalIntWild(string left, string right) {
    vector<int> res = PKB::getInstance()->getChildren(stoi(left));
    if (res.size() > 0) {
        return true;
    }
    return false;
}

bool ParentClause::evalSynInt(string left, string right, QueryResult* query_res, unordered_map<string, DesignEntity> syns) {
    int res = PKB::getInstance()->getParent(stoi(right));
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

bool ParentClause::evalSynSynHelper(string left, string right, QueryResult* query_res, unordered_map<string, DesignEntity> syns) {
    DesignEntity right_type = syns[right];
    return query_res->filterAndAdd(left, right, [&](string s) {
        vector<int> children = PKB::getInstance()->getChildren(stoi(s));
        vector<string> res;
        if (children.size() > 0) {
            for (auto child : children) {
                DesignEntity type = PKB::getInstance()->getStmtType(child);
                if (type == right_type || right_type == DesignEntity::Stmt || right_type == DesignEntity::ProgLine) {
                    res.push_back(to_string(child));
                }
            }
        }
        return res;
    });
}

string ParentClause::getClauseName() {
    return "Parent";
}

int ParentClause::getRank() {
    return 5;
}

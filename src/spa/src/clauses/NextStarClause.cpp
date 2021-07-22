#include "NextStarClause.h"

NextStarClause::NextStarClause(string left, string right) : MultiStatementRelClause(left, right) {}

void NextStarClause::validate(unordered_map<string, DesignEntity> map) {
    vector<string> params = { left, right };
    for (string p : params) {
        bool stmt = isSynonym(p) && isStmt(map[p]);
        if (!(stmt || isInteger(p) || isWildcard(p))) {
            throw InvalidSemanticException("Invalid param type for NextStar clause.");
        }
    }
    //left & right: stmt, int, wildcard
}

vector<int> NextStarClause::getRel(int statement) {
    return PKB::getInstance()->getNextStar(statement);
}

vector<int> NextStarClause::getInvRel(int statement) {
    return PKB::getInstance()->getPreviousStar(statement);
}

bool NextStarClause::isRel(int before, int after) {
    return PKB::getInstance()->isNextStar(before, after);
}

unsigned int NextStarClause::getRelListSize() {
    return PKB::getInstance()->next_list.size();
}

string NextStarClause::getClauseName() {
    return "Next*";
}

bool NextStarClause::evalSynSyn(string left, QueryResult* query_res, unordered_map<string, DesignEntity> syns) {
    vector<string> left_stmts = PKB::getInstance()->getAllMatchedEntity(syns[left]);
    unordered_set<string> res_set;
    vector<string> res;
    for (auto i : left_stmts) {
        if (isRel(stoi(i), stoi(i))) {
            res_set.insert(i);
            }
    }
    for (auto i : res_set) {
        res.push_back(i);
    }
    if (query_res->isInTable(left)) {
        return query_res->filterOneSyn(left, [&](string s1) {
            return PKB::getInstance()->isInList(res, s1);
        });
    }
    else {
        if (res.size() > 0) {
            return query_res->addOneSyn(left, res);
        }
        return false;
    }
}

int NextStarClause::getRank() {
    return 14;
}

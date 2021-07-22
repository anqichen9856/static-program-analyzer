#include "FollowsStarClause.h"

FollowsStarClause::FollowsStarClause(string left, string right) : MultiStatementRelClause(left, right) {}

void FollowsStarClause::validate(unordered_map<string, DesignEntity> map) {
    vector<string> params = { left, right };
    for (string p : params) {
        bool stmt = isSynonym(p) && isStmt(map[p]);
        if (!(stmt || isInteger(p) || isWildcard(p))) {
            throw InvalidSemanticException("Invalid param type for FollowsStar clause.");
        }
        if (isSynonym(left) && isSynonym(right) && left == right) {
            throw InvalidSemanticException("Two parameters of FollowsStar clause are the same synonym.");
        }
    }
    //left & right: stmt, int, wildcard
}

vector<int> FollowsStarClause::getRel(int statement) {
    return PKB::getInstance()->getFollowsStar(statement);
}

vector<int> FollowsStarClause::getInvRel(int statement) {
    return PKB::getInstance()->getFollowedByStar(statement);
}

bool FollowsStarClause::isRel(int before, int after) {
    return PKB::getInstance()->isFollowsStar(before, after);
}

unsigned int FollowsStarClause::getRelListSize() {
    return PKB::getInstance()->follows_star_list.size();
}

string FollowsStarClause::getClauseName() {
    return "Follows*";
}

bool FollowsStarClause::evalSynSyn(string left, QueryResult* query_res, unordered_map<string, DesignEntity> syns) {
    return false;
}

int FollowsStarClause::getRank() {
    return 13;
}

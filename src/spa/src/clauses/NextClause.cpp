#include "NextClause.h"

vector<int> NextClause::getRel(int statement) {
    return PKB::getInstance()->getNextStatements(statement);
}

vector<int> NextClause::getInvRel(int statement) {
    return PKB::getInstance()->getPreviousStatements(statement);
}

bool NextClause::isRel(int before, int after) {
    return PKB::getInstance()->isNextStatement(before, after);
}

unsigned int NextClause::getRelListSize() {
    return PKB::getInstance()->next_list.size();
}

NextClause::NextClause(string left, string right) : MultiStatementRelClause(left, right) {}

string NextClause::getClauseName() {
    return "Next";
}

void NextClause::validate(unordered_map<string, DesignEntity> map) {
    vector<string> params = { left, right };
    for (string p : params) {
        bool stmt = isSynonym(p) && isStmt(map[p]);
        if (!(stmt || isInteger(p) || isWildcard(p))) {
            throw InvalidSemanticException("Invalid param type for Next clause.");
        }
        if (isSynonym(left) && isSynonym(right) && left == right) {
            throw InvalidSemanticException("Two parameters of Next clause are the same synonym.");
        }
    }
    //left & right: stmt, int, wildcard
}

bool NextClause::evalSynSyn(string left, QueryResult* query_res, unordered_map<string, DesignEntity> syns) {
    return false;
}

int NextClause::getRank() {
    return 4;
}

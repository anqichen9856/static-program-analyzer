#include "ParentStarClause.h"

ParentStarClause::ParentStarClause(string left, string right) : MultiStatementRelClause(left, right) {}

void ParentStarClause::validate(unordered_map<string, DesignEntity> map) {
    bool valid_left = (isSynonym(left) && (map[left] == DesignEntity::Stmt || map[left] == DesignEntity::If ||
                                           map[left] == DesignEntity::While || map[left] == DesignEntity::ProgLine)) ||
                      isInteger(left) || isWildcard(left);
    bool valid_right = (isSynonym(right) && isStmt(map[right])) ||
                       isInteger(right) || isWildcard(right);

    //left: syn & stmt/if/while, int, wildcard
    //right: syn & stmttype, int, wildcard

    if (!(valid_left && valid_right)) {
        throw InvalidSemanticException("Invalid param type for ParentStar clause.");
    }
    if (isSynonym(left) && isSynonym(right) && left == right) {
        throw InvalidSemanticException("Two parameters of ParentStar clause are the same synonym.");
    }
}

vector<int> ParentStarClause::getRel(int statement) {
    return PKB::getInstance()->getChildrenStar(statement);
}

vector<int> ParentStarClause::getInvRel(int statement) {
    return PKB::getInstance()->getParentStar(statement);
}

bool ParentStarClause::isRel(int before, int after) {
    return PKB::getInstance()->isParentStar(before, after);
}

unsigned int ParentStarClause::getRelListSize() {
    return PKB::getInstance()->parent_star_list.size();
}

string ParentStarClause::getClauseName() {
    return "Parent*";
}

bool ParentStarClause::evalSynSyn(string left, QueryResult* query_res, unordered_map<string, DesignEntity> syns) {
    return false;
}

int ParentStarClause::getRank() {
    return 12;
}

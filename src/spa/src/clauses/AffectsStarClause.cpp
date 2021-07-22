#include "AffectsStarClause.h"

AffectsStarClause::AffectsStarClause(string left, string right) : NextStarClause(left, right) {}

void AffectsStarClause::validate(unordered_map<string, DesignEntity> map) {
    vector<string> params = { left, right };
    for (string p : params) {
        bool isAssign = isSynonym(p) && map[p] == DesignEntity::Assign;
        bool isStmt = isSynonym(p) && map[p] == DesignEntity::Stmt;
        bool isProgLine = isSynonym(p) && map[p] == DesignEntity::ProgLine;
        if (!(isAssign || isStmt || isProgLine || isInteger(p) || isWildcard(p))) {
            throw InvalidSemanticException("Invalid param type for Affects clause.");
        }
    }
    //left & right: stmt, prog_line, assign, int, wildcard
}

vector<int> AffectsStarClause::getRel(int statement) {
    return PKB::getInstance()->getAffectsStar(statement);
}

vector<int> AffectsStarClause::getInvRel(int statement) {
    return PKB::getInstance()->getAffectedStar(statement);
}

bool AffectsStarClause::isRel(int affecting, int affected) {
    return PKB::getInstance()->isAffectsStar(affecting, affected);
}

unsigned int AffectsStarClause::getRelListSize() {
    vector<int> assign_statements = PKB::getInstance()->getAllAssign();
    for (int assign : assign_statements) {
        if (PKB::getInstance()->getAffectsStar(assign).size()) {
            //Yeet when exists, all that's required based on current code.
            return 1;
        }
    }
    return 0;
}

string AffectsStarClause::getClauseName() {
    return "Affects*";
}

int AffectsStarClause::getRank() {
    return 16;
}


#include "AffectsClause.h"

AffectsClause::AffectsClause(string left, string right) : NextStarClause(left, right) {}

void AffectsClause::validate(unordered_map<string, DesignEntity> map) {
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

vector<int> AffectsClause::getRel(int statement) {
    return PKB::getInstance()->getAffects(statement);
}

vector<int> AffectsClause::getInvRel(int statement) {
    return PKB::getInstance()->getAffected(statement);
}

bool AffectsClause::isRel(int before, int after) {
    return PKB::getInstance()->isAffects(before, after);
}

unsigned int AffectsClause::getRelListSize() {
    vector<int> assign_statements = PKB::getInstance()->getAllAssign();
    for (int assign : assign_statements) {
        if (PKB::getInstance()->getAffects(assign).size()) {
            //Yeet when exists, all that's required based on current code.
            return 1;
        }
    }
    return 0;
}

string AffectsClause::getClauseName() {
    return "Affects";
}

int AffectsClause::getRank() {
    return 15;
}


#include "UsesClause.h"

UsesClause::UsesClause(string left, string right) : NamesRelClause(left, right) {}

void UsesClause::validate(unordered_map<string, DesignEntity> map) {
    bool valid_left = isName(left) ||
        (isSynonym(left) && isStmt(map[left]) && map[left] != DesignEntity::Read) ||
        (isSynonym(left) && map[left] == DesignEntity::Procedure) ||
        isInteger(left);
    bool valid_right = isWildcard(right) ||
        (isSynonym(right) && map[right] == DesignEntity::Variable) ||
        isName(right);

    //left: procedure name, syn & stmttype except read, int, syn & procedure
    //right: wildcard, syn & variable, name

    if (!(valid_left && valid_right)) {
        throw InvalidSemanticException("Invalid param type for Uses clause.");
    }
}

bool UsesClause::isNamesRel(int s, string v) {
	return PKB::getInstance()->isUses(s, v);
}

bool UsesClause::isNamesRel(string p, string v) {
	return PKB::getInstance()->isUses(p, v);
}

vector<string> UsesClause::getAllNamesRel(int s) {
	return PKB::getInstance()->getAllVariablesUsedBy(s);
}

vector<string> UsesClause::getAllNamesRel(string p) {
	return PKB::getInstance()->getAllVariablesUsedBy(p);
}

string UsesClause::getClauseName() {
    return "Uses";
}

int UsesClause::getRank() {
    return 7;
}

#include "ModifiesClause.h"

ModifiesClause::ModifiesClause(string left, string right) : NamesRelClause(left, right) {}

void ModifiesClause::validate(unordered_map<string, DesignEntity> map) {
    bool valid_left = isName(left) ||
                      (isSynonym(left) && isStmt(map[left]) && map[left] != DesignEntity::Print) ||
                        (isSynonym(left) && map[left] == DesignEntity::Procedure)||
                      isInteger(left);
    bool valid_right = isWildcard(right) ||
                       (isSynonym(right) && map[right] == DesignEntity::Variable) ||
                       isName(right);

    //left: procedure name, syn & stmttype except print, int, syn & procedure
    //right: wildcard, syn & variable, name,

    if (!(valid_left && valid_right)) {
        throw InvalidSemanticException("Invalid param type for Modifies clause.");
    }
}

bool ModifiesClause::isNamesRel(int s, string v) {
    return PKB::getInstance()->isModifies(s, v);
}

bool ModifiesClause::isNamesRel(string p, string v) {
    return PKB::getInstance()->isModifies(p, v);
}

vector<string> ModifiesClause::getAllNamesRel(int s) {
    return PKB::getInstance()->getAllVariablesModifiedBy(s);
}

vector<string> ModifiesClause::getAllNamesRel(string p) {
    return PKB::getInstance()->getAllVariablesModifiedBy(p);
}

string ModifiesClause::getClauseName() {
    return "Modifies";
}

int ModifiesClause::getRank() {
    return 6;
}

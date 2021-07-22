#include "Clause.h"

Clause::Clause(string left, string right) {
    this->left = left;
    this->right = right;
}

string Clause::strip(string str) {

    if (str.length() == 1) {
        return str;
    }

    if (str.at(0) == '"') {
        str.pop_back();
        str.erase(str.begin());
    }

    if (str.at(0) == '_' && str.at(1) == '"') {
        str.pop_back();
        str.erase(str.begin());
        str.pop_back();
        str.erase(str.begin());
        str = "_" + str + "_";
    }

    return str;
}

void Clause::validate(unordered_map<string, DesignEntity> map) {
}

bool Clause::evaluate(Query* query, QueryResult* query_res) {return true;}

string Clause::getLeftParam() {
    return left;
}

string Clause::getRightParam() {
    return right;
}

bool Clause::isSynonym(string s) {
    regex reg(REGEX_SYNONYM);
    return regex_match(s, reg);
}

bool Clause::isWildcard(string s) {
    regex reg(REGEX_WILDCARD);
    return regex_match(s, reg);
}

bool Clause::isInteger(string s) {
    regex reg(REGEX_INTEGER);
    return regex_match(s, reg);
}

bool Clause::isName(string s) {
    regex reg(REGEX_NAME);
    return regex_match(s, reg);
}

bool Clause::isStmt(DesignEntity e) {
    return e == DesignEntity::Stmt || e == DesignEntity::If || e == DesignEntity::While ||
           e == DesignEntity::Assign || e == DesignEntity::Read || e == DesignEntity::Print ||
           e == DesignEntity::Call || e == DesignEntity::ProgLine;
}

bool Clause::isProcedure(DesignEntity e) {
    return e == DesignEntity::Procedure;
}

bool Clause::isPartialExpr(string s) {
    string r = R"(_\"\s*[A-Za-z][A-Za-z0-9]*\s*\"_)";
    regex reg(r);
    return s[0] == '_' && s[s.size() - 1] == '_';
    return regex_match(s, reg);
}

bool Clause::isConstant(string s) {
    string r = R"(\"[0-9]+\")";
    regex reg(r);
    return regex_match(s, reg);
}

bool Clause::isSelectedSyn(string to_be_compared, string selected) {
    return selected.compare(to_be_compared) == 0;
}

vector<string> Clause::getResultsAccordingToBool(bool res, DesignEntity type) {
    if (!res) {
        return vector<string>();
    }
    return PKB::getInstance()->getAllMatchedEntity(type);
}

vector<string> Clause::findIntersect(vector<string> &res, vector<string> &temp) {
    sort(res.begin(), res.end());
    sort(temp.begin(), temp.end());
    vector<string> intersect;
    set_intersection(res.begin(), res.end(), temp.begin(), temp.end(), back_inserter(intersect));
    return intersect;
}

vector<pair<string, string>> Clause::findPairIntersect(vector<pair<string, string>> v1,
                                                       vector<pair<string, string>> v2) {
    sort(v1.begin(), v1.end());
    sort(v2.begin(), v2.end());
    vector<pair<string, string>> intersect;
    set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(), back_inserter(intersect));
    return intersect;
}

bool Clause::equals(Clause *c) {
    return this->getClauseName() == c->getClauseName() && this->left == c->getLeftParam() && this->right == c->getRightParam();
}

string Clause::getClauseName() {
    return string();
}

string Clause::constructPairString(string left, string right) {
    return left + "," + right;
}

pair<string, string> Clause::splitPairString(string pairString) {
    vector<string> parts = QueryParser::tokenizeBy(pairString, ",");
    return make_pair(parts[0], parts[1]);
}

int Clause::getNumOfSyn() {
    int count = 0;
    if (isSynonym(left)) {
        count++;
    }
    if (isSynonym(right) && left!=right) {
        count++;
    }
    return count;
}

set<string> Clause::getSynonyms() {
    set<string> syns = {};
    if (isSynonym(left)) {
        syns.insert(left);
    }
    if (isSynonym(right)) {
        syns.insert(right);
    }
    return syns;
}

int Clause::getRank() {
    return 0;
}



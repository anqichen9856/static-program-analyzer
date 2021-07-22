#include "RelClause.h"

RelClause::RelClause(string left, string right) : Clause(left, right) {}

void RelClause::insertPair(unordered_map<string,
    vector<pair<string, string>>> &common_res, string left, string right, string i, string j) {
    if (common_res.size() > 0) {
        pair<string, string> pair = make_pair(i, j);
        string key = constructPairString(left, right);
        common_res[key].push_back(pair);
    }
}

void RelClause::insertPairs(unordered_map<string,
    vector<pair<string, string>>> &common_res, string left, string right, string i, vector<string> vars) {
    if (common_res.size() > 0) {
        string key = constructPairString(left, right);
        for (auto j : vars) {
            pair<string, string> pair = make_pair(i, j);
            common_res[key].push_back(pair);
        }
    }
}

bool RelClause::evaluate(Query *query, QueryResult *query_res) {
    return true;
}

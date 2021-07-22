#ifndef AUTOTESTER_QUERYRESULT_H
#define AUTOTESTER_QUERYRESULT_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <functional>
#include "utils/SPAEnums.h"
#include "exceptions/SPAException.h"
#include <vector>
#include <iostream>


using namespace std;

class QueryResult {
public:
    QueryResult(unordered_map<string, int> cols, list<vector<string>> rows);
    static QueryResult *init();
    bool isInTable(string s);
    bool addOneSyn(string syn, vector<string> &result);
    bool addTwoSyn(string syn_1, string syn_2, vector<pair<string, string>> &results);
    bool filterOneSyn(string syn, function<bool (string)> func);
    bool filterTwoSyn(string syn_1, string syn_2, function<bool (string, string)> func);
    bool addAtrribute(string syn, string attribute, function<string(string)> func);
    bool filterAndAdd(string syn_to_match, string syn_to_add, function<vector<string>(string)> func);

    unordered_map<string, int> cols; //col name (syn), col id
    list<vector<string>> rows; //list of rows, each row = vector of results
    vector<unordered_map<string, int>> past_cols;
    vector<list<vector<string>>> past_rows;
    unordered_map<string, int> syns_table_map;
    vector<string> getResults(vector<string> syns);
    bool clear();
    void createNewTable();

private:

    void crossProduct(list<vector<string>> other, unordered_map<string, int> mapped_cols);

    void matchAndAdd(string syn_1, string syn_2, unordered_map<string, vector<string>> results, bool first_is_mapped);

    void prefilterResults(vector<string> syns);

    void prefilterTable(vector<int> syns, int table_index);
};


#endif //AUTOTESTER_QUERYRESULT_H

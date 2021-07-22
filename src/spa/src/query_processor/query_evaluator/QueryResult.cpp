#include <algorithm>
#include "QueryResult.h"

QueryResult::QueryResult(unordered_map<string, int> cols, list<vector<string>> rows) {
    this->cols = move(cols);
    this->rows = move(rows);
    for(auto syn : cols) {
        syns_table_map.insert({syn.first, 0});
    }
}

QueryResult* QueryResult::init() {
    return new QueryResult(unordered_map<string, int>(),list<vector<string>>());
}

bool QueryResult::isInTable(string s) {
    for (auto table : past_cols) {
        if (table.find(s) != table.end()) {
            return true;
        }
    }
    return cols.find(s) != cols.end();
}

void QueryResult::crossProduct(list<vector<string>>  other, unordered_map<string, int> mapped_cols) {
    int col_size = cols.size();
    for (pair<string, int> mapped_pair : mapped_cols) {
        cols.insert({mapped_pair.first, mapped_pair.second + col_size});
        syns_table_map.insert({mapped_pair.first, past_cols.size()});

    }
    if (rows.empty()) {
        rows = other;
    } else {
        list<vector<string>> updated_rows;
        for (vector<string> current_row : rows) {
            for (vector<string> to_add : other) {
                vector<string> updated_row(current_row);
                updated_row.insert(updated_row.end(), to_add.begin(), to_add.end());
                updated_rows.push_back(updated_row);
            }
        }


        rows.clear();
        rows.insert(rows.begin(), updated_rows.begin(), updated_rows.end());
    }

}

bool QueryResult::addOneSyn(string syn, vector<string> &result) {
    if (isInTable(syn)) {
        throw QueryResultException("Syn: \"" + syn + "\" is already in the table");
    }
    list<vector<string>> prep_result;
    for (string indiv_result : result) {
        prep_result.push_back(vector<string>({indiv_result}));
    }
    this->crossProduct(prep_result, unordered_map<string, int>({{syn, 0}}));
    return !rows.empty();
}

bool QueryResult::addAtrribute(string syn, string attribute, function<string(string)> func) {
    if (cols.find(syn) == cols.end()) {
        for (int i = 0; i < past_cols.size(); i++) {
            if (past_cols[i].find(syn) != past_cols[i].end()) {
                auto &temp_cols = past_cols[i];
                auto &temp_rows = past_rows[i];
                if (temp_cols.find(syn+"."+attribute) != temp_cols.end()) {
                    throw QueryResultException(syn = "." + attribute + " already in table");
                }
                temp_cols.insert({syn + "." + attribute, temp_cols.size()});
                syns_table_map.insert({syn, past_cols.size()});
                list<vector<string>> new_rows;
                for (auto row : temp_rows) {
                    row.push_back(func(row[temp_cols[syn]]));
                    new_rows.push_back(row);
                }
                temp_rows.clear();
                temp_rows.insert(temp_rows.begin(), new_rows.begin(), new_rows.end());
                return !temp_rows.empty();
            }
        }
    } else {
        auto &temp_cols = cols;
        auto &temp_rows = rows;
        if (temp_cols.find(syn+"."+attribute) != temp_cols.end()) {
            throw QueryResultException(syn = "." + attribute + " already in table");
        }
        temp_cols.insert({syn + "." + attribute, temp_cols.size()});
        syns_table_map.insert({syn + "." + attribute, past_cols.size()});
        list<vector<string>> new_rows;
        for (auto row : temp_rows) {
            row.push_back(func(row[temp_cols[syn]]));
            new_rows.push_back(row);
        }
        temp_rows.clear();
        temp_rows.insert(temp_rows.begin(), new_rows.begin(), new_rows.end());
        return !temp_rows.empty();
    }
}

bool QueryResult::addTwoSyn(string syn_1, string syn_2, vector<pair<string, string>> &results) {
    if (isInTable(syn_1) && isInTable(syn_2)) {
        throw QueryResultException("Syns: \"" + syn_1 + "\" and \"" + syn_2 + "\" are both in the table");
    }
    else if (!isInTable(syn_1)  && !isInTable(syn_2)) {
        list<vector<string>> prep_result;
        for (pair<string, string>  indiv_result : results) {
            prep_result.push_back(vector<string>({indiv_result.first, indiv_result.second}));
        }
        if (syn_1 == syn_2) {
            list<vector<string>> second_prep;
            for (pair<string, string>  indiv_result : results) {
                second_prep.push_back(vector<string>({indiv_result.first}));
            }
            this->crossProduct(second_prep, unordered_map<string, int>({{syn_1, 0}}));
        } else {
            this->crossProduct(prep_result, unordered_map<string, int>({{syn_1, 0}, {syn_2, 1}}));
        }
    }
    else if (isInTable(syn_1)) {
        throw QueryResultException("Syns: \"" + syn_1 + "is in the table");
    }
    else {
        throw QueryResultException("Syns: \"" + syn_2 + "is in the table");
    }
    return !rows.empty();
}

void QueryResult::matchAndAdd(string syn_1, string syn_2, unordered_map<string,vector<string>> results, bool first_is_mapped) {
    list<vector<string>> updated_rows;
    if (first_is_mapped) {
        cols.insert({syn_2, cols.size()});
        syns_table_map.insert({syn_2, past_cols.size()});
    }
    else if (!first_is_mapped) {
        cols.insert({syn_1, cols.size()});
        syns_table_map.insert({syn_1, past_cols.size()});

    }

    for (vector<string> row : rows) {
        vector<string> mapped_values =  results[row[cols[syn_1]]];
        for (auto  mapped_value : mapped_values) {
            if (first_is_mapped) {
                    vector<string> updated_row = vector<string>(row);
                    updated_row.push_back(mapped_value);
                    updated_rows.push_back(updated_row);
            }
            else if (!first_is_mapped) {
                    vector<string> updated_row = vector<string>(row);
                    updated_row.push_back(mapped_value);
                    updated_rows.push_back(updated_row);
            }
        }
    }
    rows.clear();
    rows.insert(rows.begin(), updated_rows.begin(), updated_rows.end());
}

bool QueryResult::filterAndAdd(string syn_to_match, string syn_to_add, function<vector<string>(string)>  func) {
    if (!isInTable(syn_to_match)) {
        throw QueryResultException("Syn: \"" + syn_to_match + "\" is not in table");
    }
    unordered_map<string,vector<string>> result;
    unordered_set<string> evaluated;
    for (vector<string> row : rows) {
        string syn_val = row[cols[syn_to_match]];
        if(evaluated.find(syn_val) == evaluated.end()) {
            evaluated.insert(syn_val);
            vector<string> func_ret = func(syn_val);
            for (string val : func_ret) {
                result[syn_val].push_back(val);
            }
        }
    }
    matchAndAdd(syn_to_match, syn_to_add, result, true);
    return !rows.empty();
}

bool QueryResult::filterOneSyn(string syn, function<bool(string )>  func) {
    if (!isInTable(syn)) {
        throw QueryResultException("Syn: \"" + syn + "\" is not in table");
    }
    int syn_index = cols[syn];
    rows.remove_if([func , syn_index](vector<string> item) {return !func(item[syn_index]);});
    return !rows.empty();
}

bool QueryResult::filterTwoSyn(string syn_1, string syn_2, function<bool(string ,
        string )>  func) {
    if (!isInTable(syn_1) || !isInTable(syn_1)) {
        throw QueryResultException("Syn: \"" + syn_1 + "\" and/or \"" + syn_2 +  " \" are not in table");
    }
    int syn_1_index = cols[syn_1];
    int syn_2_index = cols[syn_2];
    rows.remove_if([func , syn_1_index, syn_2_index](vector<string> item) {return !func(item[syn_1_index], item[syn_2_index]);});
    return !rows.empty();
}

vector<string> QueryResult::getResults(vector<string> syns) {
    prefilterResults(syns);
    for (int i = 0; i < past_cols.size(); i++) {
        if (past_rows[i].size() != 0) {
            crossProduct(past_rows[i], past_cols[i]);
        }
    }
    vector<string> result = vector<string>();
    for (vector<string> row : rows) {
        string to_add;
        for (string  syn : syns) {
            to_add += row[cols[syn]] +" ";
        }
        to_add.pop_back();
        result.push_back(to_add);
    }
    sort(result.begin(), result.end());
    result.erase(unique(result.begin(), result.end()), result.end());
    return result;
}

bool QueryResult::clear() {
    rows.clear();
    cols.clear();
    past_cols.clear();
    past_rows.clear();
    return true;
}

void QueryResult::createNewTable() {
    past_cols.push_back(unordered_map<string, int>(cols));
    past_rows.push_back(list<vector<string>>(rows));
    rows.clear();
    cols.clear();
}

void QueryResult::prefilterResults(vector<string> syns) {
    createNewTable();
    for (int i = 0; i < past_cols.size(); i++) {
        vector<int> syns_index;
        for (string syn : syns) {
            if (syns_table_map[syn] == i) {
                syns_index.push_back(past_cols[i][syn]);
            }
        }
        sort(syns_index.begin(), syns_index.end());
        prefilterTable(syns_index, i);
    }
}

void QueryResult::prefilterTable(vector<int> syns, int table_index) {
    auto sort_rule_lambda = [syns](vector<string> &a, vector<string> &b) -> bool {
        for (int i : syns) {
            if (a[i] == b[i]) {
                continue;
            } else {
                return a[i] < b[i];
            }
        }
        return true;
    };

    past_rows[table_index].sort(sort_rule_lambda);
    past_rows[table_index].erase(unique(past_rows[table_index].begin(),
                                        past_rows[table_index].end(),
                                        [syns](vector<string> &a, vector<string> &b) -> bool {
        for (int i : syns) {
            if (a[i] == b[i]) {
                continue;
            } else {
                return false;
            }
        }
        return true;
    }), past_rows[table_index].end());
}

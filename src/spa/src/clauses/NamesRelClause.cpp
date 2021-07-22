#include "NamesRelClause.h"

NamesRelClause::NamesRelClause(string left, string right) : RelClause(left, right) {}

void NamesRelClause::validate(unordered_map<string, DesignEntity> map) {
    validate(map);
}

bool NamesRelClause::evaluate(Query* query, QueryResult* query_res) {
    unordered_map<string, DesignEntity> syns = query->synonyms;
    bool result = false;

    if (isInteger(left) && isName(right)) {
        result = isNamesRel(stoi(left), strip(right));
    }
    else if (isInteger(left) && isWildcard(right)) { 
        vector<string> res = getAllNamesRel(stoi(left));
        if (res.size() > 0) {
            result = true;
        }
    }
    else if (isName(left) && isWildcard(right)) {
        vector<string> res = getAllNamesRel(strip(left));
        if (res.size() > 0) {
            result = true;
        }
    }
    else if (isInteger(left) && isSynonym(right)) {
        vector<string> res = getAllNamesRel(stoi(left));
        if (query_res->isInTable(right)) {
            result = query_res->filterOneSyn(right, [&](string s) {
                return PKB::getInstance()->isInList(res, s);
            });
        }
        else {
            if (res.size() > 0) {
                result = query_res->addOneSyn(right, res);
            }
        }
    }
    else if (isSynonym(left) && isSynonym(right)) {
        if (query_res->isInTable(left) && query_res->isInTable(right)) {
            result = query_res->filterTwoSyn(left, right, [&](string s1, string s2) {
                if (syns[left] == DesignEntity::Procedure) {
                    return isNamesRel(s1, s2);
                }
                else {
                    return isNamesRel(stoi(s1), s2);
                }
            });
        }
        else if (query_res->isInTable(left) && !query_res->isInTable(right)) {
            result = query_res->filterAndAdd(left, right, [&](string s) {
                vector<string> temp;
                if (syns[left] == DesignEntity::Procedure) {
                    temp = getAllNamesRel(s);
                }
                else {
                    temp = getAllNamesRel(stoi(s));
                }
                vector<string> res;
                for (auto i : temp) {
                    res.push_back(i);
                }
                return res;
            });
        }
        else if (!query_res->isInTable(left) && query_res->isInTable(right)) {
            vector<string> left_res = PKB::getInstance()->getAllMatchedEntity(syns[left]);
            bool isProc = false;
            if (syns[left] == DesignEntity::Procedure) isProc = true;
            result = query_res->filterAndAdd(right, left, [&](string s) {
                vector<string> res;
                if (isProc) {
                    for (auto i : left_res) {
                        if (isNamesRel(i, s)) {
                            res.push_back(i);
                        }
                    }
                }
                else {
                    for (auto i : left_res) {
                        if (isNamesRel(stoi(i), s)) {
                            res.push_back(i);
                        }
                    }
                }
                return res;
            });
        }
        else {
            vector<string> matched = PKB::getInstance()->getAllMatchedEntity(syns[left]);
            vector<pair<string, string>> pairs;
            if (syns[left] == DesignEntity::Procedure) {
                for (auto i : matched) {
                    vector<string> required_variables = getAllNamesRel(i);
                    if (required_variables.size() > 0) {
                        for (auto var : required_variables) {
                            pairs.push_back({ i, var });
                        }
                    }
                }
            }
            else {
                for (auto i : matched) {
                    vector<string> required_variables = getAllNamesRel(stoi(i));
                    if (required_variables.size() > 0) {
                        for (auto var : required_variables) {
                            pairs.push_back({ i, var });
                        }
                    }
                }
            }
            if (pairs.size() > 0) {
                result = query_res->addTwoSyn(left, right, pairs);
            }
        }
    }
    else if (isName(left), isSynonym(right)) { 
        vector<string> res = getAllNamesRel(strip(left));
        if (query_res->isInTable(right)) {
            result = query_res->filterOneSyn(right, [&](string s) {
                return PKB::getInstance()->isInList(res, s);
            });
        }
        else {
            if (res.size() > 0) {
                result = query_res->addOneSyn(right, res);
            }
        }
    }
    else if (isName(left) && isName(right)) {
        result = isNamesRel(strip(left), strip(right));
    }
    else if (isSynonym(left) && isName(right)) { 
        vector<string> res = PKB::getInstance()->getAllMatchedEntity(syns[left]);
        vector<string> temp;
        if (query_res->isInTable(left)) {
            if (syns[left] == DesignEntity::Procedure) {
                for (auto i : res) {
                    if (isNamesRel(i, strip(right))) {
                        temp.push_back(i);
                    }
                }
            }
            else {
                for (auto i : res) {
                    if (isNamesRel(stoi(i), strip(right))) {
                        temp.push_back(i);
                    }
                }
            }
            result = query_res->filterOneSyn(left, [&](string s) {
                return PKB::getInstance()->isInList(temp, s);
            });
        }
        else {
            if (syns[left] == DesignEntity::Procedure) {
                for (auto i : res) {
                    if (isNamesRel(i, strip(right))) {
                        temp.push_back(i);
                    }
                }
            }
            else {
                for (auto i : res) {
                    if (isNamesRel(stoi(i), strip(right))) {
                        temp.push_back(i);
                    }
                }
            }
            if (temp.size() > 0) {
                result = query_res->addOneSyn(left, temp);
            }
        }
    }
    else if (isSynonym(left) && isWildcard(right)) { 
        vector<string> res = PKB::getInstance()->getAllMatchedEntity(syns[left]);
        vector<string> temp;
        if (query_res->isInTable(left)) {
            if (syns[left] == DesignEntity::Procedure) {
                for (auto i : res) {
                    if (getAllNamesRel(i).size() > 0) {
                        temp.push_back(i);
                    }
                }
            }
            else {
                for (auto i : res) {
                    if (getAllNamesRel(stoi(i)).size() > 0) {
                        temp.push_back(i);
                    }
                }
            }
            result = query_res->filterOneSyn(left, [&](string s) {
                return PKB::getInstance()->isInList(temp, s);
            });
        }
        else {
            if (syns[left] == DesignEntity::Procedure) {
                for (auto i : res) {
                    if (getAllNamesRel(i).size() > 0) {
                        temp.push_back(i);
                    }
                }
            }
            else {
                for (auto i : res) {
                    if (getAllNamesRel(stoi(i)).size() > 0) {
                        temp.push_back(i);
                    }
                }
            }
            if (temp.size() > 0) {
                result = query_res->addOneSyn(left, temp);
            }
        }
    }
    else {
    }
    return result;
}
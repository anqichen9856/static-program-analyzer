#ifndef TEAM21_CP_SPA_20S1_QUERY_H
#define TEAM21_CP_SPA_20S1_QUERY_H

#include <unordered_map>
#include <vector>
#include <string>
#include "utils/SPAEnums.h"
#include "clauses/Clause.h"

using std::string;
using std::unordered_map;
using std::vector;

struct Query {

    unordered_map<string, DesignEntity> synonyms; // from declaration
    vector<string> selected; // select clause
    vector<Clause *> clauses; // unsorted clauses
    vector<list<Clause*>> no_select; // clauses with no synonym, or groups of synonyms that do not contain select clause
    vector<list<Clause*>> with_select; // groups of synonyms that contain select clause

};

#endif //TEAM21_CP_SPA_20S1_QUERY_H

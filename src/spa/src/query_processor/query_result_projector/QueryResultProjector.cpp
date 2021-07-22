#include "QueryResultProjector.h"

QueryResultProjector *QueryResultProjector::instance = NULL;

QueryResultProjector *QueryResultProjector::getInstance() {
    if (instance == NULL) {
        instance = new QueryResultProjector();
    }
    return instance;
}

void QueryResultProjector::formatResults(list<string> &results, QueryResult* query_res, Query query, bool res) {
    if (query.selected[0] == "BOOLEAN" && query.synonyms.find(query.selected[0]) == query.synonyms.end()) {
        if (res) {
            results = {"TRUE"};
        } else {
            results = {"FALSE"};
        }
        return;
    }

    if (!res) {
        results = list<string>();
        return;
    }

    auto raw = query_res->getResults(query.selected);
    copy(raw.begin(), raw.end(), back_inserter(results));
}

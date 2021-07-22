#include "QueryEvaluator.h"


QueryEvaluator *QueryEvaluator::instance = NULL;

QueryEvaluator *QueryEvaluator::getInstance() {
    if (instance == NULL) {
        instance = new QueryEvaluator();
    }
    return instance;
}

bool QueryEvaluator::evaluateQuery(Query query, QueryResult* query_res) {

    for (auto clause : query.clauses) {
        if(!clause->evaluate(&query, query_res)){
            return false;
        }
    }

    return evaluateSelectClause(&query, query_res);
}

bool QueryEvaluator::evaluateSelectClause(Query *query, QueryResult *query_res) {
    for (auto selected : query->selected) {
        if (selected == "BOOLEAN" && query->synonyms.find(selected) == query->synonyms.end()) {
            continue;
        }

        if (QueryParser::isSynAttrPair(selected)) {
            string synonym = QueryParser::getSyn(selected);
            string attr = QueryParser::getAttr(selected);
            if (!query_res->isInTable(synonym)) {
                query_res->createNewTable();
                auto res = PKB::getInstance()->getAllMatchedEntity(query->synonyms[synonym]);
                if (res.empty()) {
                    return false;
                } else {
                    query_res->addOneSyn(synonym, res);
                }
            }

            if (!query_res->isInTable(selected)) {
                query_res -> addAtrribute(synonym, attr, [&](string val) {
                    return PKB::getInstance()->getAttribute(query->synonyms[synonym], val, attr);
                });
            }
        }

        if (QueryParser::isSynonym(selected) && !query_res->isInTable(selected)) {
            query_res->createNewTable();
            auto res = PKB::getInstance()->getAllMatchedEntity(query->synonyms[selected]);
            if (res.empty()) {
                return false;
            } else {
                query_res->addOneSyn(selected, res);
            }
        }
    }
    query_res->createNewTable();
    return true;
}

bool QueryEvaluator::evaluateWithOptimization(Query query, QueryResult* query_res) {

    for (auto lst : query.no_select) {
        for (auto clause : lst) {
            if(!clause->evaluate(&query, query_res)){
                return false;
            }
        }
        query_res -> clear();
    }

    for (auto lst : query.with_select) {
        query_res->createNewTable();
        for (auto clause : lst ) {
            if(!clause->evaluate(&query, query_res)){
                return false;
            }
        }
    }

    return evaluateSelectClause(&query, query_res);
}



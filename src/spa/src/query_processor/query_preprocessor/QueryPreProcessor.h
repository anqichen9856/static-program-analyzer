#ifndef AUTOTESTER_QUERYPREPROCESSOR_H
#define AUTOTESTER_QUERYPREPROCESSOR_H

#include <string>
#include <unordered_map>
#include "query_processor/Query.h"
#include "clauses/RelClause.h"
#include "clauses/PatternClause.h"
#include "QuerySyntaxChecker.h"
#include "exceptions/SPAException.h"
#include "QueryParser.h"
#include "clauses/FollowsClause.h"
#include "clauses/FollowsStarClause.h"
#include "clauses/ParentClause.h"
#include "clauses/ParentStarClause.h"
#include "clauses/ModifiesClause.h"
#include "clauses/UsesClause.h"
#include "clauses/PatternClause.h"
#include "clauses/PatternAssign.h"
#include "clauses/PatternWhile.h"
#include "clauses/PatternIf.h"
#include "clauses/CallsClause.h"
#include "clauses/CallsStarClause.h"
#include "clauses/NextClause.h"
#include "clauses/NextStarClause.h"
#include "clauses/AffectsClause.h"
#include "clauses/AffectsStarClause.h"
#include "clauses/WithClause.h"

using namespace std;

class QueryPreProcessor {
public:

    static QueryPreProcessor *getInstance();

    Query constructQuery(string query_string);

    unordered_map<string, DesignEntity> getDeclaration(string query_string);

    vector<string> getSelected(string query_string);

    vector<Clause *> getClauses(string query_string, unordered_map<string, DesignEntity> syn_entity_map);

    void getRelClauses(string query_string, vector<Clause *> &clauses);
    void getPatternClauses(string query_string, vector<Clause *> &clauses, unordered_map<string, DesignEntity> syn_entity_map);
    void getWithClauses(string query_string, vector<Clause *> &clauses, unordered_map<string, DesignEntity> syn_entity_map);

private:
    static QueryPreProcessor *instance;

    DesignEntity toEntity(string s);

    RelClause *constructRelClause(string rel, string l, string r);

    bool isInMap(string s, unordered_map<string, DesignEntity> map);
    bool isValidSynAttrPair(DesignEntity type, string attr);
    bool isPatternAssignWhile(string s);
    bool isPatternIf(string s);
};

#endif //AUTOTESTER_QUERYPREPROCESSOR_H

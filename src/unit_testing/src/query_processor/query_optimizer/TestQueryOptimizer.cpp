#include <clauses/WithClause.h>
#include <clauses/ModifiesClause.h>
#include <clauses/FollowsClause.h>
#include <clauses/CallsClause.h>
#include <clauses/PatternAssign.h>
#include <clauses/PatternWhile.h>
#include <clauses/PatternIf.h>
#include <clauses/UsesClause.h>
#include <clauses/NextClause.h>
#include "query_processor/query_optimizer/QueryOptimizer.h"
#include "query_processor/query_preprocessor/QueryPreProcessor.h"
#include "catch.hpp"

using namespace std;


TEST_CASE("simple query"){
    Query query;
    query.synonyms = {
            {"s", DesignEntity::Stmt},
            {"v", DesignEntity::Variable},
            {"a", DesignEntity::Assign},
            {"w", DesignEntity::While},
            {"ifs", DesignEntity::If},
            {"p", DesignEntity::Procedure}
    };
    query.selected = {"a"};
    query.clauses = vector<Clause *>();

    FollowsClause follows = FollowsClause("3","4");
    NextClause next = NextClause("10", "s");
    FollowsClause follows2 = FollowsClause("5", "s");
    CallsClause calls = CallsClause("p", "\"name\"");
    PatternAssign assign =  PatternAssign("a", "v", "_");
    PatternWhile wh = PatternWhile("w", "v", "_");
    PatternIf ifs = PatternIf("ifs", "v", "_", "_");
    query.clauses = {&ifs, &calls, &assign, &follows, &wh, &next, &follows2};

    list<Clause*> no_select_grp1 = {&follows};
    list<Clause*> no_select_grp2 = {&calls};
//    list<Clause*> no_select_grp3 = {&next, &follows2};
    list<Clause*> no_select_grp3 = {&follows2, &next};
//    list<Clause*> with_select_grp1 = {&ifs, &assign, &wh};
    list<Clause*> with_select_grp1 = {&wh, &ifs, &assign};

    QueryOptimizer* qo = new QueryOptimizer(&query);
    qo->optimizeQuery();

    REQUIRE(query.no_select[0] == no_select_grp1);
    REQUIRE(query.no_select[1] == no_select_grp2);
    REQUIRE(query.no_select[2] == no_select_grp3);
    REQUIRE(query.with_select[0] == with_select_grp1);
}

TEST_CASE("2"){
    string query_string = "stmt s; variable v; print pn; Select pn such that Uses(pn, v) and Modifies(s,v) and Next(s, pn)";
    Query query = QueryPreProcessor::getInstance()->constructQuery(query_string);

    ModifiesClause modifies = ModifiesClause("s","v");
    NextClause next = NextClause("s", "pn");
    UsesClause uses = UsesClause("pn", "v");

    query.clauses = {&uses, &modifies, &next};
//    list<Clause*> with_select_grp1 = {&uses, &next, &modifies};
    list<Clause*> with_select_grp1 = {&next, &uses, &modifies};

    QueryOptimizer* qo = new QueryOptimizer(&query);
    qo->optimizeQuery();

    REQUIRE(query.no_select.size() == 0);
    REQUIRE(query.with_select.size() == 1);
    REQUIRE(query.with_select[0] == with_select_grp1);
}

TEST_CASE("test rank") {
    FollowsClause follows = FollowsClause("3","4");
    NextClause next = NextClause("10", "s");
    CallsClause calls = CallsClause("p", "\"name\"");
    PatternAssign assign =  PatternAssign("a", "v", "_");
    PatternWhile wh = PatternWhile("w", "v", "_");
    PatternIf ifs = PatternIf("ifs", "v", "_", "_");

    vector<Clause*> clauses = {};
    clauses.push_back(&ifs);
    clauses.push_back(&calls);
    clauses.push_back(&assign);
    clauses.push_back(&follows);
    clauses.push_back(&wh);
    clauses.push_back(&next);
    Query query;
    QueryOptimizer* qp = new QueryOptimizer(&query);
    qp->rankByClauseType(clauses);
    vector<Clause*> answers = {&calls, &follows, &next, &wh, &ifs, &assign};
    REQUIRE(clauses == answers);
}


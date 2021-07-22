#include "clauses/ParentClause.h"
#include "clauses/ParentStarClause.h"
#include "clauses/UsesClause.h"
#include "clauses/ModifiesClause.h"
#include "clauses/FollowsClause.h"
#include "clauses/FollowsStarClause.h"
#include "catch.hpp"
#include "query_processor/query_preprocessor/QueryPreProcessor.h"
#include "utils/Constants.h"
#include "query_processor/query_preprocessor/QueryParser.h"

using namespace std;

TEST_CASE("get declaration") {

    unordered_map<string, DesignEntity> answer = {
            { "s",  DesignEntity::Stmt},
            { "v", DesignEntity::Variable},
            { "w",  DesignEntity::While} };

    string query1 = "stmt s; variable v; while w; Select s";
    unordered_map<string, DesignEntity> map1 = QueryPreProcessor::getInstance()->getDeclaration(query1);
    REQUIRE(answer == map1);

    string query2 = "stmt s; variable    v   ;    while w   ;   Select s";
    unordered_map<string, DesignEntity> map2 = QueryPreProcessor::getInstance()->getDeclaration(query2);
    REQUIRE(answer == map2);


    SECTION("comma") {
        string query = "variable a   , b  ; print pn  ; Select pn";
        unordered_map<string, DesignEntity> map = {
                {"a", DesignEntity::Variable},
                {"b", DesignEntity::Variable},
                {"pn", DesignEntity::Print}
        };
        unordered_map<string, DesignEntity> result = QueryPreProcessor::getInstance() -> getDeclaration(query);
        REQUIRE(result == map);
        REQUIRE(map["a"] == DesignEntity::Variable);
        REQUIRE(map["b"] == DesignEntity::Variable);
        REQUIRE(map["pn"] == DesignEntity::Print);
        REQUIRE(map.size() == 3);
    }

    SECTION("repeated syn") {
        string query = "assign a; while a; variable v; Select v";
        REQUIRE_THROWS(QueryPreProcessor::getInstance() -> getDeclaration(query));
    }

    SECTION("comma") {
        string query = "assign a, a1; Select a such that Follows(a,a1)";
        unordered_map<string, DesignEntity> map = {
                {"a",  DesignEntity::Assign},
                {"a1", DesignEntity::Assign}
        };
        unordered_map<string, DesignEntity> result = QueryPreProcessor::getInstance()->getDeclaration(query);
        REQUIRE(result == map);
        REQUIRE(map["a"] == DesignEntity::Assign);
        REQUIRE(map["a1"] == DesignEntity::Assign);
        REQUIRE(map.size() == 2);
    }

    SECTION("no declaration") {
        string query = "Select BOOLEAN such that Follows(1,2)";
        unordered_map<string, DesignEntity> result = QueryPreProcessor::getInstance()->getDeclaration(query);
        REQUIRE(result.size() == 0);
    }
}

TEST_CASE("get selected entity") {
    string query = "assign a; while w; variable v; if ifs; Select <a,v> such that Uses(w,v) and Follows(3,w) pattern a(v,_) and ifs(v,_,_) with a.stmt# = 5";
    vector<string> res = QueryPreProcessor::getInstance()->getSelected(query);
    REQUIRE(res.size() == 2);
    REQUIRE(res[0] == "a");
    REQUIRE(res[1] == "v");

    SECTION("select BOOLEAN") {
        string query = "stmt s; Select BOOLEAN such that Follows(1,2)";
        vector<string> res = QueryPreProcessor::getInstance()->getSelected(query);
        REQUIRE(res.size() == 1);
        REQUIRE(res[0] == "BOOLEAN");
    }
    SECTION("select Select") {
        string query = "stmt Select; Select Select";
        vector<string> res = QueryPreProcessor::getInstance()->getSelected(query);
        REQUIRE(res.size() == 1);
        REQUIRE(res[0] == "Select");
    }
    string query1 = "stmt s; variable v; Select s";
    vector<string> res1 = QueryPreProcessor::getInstance()->getSelected(query1);
    REQUIRE(res1.size() == 1);
    REQUIRE(res1[0] == "s");

    string query2 = "stmt s; variable v;     Select      v";
    vector<string> res2 = QueryPreProcessor::getInstance()->getSelected(query2);
    REQUIRE(res2.size() == 1);
    REQUIRE(res2[0] == "v");

    string query5 = "stmt s; variable v; Select v such that ";
    vector<string> res5 = QueryPreProcessor::getInstance()->getSelected(query5);
    REQUIRE(res5.size() == 1);
    REQUIRE(res5[0] == "v");

    string query6 = "stmt s; variable v; Select v pattern a(v, _) ";
    vector<string> res6 = QueryPreProcessor::getInstance()->getSelected(query6);
    REQUIRE(res6.size() == 1);
    REQUIRE(res6[0] == "v");
}

TEST_CASE("get relational clause") {
    string query = "assign a; while w; variable v; if ifs; Select <a,v> such that Uses(w,v) and Follows(3,w) pattern a(v,_) and ifs(v,_,_) with a.stmt# = 5";
    UsesClause u = UsesClause("w", "v");
    FollowsClause f = FollowsClause("3", "w");
    vector<Clause *> clauses;
    QueryPreProcessor::getInstance()->getRelClauses(query, clauses);
    REQUIRE(clauses.size() == 2);
    REQUIRE(u.equals((UsesClause *) clauses[0]));
    REQUIRE(f.equals((FollowsClause *) clauses[1]));

    SECTION("Follows Clause") {
        string query1 = "stmt s; Select s such that Follows(3, 4)";
        vector<Clause *> v1;
        QueryPreProcessor::getInstance()->getRelClauses(query1, v1);
        RelClause* c1 = (RelClause*)v1[0];
        FollowsClause answer1 = FollowsClause("3", "4");
        REQUIRE(c1->getClauseName() == "Follows");
        REQUIRE(answer1.equals((FollowsClause*)c1));

        string query2 = "stmt s; Select s such that Follows(3, _)";
        vector<Clause *> v2;
        QueryPreProcessor::getInstance()->getRelClauses(query2, v2);
        RelClause* c2 = (RelClause*)v2[0];
        FollowsClause answer2 = FollowsClause("3", "_");
        REQUIRE(c2->getClauseName() == "Follows");
        REQUIRE(answer2.equals((FollowsClause*)c2));

        string query3 = "stmt s; Select s such that Follows(s, _)";
        vector<Clause *> v3;
        QueryPreProcessor::getInstance()->getRelClauses(query3, v3);
        RelClause* c3 = (RelClause*)v3[0];
        FollowsClause answer3 = FollowsClause("s", "_");
        REQUIRE(c3->getClauseName() == "Follows");
        REQUIRE(answer3.equals((FollowsClause*)c3));

        string query4 = "stmt s; Select s such that Follows(s, 4)";
        vector<Clause *> v4;
        QueryPreProcessor::getInstance()->getRelClauses(query4, v4);
        RelClause* c4 = (RelClause*)v4[0];
        FollowsClause answer4 = FollowsClause("s", "4");
        REQUIRE(c4->getClauseName() == "Follows");
        REQUIRE(answer4.equals((FollowsClause*)c4));

        string query5 = "stmt s; Select s such that Follows(s, a)";
        vector<Clause *> v5;
        QueryPreProcessor::getInstance()->getRelClauses(query5, v5);
        RelClause* c5 = (RelClause*)v5[0];
        FollowsClause answer5 = FollowsClause("s", "a");
        REQUIRE(c5->getClauseName() == "Follows");
        REQUIRE(answer5.equals((FollowsClause*)c5));
    }

    SECTION("Follows* Clause") {
        string query1 = "stmt s; Select s such that Follows*(3, 5)";
        vector<Clause *> v1;
        QueryPreProcessor::getInstance()->getRelClauses(query1, v1);
        RelClause* c1 = (RelClause*)v1[0];
        FollowsStarClause answer1 = FollowsStarClause("3", "5");
        REQUIRE(c1->getClauseName() == "Follows*");
        REQUIRE(answer1.equals((FollowsStarClause*)c1));

        string query2 = "stmt s; Select s such that Follows*(3, _)";
        vector<Clause *> v2;
        QueryPreProcessor::getInstance()->getRelClauses(query2, v2);
        RelClause* c2 = (RelClause*)v2[0];
        FollowsStarClause answer2 = FollowsStarClause("3", "_");
        REQUIRE(c2->getClauseName() == "Follows*");
        REQUIRE(answer2.equals((FollowsStarClause*)c2));

        string query3 = "stmt s; Select s such that Follows*(s, _)";
        vector<Clause *> v3;
        QueryPreProcessor::getInstance()->getRelClauses(query3, v3);
        RelClause* c3 = (RelClause*)v3[0];
        FollowsStarClause answer3 = FollowsStarClause("s", "_");
        REQUIRE(c3->getClauseName() == "Follows*");
        REQUIRE(answer3.equals((FollowsStarClause*)c3));

        string query4 = "stmt s; Select s such that Follows*(s, 4)";
        vector<Clause *> v4;
        QueryPreProcessor::getInstance()->getRelClauses(query4, v4);
        RelClause* c4 = (RelClause*)v4[0];
        FollowsStarClause answer4 = FollowsStarClause("s", "4");
        REQUIRE(c4->getClauseName() == "Follows*");
        REQUIRE(answer4.equals((FollowsStarClause*)c4));

        string query5 = "stmt s; Select s such that Follows*(s, a)";
        vector<Clause *> v5;
        QueryPreProcessor::getInstance()->getRelClauses(query5, v5);
        RelClause* c5 = (RelClause*)v5[0];
        FollowsStarClause answer5 = FollowsStarClause("s", "a");
        REQUIRE(c5->getClauseName() == "Follows*");
        REQUIRE(answer5.equals((FollowsStarClause*)c5));
    }

    SECTION("Parent Clause") {
        string query1 = "stmt s; Select s such that Parent(3, 4)";
        vector<Clause *> v1;
        QueryPreProcessor::getInstance()->getRelClauses(query1, v1);
        RelClause* c1 = (RelClause*)v1[0];
        ParentClause answer1 = ParentClause("3", "4");
        REQUIRE(c1->getClauseName() == "Parent");
        REQUIRE(answer1.equals((ParentClause*)c1));

        string query2 = "stmt s; Select s such that Parent(3, _)";
        vector<Clause *> v2;
        QueryPreProcessor::getInstance()->getRelClauses(query2, v2);
        RelClause* c2 = (RelClause*)v2[0];
        ParentClause answer2 = ParentClause("3", "_");
        REQUIRE(c2->getClauseName() == "Parent");
        REQUIRE(answer2.equals((ParentClause*)c2));

        string query3 = "stmt s; Select s such that Parent(s, _)";
        vector<Clause *> v3;
        QueryPreProcessor::getInstance()->getRelClauses(query3, v3);
        RelClause* c3 = (RelClause*)v3[0];
        ParentClause answer3 = ParentClause("s", "_");
        REQUIRE(c3->getClauseName() == "Parent");
        REQUIRE(answer3.equals((ParentClause*)c3));

        string query4 = "stmt s; Select s such that Parent(s, 4)";
        vector<Clause *> v4;
        QueryPreProcessor::getInstance()->getRelClauses(query4, v4);
        RelClause* c4 = (RelClause*)v4[0];
        ParentClause answer4 = ParentClause("s", "4");
        REQUIRE(c4->getClauseName() == "Parent");
        REQUIRE(answer4.equals((ParentClause*)c4));

        string query5 = "stmt s; Select s such that Parent(s, a)";
        vector<Clause *> v5;
        QueryPreProcessor::getInstance()->getRelClauses(query5, v5);
        RelClause* c5 = (RelClause*)v5[0];
        ParentClause answer5 = ParentClause("s", "a");
        REQUIRE(c5->getClauseName() == "Parent");
        REQUIRE(answer5.equals((ParentClause*)c5));

    }

    SECTION("Parent* Clause") {
        string query1 = "stmt s; Select s such that Parent*(3, 4)";
        vector<Clause *> v1;
        QueryPreProcessor::getInstance()->getRelClauses(query1, v1);
        RelClause* c1 = (RelClause*)v1[0];
        ParentStarClause answer1 = ParentStarClause("3", "4");
        REQUIRE(c1->getClauseName() == "Parent*");
        REQUIRE(answer1.equals((ParentStarClause*)c1));

        string query2 = "stmt s; Select s such that Parent*(3, _)";
        vector<Clause *> v2;
        QueryPreProcessor::getInstance()->getRelClauses(query2, v2);
        RelClause* c2 = (RelClause*)v2[0];
        ParentStarClause answer2 = ParentStarClause("3", "_");
        REQUIRE(c2->getClauseName() == "Parent*");
        REQUIRE(answer2.equals((ParentStarClause*)c2));

        string query3 = "stmt s; Select s such that Parent*(s, _)";
        vector<Clause *> v3;
        QueryPreProcessor::getInstance()->getRelClauses(query3, v3);
        RelClause* c3 = (RelClause*)v3[0];
        ParentStarClause answer3 = ParentStarClause("s", "_");
        REQUIRE(c3->getClauseName() == "Parent*");
        REQUIRE(answer3.equals((ParentStarClause*)c3));

        string query4 = "stmt s; Select s such that Parent*(s, 4)";
        vector<Clause *> v4;
        QueryPreProcessor::getInstance()->getRelClauses(query4, v4);
        RelClause* c4 = (RelClause*)v4[0];
        ParentStarClause answer4 = ParentStarClause("s", "4");
        REQUIRE(c4->getClauseName() == "Parent*");
        REQUIRE(answer4.equals((ParentStarClause*)c4));

        string query5 = "stmt s; Select s such that Parent*(s, a)";
        vector<Clause *> v5;
        QueryPreProcessor::getInstance()->getRelClauses(query5, v5);
        RelClause* c5 = (RelClause*)v5[0];
        ParentStarClause answer5 = ParentStarClause("s", "a");
        REQUIRE(c5->getClauseName() == "Parent*");
        REQUIRE(answer5.equals((ParentStarClause*)c5));

    }

    SECTION("Uses Clause") {
        string query1 = "stmt s; Select s such that Uses(3, v)";
        vector<Clause *> v1;
        QueryPreProcessor::getInstance()->getRelClauses(query1, v1);
        RelClause* c1 = (RelClause*)v1[0];
        UsesClause answer1 = UsesClause("3", "v");
        REQUIRE(c1->getClauseName() == "Uses");
        REQUIRE(answer1.equals((UsesClause*)c1));

        string query2 = "stmt s; Select s such that Uses(3, _)";
        vector<Clause *> v2;
        QueryPreProcessor::getInstance()->getRelClauses(query2, v2);
        RelClause* c2 = (RelClause*)v2[0];
        UsesClause answer2 = UsesClause("3", "_");
        REQUIRE(c2->getClauseName() == "Uses");
        REQUIRE(answer2.equals((UsesClause*)c2));

        string query3 = "stmt s; Select s such that Uses(s, _)";
        vector<Clause *> v3;
        QueryPreProcessor::getInstance()->getRelClauses(query3, v3);
        RelClause* c3 = (RelClause*)v3[0];
        UsesClause answer3 = UsesClause("s", "_");
        REQUIRE(c3->getClauseName() == "Uses");
        REQUIRE(answer3.equals((UsesClause*)c3));

        string query4 = "stmt s; Select s such that Uses(s, \"x\")";
        vector<Clause *> v4;
        QueryPreProcessor::getInstance()->getRelClauses(query4, v4);
        RelClause* c4 = (RelClause*)v4[0];
        UsesClause answer4 = UsesClause("s", "\"x\"");
        REQUIRE(c4->getClauseName() == "Uses");
        REQUIRE(answer4.equals((UsesClause*)c4));

        string query5 = "stmt s; Select s such that Uses(\"Sun\", v)";
        vector<Clause *> v5;
        QueryPreProcessor::getInstance()->getRelClauses(query5, v5);
        RelClause* c5 = (RelClause*)v5[0];
        UsesClause answer5 = UsesClause("\"Sun\"", "v");
        REQUIRE(c5->getClauseName() == "Uses");
        REQUIRE(answer5.equals((UsesClause*)c5));

        string query6 = "stmt s; Select s such that Uses(_, v)";
        vector<Clause *> v6;
        QueryPreProcessor::getInstance()->getRelClauses(query6, v6);
        RelClause* c6 = (RelClause*)v6[0];
        UsesClause answer6 = UsesClause("_", "v");
        REQUIRE(c6->getClauseName() == "Uses");
        REQUIRE(answer6.equals((UsesClause*)c6));

    }

    SECTION("Modifies Clause") {
        string query1 = "stmt s; Select s such that Modifies(3, v)";
        vector<Clause *> v1;
        QueryPreProcessor::getInstance()->getRelClauses(query1, v1);
        RelClause* c1 = (RelClause*)v1[0];
        ModifiesClause answer1 = ModifiesClause("3", "v");
        REQUIRE(c1->getClauseName() == "Modifies");
        REQUIRE(answer1.equals((ModifiesClause*)c1));

        string query2 = "stmt s; Select s such that Modifies(3, _)";
        vector<Clause *> v2;
        QueryPreProcessor::getInstance()->getRelClauses(query2, v2);
        RelClause* c2 = (RelClause*)v2[0];
        ModifiesClause answer2 = ModifiesClause("3", "_");
        REQUIRE(c2->getClauseName() == "Modifies");
        REQUIRE(answer2.equals((ModifiesClause*)c2));

        string query3 = "stmt s; Select s such that Modifies(s, _)";
        vector<Clause *> v3;
        QueryPreProcessor::getInstance()->getRelClauses(query3, v3);
        RelClause* c3 = (RelClause*)v3[0];
        ModifiesClause answer3 = ModifiesClause("s", "_");
        REQUIRE(c3->getClauseName() == "Modifies");
        REQUIRE(answer3.equals((ModifiesClause*)c3));

        string query4 = "stmt s; Select s such that Modifies(s, \"x\")";
        vector<Clause *> v4;
        QueryPreProcessor::getInstance()->getRelClauses(query4, v4);
        RelClause* c4 = (RelClause*)v4[0];
        ModifiesClause answer4 = ModifiesClause("s", "\"x\"");
        REQUIRE(c4->getClauseName() == "Modifies");
        REQUIRE(answer4.equals((ModifiesClause*)c4));

        string query5 = "stmt s; Select s such that Modifies(\"Sun\", v)";
        vector<Clause *> v5;
        QueryPreProcessor::getInstance()->getRelClauses(query5, v5);
        RelClause* c5 = (RelClause*)v5[0];
        ModifiesClause answer5 = ModifiesClause("\"Sun\"", "v");
        REQUIRE(c5->getClauseName() == "Modifies");
        REQUIRE(answer5.equals((ModifiesClause*)c5));

        string query6 = "stmt s; Select s such that Modifies(_, v)";
        vector<Clause *> v6;
        QueryPreProcessor::getInstance()->getRelClauses(query6, v6);
        RelClause* c6 = (RelClause*)v6[0];
        ModifiesClause answer6 = ModifiesClause("_", "v");
        REQUIRE(c6->getClauseName() == "Modifies");
        REQUIRE(answer6.equals((ModifiesClause*)c6));
    }

    SECTION("Calls Clause") {
        string query1 = "Select BOOLEAN such that Calls(\"Dio\", \"proc\")";
        vector<Clause *> v1;
        QueryPreProcessor::getInstance()->getRelClauses(query1, v1);
        RelClause *c1 = (RelClause *) v1[0];
        CallsClause answer1 = CallsClause("\"Dio\"", "\"proc\"");
        REQUIRE(c1->getClauseName() == "Calls");
        REQUIRE(answer1.equals((CallsClause *) c1));
    }

    SECTION("Calls* Clause") {
        string query1 = "procedure p; Select p such that Calls*(\"Joseph\", p)";
        vector<Clause *> v1;
        QueryPreProcessor::getInstance()->getRelClauses(query1, v1);
        RelClause *c1 = (RelClause *) v1[0];
        CallsStarClause answer1 = CallsStarClause("\"Joseph\"", "p");
        REQUIRE(c1->getClauseName() == "Calls*");
        REQUIRE(answer1.equals((CallsStarClause *) c1));
    }

    SECTION("Next Clause") {
        string query1 = "prog_line p, q; Select <p, q> such that Next(q, p)";
        vector<Clause *> v1;
        QueryPreProcessor::getInstance()->getRelClauses(query1, v1);
        RelClause *c1 = (RelClause *) v1[0];
        NextClause answer1 = NextClause("q", "p");
        REQUIRE(c1->getClauseName() == "Next");
        REQUIRE(answer1.equals((NextClause *) c1));
    }

    SECTION("Next* Clause") {
        string query1 = "prog_line p; Select p such that Next*(5, p)";
        vector<Clause *> v1;
        QueryPreProcessor::getInstance()->getRelClauses(query1, v1);
        RelClause *c1 = (RelClause *) v1[0];
        NextStarClause answer1 = NextStarClause("5", "p");
        REQUIRE(c1->getClauseName() == "Next*");
        REQUIRE(answer1.equals((NextStarClause *) c1));

        //"same synonym in Next* clause"
        string query = "procedure p; variable v; constant c; stmt s; read r; print pr; call cp; while w; if ifs; assign a; prog_line n;\nSelect ifs such that Next*(ifs,ifs)\n";
        NextStarClause c = NextStarClause("ifs", "ifs");
        vector<Clause *> clauses;
        QueryPreProcessor::getInstance()->getRelClauses(query, clauses);
        REQUIRE(clauses.size() == 1);
        REQUIRE(c.equals((NextStarClause*) clauses[0]));
    }

    SECTION("negative stmt number") {
        string query = "stmt s; Select s such that Follows(-2,1)";
        vector<Clause *> v;
        REQUIRE_THROWS_AS(QueryPreProcessor::getInstance()->getRelClauses(query, v), InvalidQueryException);
    }
}

TEST_CASE("get pattern clause") {
    SECTION("1") {
        string query = "assign a; while w; variable v; if ifs; Select <a,v> such that Uses(w,v) and Follows(3,w) pattern a(v,_) and ifs(v,_,_) with a.stmt# = 5";
        PatternAssign pa = PatternAssign("a", "v", "_");
        PatternIf pi = PatternIf("ifs", "v", "_", "_");
        vector<Clause *> clauses;
        unordered_map<string, DesignEntity> map = QueryPreProcessor::getInstance()->getDeclaration(query);

        QueryPreProcessor::getInstance()->getPatternClauses(query, clauses, map);
        REQUIRE(clauses.size() == 2);
        REQUIRE(pa.equals((PatternAssign *) clauses[0]));
        REQUIRE(pi.equals((PatternIf *) clauses[1]));
    }

    SECTION("2") {
        string query = "assign a; while w; variable v; if ifs; Select <a,v> such that Uses(w,v) and Follows(3,w) pattern a(v, \"x\") with a.stmt# = 5";
        PatternAssign pa = PatternAssign("a", "v", "\"x\"");
        vector<Clause *> clauses;
        unordered_map<string, DesignEntity> map = QueryPreProcessor::getInstance()->getDeclaration(query);

        QueryPreProcessor::getInstance()->getPatternClauses(query, clauses, map);
        REQUIRE(clauses.size() == 1);
        REQUIRE(pa.equals((PatternAssign *) clauses[0]));
    }

    SECTION("3") {
        string query = "assign a; while w; variable v; if ifs; Select <a,v> such that Uses(w,v) and Follows(3,w) pattern a(v, v) with a.stmt# = 5";
        vector<Clause *> clauses;
        unordered_map<string, DesignEntity> map = QueryPreProcessor::getInstance()->getDeclaration(query);

        REQUIRE_THROWS(QueryPreProcessor::getInstance()->getPatternClauses(query, clauses, map));
    }

    SECTION("4") {
        string query = "assign a; while w; variable v; if ifs; Select <a,v> such that Uses(w,v) and Follows(3,w) pattern a(_, \"x\") with a.stmt# = 5";
        PatternAssign pa = PatternAssign("a", "_", "\"x\"");
        vector<Clause *> clauses;
        unordered_map<string, DesignEntity> map = QueryPreProcessor::getInstance()->getDeclaration(query);

        QueryPreProcessor::getInstance()->getPatternClauses(query, clauses, map);
        REQUIRE(clauses.size() == 1);
        REQUIRE(pa.equals((PatternAssign *) clauses[0]));
    }


    SECTION("5") {
        string query = "assign a; while w; variable v; if ifs; Select <a,v> such that Uses(w,v) and Follows(3,w) pattern a(\"y\", \"x\") with a.stmt# = 5";
        PatternAssign pa = PatternAssign("a", "\"y\"", "\"x\"");
        vector<Clause *> clauses;
        unordered_map<string, DesignEntity> map = QueryPreProcessor::getInstance()->getDeclaration(query);

        QueryPreProcessor::getInstance()->getPatternClauses(query, clauses, map);
        REQUIRE(clauses.size() == 1);
        REQUIRE(pa.equals((PatternAssign *) clauses[0]));
    }

    SECTION("6") {
        string query = "assign a; while w; variable v; if ifs; Select <a,v> such that Uses(w,v) and Follows(3,w) pattern a(\"x\", _\"y\"_) with a.stmt# = 5";
        PatternAssign pa = PatternAssign("a", "\"x\"", "_\"y\"_");
        vector<Clause *> clauses;
        unordered_map<string, DesignEntity> map = QueryPreProcessor::getInstance()->getDeclaration(query);

        QueryPreProcessor::getInstance()->getPatternClauses(query, clauses, map);
        REQUIRE(clauses.size() == 1);
        REQUIRE(pa.equals((PatternAssign *) clauses[0]));
    }

    SECTION("7") {
        string query = "assign a; while w; variable v; if ifs; Select <a,v> such that Uses(w,v) and Follows(3,w) pattern a(_, _\"1\"_) with a.stmt# = 5";
        PatternAssign pa = PatternAssign("a", "_", "_\"1\"_");
        vector<Clause *> clauses;
        unordered_map<string, DesignEntity> map = QueryPreProcessor::getInstance()->getDeclaration(query);

        QueryPreProcessor::getInstance()->getPatternClauses(query, clauses, map);
        REQUIRE(clauses.size() == 1);
        REQUIRE(pa.equals((PatternAssign *) clauses[0]));
    }
}

TEST_CASE("get with clause") {
    string query = "assign a; while w; variable v; if ifs; Select <a,v> such that Uses(w,v) and Follows(3,w) pattern a(v,_) and ifs(v,_,_) with a.stmt# = 5 and v.varName= \"x\"";
    unordered_map<string, DesignEntity> map = {
            {"a", DesignEntity::Assign},
            {"w", DesignEntity::While},
            {"v", DesignEntity::Variable},
            {"ifs", DesignEntity::If}
    };
    WithClause w1 = WithClause("a.stmt#", "5");
    WithClause w2 = WithClause("v.varName", "\"x\"");
    vector<Clause *> clauses;
    QueryPreProcessor::getInstance()->getWithClauses(query, clauses, map);
    REQUIRE(clauses.size() == 2);
    REQUIRE(w1.equals((WithClause *) clauses[0]));
    REQUIRE(w2.equals((WithClause *) clauses[1]));
}


TEST_CASE("get clauses") {
    string query = "assign a; Select a such that Modifies(a, \"x\") pattern a(\"x\", \"y\")";
    unordered_map<string, DesignEntity> map = {
            {"a", DesignEntity::Assign}
    };
    vector<Clause*> clauses = QueryPreProcessor::getInstance()->getClauses(query, map);
    ModifiesClause modifies = ModifiesClause("a", "\"x\"");
    PatternAssign pttn = PatternAssign("a", "\"x\"", "\"y\"");
    REQUIRE(modifies.equals((ModifiesClause*)clauses[0]));
    REQUIRE(pttn.equals((PatternAssign*)clauses[1]));
}


TEST_CASE("construct query") {
    SECTION("1") {
        string query = "assign a; while w; variable v; if ifs; Select <a,v> such that Uses(w,v) and Follows(3,w) pattern a(v,_) and ifs(v,_,_) with a.stmt# = 5";
        Query q = QueryPreProcessor::getInstance()->constructQuery(query);
        unordered_map<string, DesignEntity> synonyms = {
                {"a",   DesignEntity::Assign},
                {"w",   DesignEntity::While},
                {"v",   DesignEntity::Variable},
                {"ifs", DesignEntity::If}};
        vector<string> selected = {"a", "v"};

        UsesClause u = UsesClause("w", "v");
        FollowsClause f = FollowsClause("3", "w");
        PatternAssign pa = PatternAssign("a", "v", "_");
        PatternIf pi = PatternIf("ifs", "v", "_", "_");
        WithClause w = WithClause("a.stmt#", "5");

        REQUIRE(synonyms == q.synonyms);
        REQUIRE(q.clauses.size() == 5);
        REQUIRE(u.equals((UsesClause *) q.clauses[0]));
        REQUIRE(f.equals((FollowsClause *) q.clauses[1]));
        REQUIRE(pa.equals((PatternAssign *) q.clauses[2]));
        REQUIRE(pi.equals((PatternIf *) q.clauses[3]));
        REQUIRE(w.equals((WithClause *) q.clauses[4]));
        REQUIRE(selected == q.selected);
    }

    SECTION("select BOOLEAN") {
        string query = "stmt s; \nSelect BOOLEAN such that Follows(1,2)";
        Query q = QueryPreProcessor::getInstance()->constructQuery(query);
        REQUIRE(q.selected.size() == 1);
        REQUIRE(q.selected[0] == "BOOLEAN");
        REQUIRE(q.synonyms.size() == 1);
        REQUIRE(q.clauses.size() == 1);
    }

    SECTION("all tabs") {
        string query = "\tassign\ta\t;\tstmt\ts\t;\tSelect\ts\tsuch that\tParent*\t(\ts\t,\ta\t)\t";
        Query q = QueryPreProcessor::getInstance()->constructQuery(query);
        unordered_map<string, DesignEntity> synonyms = {
                {"a",   DesignEntity::Assign},
                {"s",   DesignEntity::Stmt}};
        vector<string> selected = {"s"};

        ParentStarClause p = ParentStarClause("s", "a");
        REQUIRE(synonyms == q.synonyms);
        REQUIRE(q.clauses.size() == 1);
        REQUIRE(p.equals((ParentStarClause *) q.clauses[0]));
        REQUIRE(selected == q.selected);
    }

    SECTION("all line breaks") {
        string query = "\nassign\na\n;\nstmt\ns\n;\nSelect\ns\nsuch that\nParent*\n(\ns\n,\na\n)\n";
        Query q = QueryPreProcessor::getInstance()->constructQuery(query);
        unordered_map<string, DesignEntity> synonyms = {
                {"a",   DesignEntity::Assign},
                {"s",   DesignEntity::Stmt}};
        vector<string> selected = {"s"};

        ParentStarClause p = ParentStarClause("s", "a");
        REQUIRE(synonyms == q.synonyms);
        REQUIRE(q.clauses.size() == 1);
        REQUIRE(p.equals((ParentStarClause *) q.clauses[0]));
        REQUIRE(selected == q.selected);
    }

    SECTION("combination") {
        string query = "assign \n a \t ; \t stmt \n s \n ; \t Select\t s \n such that \t Parent* \n ( \t s \n,\n a\t) \n ";
        Query q = QueryPreProcessor::getInstance()->constructQuery(query);
        unordered_map<string, DesignEntity> synonyms = {
                {"a",   DesignEntity::Assign},
                {"s",   DesignEntity::Stmt}};
        vector<string> selected = {"s"};

        ParentStarClause p = ParentStarClause("s", "a");
        REQUIRE(synonyms == q.synonyms);
        REQUIRE(q.clauses.size() == 1);
        REQUIRE(p.equals((ParentStarClause *) q.clauses[0]));
        REQUIRE(selected == q.selected);
    }

    SECTION("system test") {
        string query = "assign a; variable v;\nSelect a pattern a(v, \"100\")";
        Query q = QueryPreProcessor::getInstance()->constructQuery(query);
        unordered_map<string, DesignEntity> synonyms = {
                {"a",   DesignEntity::Assign},
                {"v",   DesignEntity::Variable}};
        vector<string> selected = {"a"};

        PatternAssign p = PatternAssign("a", "v", "\"100\"");
        REQUIRE(synonyms == q.synonyms);
        REQUIRE(q.clauses.size() == 1);
        REQUIRE(p.equals((PatternAssign *) q.clauses[0]));
        REQUIRE(selected == q.selected);
    }

    SECTION("system test select BOOLEAN") {
        string query = "variable v; Select BOOLEAN with \"else\" = v.varName";
        Query q = QueryPreProcessor::getInstance()->constructQuery(query);
        unordered_map<string, DesignEntity> synonyms = {
                {"v", DesignEntity::Variable}
        };
        vector<string> selected = {"BOOLEAN"};

        WithClause w = WithClause("\"else\"", "v.varName");
        REQUIRE(synonyms == q.synonyms);
        REQUIRE(q.clauses.size() == 1);
        REQUIRE(w.equals((WithClause *) q.clauses[0]));
        REQUIRE(selected == q.selected);
    }

    SECTION("select BOOLEAN") {

        string query = "assign a; Select a pattern a(_, \"(easy + question) / mark\")";
        unordered_map<string, DesignEntity> map = QueryPreProcessor::getInstance()->getDeclaration(query);
        vector<Clause *> v;
        QueryPreProcessor::getInstance()->getPatternClauses(query, v, map);
        REQUIRE(v[0]->getRightParam() == "\"(easy+question)/mark\"");
    }

    SECTION("no clause") {
        string query = "stmt s; Select s";
        Query q = QueryPreProcessor::getInstance()->constructQuery(query);
        unordered_map<string, DesignEntity> syn_entity_map = {
                {"s", DesignEntity::Stmt}};
        vector<string> selected = {"s"};
        REQUIRE(syn_entity_map == q.synonyms);
        REQUIRE(q.clauses.size() == 0);
        REQUIRE(selected == q.selected);
    }

    SECTION("no clause 2") {
        string query = "read r; variable Select; Select Select";
        Query q = QueryPreProcessor::getInstance()->constructQuery(query);
        unordered_map<string, DesignEntity> syn_entity_map = {
                {"r", DesignEntity::Read},
                {"Select", DesignEntity::Variable}};
        vector<string> selected = {"Select"};
        REQUIRE(syn_entity_map == q.synonyms);
        REQUIRE(q.clauses.size() == 0);
        REQUIRE(selected == q.selected);
    }

    SECTION("rel + pattern") {
        string query = "assign Select; variable v; Select v such that Modifies(Select, v) pattern Select(v, \"y\")";
        Query q = QueryPreProcessor::getInstance()->constructQuery(query);
        unordered_map<string, DesignEntity> syn_entity_map = {
                {"Select", DesignEntity::Assign},
                {"v", DesignEntity::Variable}};
        ModifiesClause rel = ModifiesClause("Select", "v");
        PatternAssign pttn = PatternAssign("Select", "v", "\"y\"");
        vector<string> selected = {"v"};
        REQUIRE(syn_entity_map == q.synonyms);
        REQUIRE(q.clauses.size() == 2);
        REQUIRE(rel.equals((ModifiesClause*)q.clauses[0]));
        REQUIRE(pttn.equals((PatternAssign*)q.clauses[1]));
        REQUIRE(selected == q.selected);
    }

    SECTION("rel") {
        string query = "stmt s; Select s such that Follows(s, 4)";
        Query q = QueryPreProcessor::getInstance()->constructQuery(query);
        unordered_map<string, DesignEntity> syn_entity_map = {
                {"s", DesignEntity::Stmt}};
        FollowsClause rel = FollowsClause("s", "4");
        vector<string> selected = {"s"};
        REQUIRE(syn_entity_map == q.synonyms);
        REQUIRE(q.clauses.size() == 1);
        REQUIRE(rel.equals((FollowsClause*)q.clauses[0]));
        REQUIRE(selected == q.selected);
    }

    SECTION("pattern") {
        string query = "print p; assign a; Select a pattern a(\"x\", _)";
        Query q = QueryPreProcessor::getInstance()->constructQuery(query);
        unordered_map<string, DesignEntity> syn_entity_map = {
                {"a", DesignEntity::Assign},
                {"p", DesignEntity::Print}};
        PatternAssign pttn = PatternAssign("a", "\"x\"", "_");
        vector<string> selected = {"a"};
        REQUIRE(syn_entity_map == q.synonyms);
        REQUIRE(q.clauses.size() == 1);
        REQUIRE(pttn.equals((PatternAssign*)q.clauses[0]));
        REQUIRE(selected == q.selected);
    }

    SECTION("rel + pattern, one common syn") {
        string query = "assign x; variable v; print pn; Select v such that Follows*(pn, x) pattern x(v, \"y\")";
        Query q = QueryPreProcessor::getInstance()->constructQuery(query);
        unordered_map<string, DesignEntity> syn_entity_map = {
                {"x", DesignEntity::Assign},
                {"v", DesignEntity::Variable},
                {"pn", DesignEntity::Print}};
        FollowsStarClause rel = FollowsStarClause("pn", "x");
        PatternAssign pttn = PatternAssign("x", "v", "\"y\"");
        vector<string> selected = {"v"};
        REQUIRE(syn_entity_map == q.synonyms);
        REQUIRE(q.clauses.size() == 2);
        REQUIRE(rel.equals((FollowsStarClause*)q.clauses[0]));
        REQUIRE(pttn.equals((PatternAssign*)q.clauses[1]));
        REQUIRE(selected == q.selected);
    }

    SECTION("rel + pattern, one common syn") {
        string query = "assign x; variable v; if i; Select i such that Uses(i, v) pattern x(v, \"y\")";
        Query q = QueryPreProcessor::getInstance()->constructQuery(query);
        unordered_map<string, DesignEntity> syn_entity_map = {
                {"x", DesignEntity::Assign},
                {"v", DesignEntity::Variable},
                {"i", DesignEntity::If}};
        UsesClause rel = UsesClause("i", "v");
        PatternAssign pttn = PatternAssign("x", "v", "\"y\"");
        vector<string> selected = {"i"};
        REQUIRE(syn_entity_map == q.synonyms);
        REQUIRE(q.clauses.size() == 2);
        REQUIRE(rel.equals((UsesClause*)q.clauses[0]));
        REQUIRE(pttn.equals((PatternAssign*)q.clauses[1]));
        REQUIRE(selected == q.selected);
    }

    SECTION("rel + pattern, two common syns") {
        string query = "assign a1; variable v; while w; Select w such that Modifies(a1, v) pattern a1(v, \"y\")";
        Query q = QueryPreProcessor::getInstance()->constructQuery(query);
        unordered_map<string, DesignEntity> syn_entity_map = {
                {"a1", DesignEntity::Assign},
                {"v", DesignEntity::Variable},
                {"w", DesignEntity::While}};
        ModifiesClause rel = ModifiesClause("a1", "v");
        PatternAssign pttn = PatternAssign("a1", "v", "\"y\"");
        vector<string> selected = {"w"};
        REQUIRE(syn_entity_map == q.synonyms);
        REQUIRE(q.clauses.size() == 2);
        REQUIRE(rel.equals((ModifiesClause*)q.clauses[0]));
        REQUIRE(pttn.equals((PatternAssign*)q.clauses[1]));
        REQUIRE(selected == q.selected);
    }

    SECTION("modifies/uses p") {
        string query = "variable v; procedure p;\nSelect p such that Modifies(p,v)";
        Query q = QueryPreProcessor::getInstance()->constructQuery(query);
        unordered_map<string, DesignEntity> syn_entity_map = {
                {"p", DesignEntity::Procedure},
                {"v", DesignEntity::Variable}};
        ModifiesClause m = ModifiesClause("p", "v");
        vector<string> selected = {"p"};
        REQUIRE(syn_entity_map == q.synonyms);
        REQUIRE(q.clauses.size() == 1);
        REQUIRE(m.equals((ModifiesClause*)q.clauses[0]));
        REQUIRE(selected == q.selected);
    }

}


TEST_CASE("complex query") {
    SECTION("common syn") {
        string query = "assign a; variable v; Select a such that Uses(a,_) pattern a(v, _)";
        Query q = QueryPreProcessor::getInstance()->constructQuery(query);
        unordered_map<string, DesignEntity> syn_entity_map = {
                {"a", DesignEntity::Assign},
                {"v", DesignEntity::Variable}};
        UsesClause u = UsesClause("a", "_");
        PatternAssign p = PatternAssign("a", "v", "_");
        vector<string> selected = {"a"};
        REQUIRE(syn_entity_map == q.synonyms);
        REQUIRE(q.clauses.size() == 2);
        REQUIRE(u.equals((UsesClause *) q.clauses[0]));
        REQUIRE(p.equals((PatternAssign *) q.clauses[1]));
        REQUIRE(selected == q.selected);
    }

    SECTION("system testing 164") {
        string query = "read r; print pn; while w; if ifs; assign a; variable v; procedure p; stmt s; constant c; Select v pattern a(_,_\"while\"_) such that Modifies(a,v) ";
        Query q = QueryPreProcessor::getInstance()->constructQuery(query);
        unordered_map<string, DesignEntity> syn_entity_map = {
                {"r",   DesignEntity::Read},
                {"pn",  DesignEntity::Print},
                {"w",   DesignEntity::While},
                {"ifs", DesignEntity::If},
                {"a",   DesignEntity::Assign},
                {"v",   DesignEntity::Variable},
                {"p",   DesignEntity::Procedure},
                {"s",   DesignEntity::Stmt},
                {"c",   DesignEntity::Constant}};
        ModifiesClause rel = ModifiesClause("a", "v");
        PatternAssign pttn = PatternAssign("a", "_", "_\"while\"_");
        vector<string> selected = {"v"};
        REQUIRE(syn_entity_map == q.synonyms);
        REQUIRE(q.clauses.size() == 2);
        REQUIRE(rel.equals((ModifiesClause *) q.clauses[0]));
        REQUIRE(pttn.equals((PatternAssign *) q.clauses[1]));
        REQUIRE(selected == q.selected);
    }

    SECTION("system test 428") {
        string query = "while w; assign aa; stmt s; Select aa pattern aa(_, _) such that Follows*(w,aa)";
        Query q = QueryPreProcessor::getInstance()->constructQuery(query);
        unordered_map<string, DesignEntity> syn_entity_map = {
                {"w",   DesignEntity::While},
                {"aa",   DesignEntity::Assign},
                {"s",   DesignEntity::Stmt}};
        FollowsStarClause rel = FollowsStarClause("w", "aa");
        PatternAssign pttn = PatternAssign("aa", "_", "_");
        vector<string> selected = {"aa"};
        REQUIRE(syn_entity_map == q.synonyms);
        REQUIRE(q.clauses.size() == 2);
        REQUIRE(rel.equals((FollowsStarClause *) q.clauses[0]));
        REQUIRE(pttn.equals((PatternAssign *) q.clauses[1]));
        REQUIRE(selected == q.selected);
    }

    SECTION("repeated syn") {
        string query = "assign a; while a; variable v; Select v";
        REQUIRE_THROWS(QueryPreProcessor::getInstance() -> constructQuery(query));
    }
}


#include "catch.hpp"
#include "query_processor/query_preprocessor/QuerySyntaxChecker.h"
#include "utils/Constants.h"


using namespace std;

TEST_CASE("check valid query structure") {

    string s1 = "stmt s;";
    REQUIRE(QuerySyntaxChecker::checkQueryStructure(s1) == false);

    string s2 = "stmt s; Select a";
    REQUIRE(QuerySyntaxChecker::checkQueryStructure(s2) == true);

    string s3 = "stmt     s  ;   Select   a ";
    REQUIRE(QuerySyntaxChecker::checkQueryStructure(s3) == true);

    string s4 = "stmt select; Select select";
    REQUIRE(QuerySyntaxChecker::checkQueryStructure(s4) == true);

    string s5 = ";Select";
    REQUIRE(QuerySyntaxChecker::checkQueryStructure(s5) == false);

    string s6 = "; Select such that ";
    REQUIRE(QuerySyntaxChecker::checkQueryStructure(s6) == false);

    string s8 = "assign a  ; Select pattern ";
    REQUIRE(QuerySyntaxChecker::checkQueryStructure(s8) == true);

    string s10 = "statement s; assign a; Select pattern a(v, v)";
    REQUIRE(QuerySyntaxChecker::checkQueryStructure(s10) == false);
}

TEST_CASE("get valid declaration") {

    string s1 = "stmt s; Select s";
    REQUIRE(QuerySyntaxChecker::getValidDeclaration(s1) == "stmt s; ");

    string s2 = "stmt s; assign a; Select s";
    REQUIRE(QuerySyntaxChecker::getValidDeclaration(s2) == "stmt s; assign a; ");

    string s3 = "stmt s1, s2; assign a; Select s";
    REQUIRE(QuerySyntaxChecker::getValidDeclaration(s3) == "stmt s1, s2; assign a; ");

    string s4 = "stmt   s      ;     assign a   ; Select s";
    REQUIRE(QuerySyntaxChecker::getValidDeclaration(s4) == "stmt   s      ;     assign a   ; ");

    string s5 = "stmt Select,   Select2; assign a; Select Select";
    REQUIRE(QuerySyntaxChecker::getValidDeclaration(s5) == "stmt Select,   Select2; assign a; ");

    string s6 = "stmt s; assign a; procedure p; while w; if ifs; variable v; read r; print pr; call call; constant const; Select s";
    REQUIRE(QuerySyntaxChecker::getValidDeclaration(s6) == "stmt s; assign a; procedure p; while w; if ifs; variable v; read r; print pr; call call; constant const; ");

    string s7 = "stmt; Select s";
    REQUIRE(QuerySyntaxChecker::getValidDeclaration(s7).empty());

    string s8 = "s; Select s";
    REQUIRE(QuerySyntaxChecker::getValidDeclaration(s8).empty());

    string s9 = "; Select s";
    REQUIRE(QuerySyntaxChecker::getValidDeclaration(s9).empty());

    string s10 = "statement s; Select s";
    REQUIRE(QuerySyntaxChecker::getValidDeclaration(s10).empty());

}

TEST_CASE("get valid selected synonym") {

    string s1 = "stmt s; Select s";
    REQUIRE(QuerySyntaxChecker::getValidSelected(s1) == "s");

    string s2 = "stmt s; Select s such that";
    REQUIRE(QuerySyntaxChecker::getValidSelected(s2) == "s");

    string s3 = "stmt s; Select s pattern a(v,_)";
    REQUIRE(QuerySyntaxChecker::getValidSelected(s3) == "s");

    string s4 = "stmt s;     Select       s     ";
    REQUIRE(QuerySyntaxChecker::getValidSelected(s4) == "s");

    string s5 = "stmt a;     Select       s     ";
    REQUIRE(QuerySyntaxChecker::getValidSelected(s5) == "s");


}

TEST_CASE("get valid relational clause") {

    string s1 = "stmt s; Select s such that Follows(3, 4)";
    REQUIRE(QuerySyntaxChecker::getValidRelClauseGroups(s1)[0] == "such that Follows(3, 4)");

    string s2 = "stmt s; Select s such that Follows*(3, 4)";
    REQUIRE(QuerySyntaxChecker::getValidRelClauseGroups(s2)[0] == "such that Follows*(3, 4)");

    string s3 = "stmt s; Select s such that Parent(_, 4) pattern a(v, v)";
    REQUIRE(QuerySyntaxChecker::getValidRelClauseGroups(s3)[0] == "such that Parent(_, 4)");

    string s4 = "stmt s; Select s such that Parent*(s, 4)";
    REQUIRE(QuerySyntaxChecker::getValidRelClauseGroups(s4)[0] == "such that Parent*(s, 4)");

    string s5 = "stmt s; Select s such that Modifies(s, w) pattern a(v, v)";
    REQUIRE(QuerySyntaxChecker::getValidRelClauseGroups(s5)[0] == "such that Modifies(s, w)");

    string s6 = "stmt s; Select s such that Uses(s, _)";
    REQUIRE(QuerySyntaxChecker::getValidRelClauseGroups(s6)[0] == "such that Uses(s, _)");

    string s7 = "stmt s; Select s such that Follows* (    3, s )     ";
    REQUIRE(QuerySyntaxChecker::getValidRelClauseGroups(s7)[0] == "such that Follows* (    3, s )");

    string s8 = "stmt s; Select s such that    Follows(s, w)";
    REQUIRE(QuerySyntaxChecker::getValidRelClauseGroups(s8)[0] == "such that    Follows(s, w)");

    string s9 = "stmt s; Select s such  that Follows(s, _)";
    REQUIRE(QuerySyntaxChecker::getValidRelClauseGroups(s9).empty());

    string s10 = "stmt s; Select s such that Uses*(s, _)";
    REQUIRE(QuerySyntaxChecker::getValidRelClauseGroups(s10).empty());

    string s11 = "stmt s; Select s such that Modifies*(s, _)";
    REQUIRE(QuerySyntaxChecker::getValidRelClauseGroups(s11).empty());

}

TEST_CASE("get valid pattern clause") {
    string s1 = "assign a; Select a pattern a(v, v)";
    REQUIRE(QuerySyntaxChecker::getValidPatternClauseGroups(s1).empty());

    string s2 = "assign a; Select a pattern a(_, v)";
    REQUIRE(QuerySyntaxChecker::getValidPatternClauseGroups(s2).empty());

    string s3 = "assign a; Select a pattern a(v, _)";
    REQUIRE(QuerySyntaxChecker::getValidPatternClauseGroups(s3)[0] == "pattern a(v, _)");

    string s4 = "assign a; Select a such that Follows(3, 4) pattern a(v, v)";
    REQUIRE(QuerySyntaxChecker::getValidPatternClauseGroups(s4).empty());

    string s5 = "assign a; Select a   pattern    a(  v, v  )   ";
    REQUIRE(QuerySyntaxChecker::getValidPatternClauseGroups(s5).empty());

    string s6 = "assign a; Select a pattern a(v, \"x\")";
    REQUIRE(QuerySyntaxChecker::getValidPatternClauseGroups(s6)[0] == "pattern a(v, \"x\")");

    string s7 = "assign a; Select a pattern a(v, \"x+y\")";
    REQUIRE(QuerySyntaxChecker::getValidPatternClauseGroups(s7)[0] == "pattern a(v, \"x+y\")");

    string s8 = "assign a; Select a pattern a(v, _\"x\"_)";
    REQUIRE(QuerySyntaxChecker::getValidPatternClauseGroups(s8)[0] == "pattern a(v, _\"x\"_)");

    string s9 = "assign a; Select a pattern s(v, v)";
    REQUIRE(QuerySyntaxChecker::getValidPatternClauseGroups(s9).empty());

}

TEST_CASE("check remaining") {
    SECTION("such that before pattern") {
        string query = "assign a; variable v; Select a such that Uses(a,_) pattern a(v, _)";
        REQUIRE_FALSE(QuerySyntaxChecker::hasRemaining(query));
    }

    SECTION("pattern before such that") {
        string query = "while w; assign aa; stmt s; Select aa pattern aa(_, _) such that Follows*(w,aa)";
        REQUIRE_FALSE(QuerySyntaxChecker::hasRemaining(query));
    }

    SECTION("random string between such that and pattern") {
        string query = "stmt s; assign a; Select s such that Follows(s, 3) その血の運命 pattern a(\"x\", _)";
        REQUIRE(QuerySyntaxChecker::hasRemaining(query));
    }

    SECTION("random string between pattern and such that") {
        string query = "stmt s; assign a; Select s pattern a(\"x\", _) その血の運命 such that Follows(s, 3)";
        REQUIRE(QuerySyntaxChecker::hasRemaining(query));
    }

    SECTION("pattern before such that, trailing chars") {
        string query = "stmt s; assign a; Select s pattern a(\"x\", _) such that Follows(s, 3) このDIOだ";
        REQUIRE(QuerySyntaxChecker::hasRemaining(query));
    }

    SECTION("such that before pattern, trailing chars") {
        string query = "stmt s; assign a; Select s such that Follows(s, 3)　pattern a(\"x\", _) 無駄無駄無駄無駄 ";
        REQUIRE(QuerySyntaxChecker::hasRemaining(query));
    }

    SECTION("pattern before such that") {
        string query = "stmt s; assign a; Select s pattern a(\"x\", _) such that Follows(s, 3)";
        REQUIRE_FALSE(QuerySyntaxChecker::hasRemaining(query));
    }
}

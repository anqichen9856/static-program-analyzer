#include "catch.hpp"
#include "query_processor/query_preprocessor/QueryParser.h"
#include "utils/Constants.h"
#include <iostream>

using namespace std;

TEST_CASE("isMatched") {
    SECTION("query regex") {
        string s1 = "stmt s;  Select a";
        REQUIRE(QueryParser::isMatched(s1, REGEX_QUERY) == true);
    }

    SECTION("select regex") {
        string s2 = "Select  a";
        REQUIRE(QueryParser::isMatched(s2, REGEX_SELECT) == true);
    }

    SECTION("rel regex") {
        string s3 = "such that    Follows(a, w)";
        REQUIRE(QueryParser::isMatched(s3, REGEX_REL_CLAUSE) == true);

        string s4 = "such that    Follows(3, s)";
        REQUIRE(QueryParser::isMatched(s4, REGEX_REL_CLAUSE) == true);

        string s5 = "such  that Follows(3, s)";
        REQUIRE(QueryParser::isMatched(s5, REGEX_REL_CLAUSE) == false);

        string s6 = "such that follows(3, s)";
        REQUIRE(QueryParser::isMatched(s6, REGEX_REL_CLAUSE) == false);

        string s7 = "such that Follows(3, s) and Modifies(a,_)";
        REQUIRE(QueryParser::isMatched(s4, REGEX_REL_CLAUSE) == true);
    }

    SECTION("pattern regex") {
        string s7 = "pattern a(_,_)";
        REQUIRE(QueryParser::isMatched(s7, REGEX_PATTERN) == true);

        string s8 = "pattern s(v,_)";
        REQUIRE(QueryParser::isMatched(s8, REGEX_PATTERN) == true);

        string s9 = "pattern a(\"x\", s)";
        REQUIRE(QueryParser::isMatched(s9, REGEX_PATTERN) == false);

        string s10 = "pattern a(x, _\"y\"_)";
        REQUIRE(QueryParser::isMatched(s10, REGEX_PATTERN) == true);

        string s11 = "pattern a(x, _\"y\")";
        REQUIRE(QueryParser::isMatched(s11, REGEX_PATTERN) == false);
    }

    SECTION("pattern if regex") {
        string s1 = "ifs(_,_,_)";
        REQUIRE(QueryParser::isMatched(s1, REGEX_PATTERN_IF) == true);

        string s2 = "ifs(_,_,x)";
        REQUIRE(QueryParser::isMatched(s2, REGEX_PATTERN_IF) == false);

        string s3 = "ifs(_,y,_)";
        REQUIRE(QueryParser::isMatched(s3, REGEX_PATTERN_IF) == false);

        string s4 = "ifs(x,_,_)";
        REQUIRE(QueryParser::isMatched(s4, REGEX_PATTERN_IF) == true);

        string s5 = "ifs(\"x\",_,_)";
        REQUIRE(QueryParser::isMatched(s5, REGEX_PATTERN_IF) == true);
    }

    SECTION("pattern while regex") {
        string s1 = "w(_,_,_)";
        REQUIRE(QueryParser::isMatched(s1, REGEX_PATTERN_ASSIGN_WHILE) == false);

        string s2 = "w(_,_)";
        REQUIRE(QueryParser::isMatched(s2, REGEX_PATTERN_ASSIGN_WHILE) == true);

        string s3 = "w(_,y)";
        REQUIRE(QueryParser::isMatched(s3, REGEX_PATTERN_ASSIGN_WHILE) == false);

        string s4 = "w(x,_)";
        REQUIRE(QueryParser::isMatched(s4, REGEX_PATTERN_ASSIGN_WHILE) == true);

        string s5 = "w(\"x\",_)";
        REQUIRE(QueryParser::isMatched(s5, REGEX_PATTERN_ASSIGN_WHILE) == true);
    }

}

TEST_CASE("getMatched") {

    string s = "assign a; Select a such that ";
    string answer1 = "Select a";
    string answer2 = "assign a; ";
    REQUIRE(QueryParser::getMatched(s, REGEX_SELECT) == answer1);
    REQUIRE(QueryParser::getMatched(s, REGEX_PATTERN).empty());
    REQUIRE(QueryParser::getMatched(s, REGEX_PATTERN_IF).empty());
    REQUIRE(QueryParser::getMatched(s, REGEX_PATTERN_ASSIGN_WHILE).empty());
    REQUIRE(QueryParser::getMatched(s, REGEX_DECL) == answer2);
    REQUIRE(QueryParser::getMatched(s, REGEX_REL_CLAUSE).empty());
    REQUIRE(QueryParser::getMatched(s, REGEX_WITH).empty());

    string s1 = "assign a; Select a such that Follows(3, a)";
    string answer3 = " Follows(3, a)";
    REQUIRE(QueryParser::getMatched(s1, REGEX_FOLLOWS) == answer3);

    string s2 = "assign a; Select a pattern a(v,    _)";
    string answer4 = "pattern a(v,    _)";
    string answer5 = "a(v,    _)";
    REQUIRE(QueryParser::getMatched(s2, REGEX_PATTERN) == answer4);
    REQUIRE(QueryParser::getMatched(s2, REGEX_PATTERN_ASSIGN_WHILE) == answer5);
    REQUIRE(QueryParser::getMatched(s2, REGEX_PATTERN_IF).empty());

    string s3 = "assign a; Select a such that Parent(3, a)";
    string answer6 = " Parent(3, a)";
    REQUIRE(QueryParser::getMatched(s3, REGEX_PARENT) == answer6);

}


TEST_CASE("getMatchedMulti") {
    SECTION("basic string") {
        string s = "ab1df9d3dsa4";
        string rgx = R"(\d)";
        vector<string> v = QueryParser::getMatchedMulti(s, rgx);
        REQUIRE(v.size() == 4);
        REQUIRE(v[0] == "1");
        REQUIRE(v[1] == "9");
        REQUIRE(v[2] == "3");
        REQUIRE(v[3] == "4");
    }

    SECTION("simple query") {
        string s = "assign a; variable v; Select a such that Uses(a,_) and Modifies(a,v) and Follows(3,4) pattern a(v, _) such that Follows(3,5) and Uses(a,v)";
        vector<string> v = QueryParser::getMatchedMulti(s, REGEX_REL_CLAUSE);
        REQUIRE(v.size() == 2);
        REQUIRE(v[0] == "such that Uses(a,_) and Modifies(a,v) and Follows(3,4)");
        REQUIRE(v[1] == "such that Follows(3,5) and Uses(a,v)");
    }

}

TEST_CASE("tokenizedBy") {

    vector<string> s1 = QueryParser::tokenizeBy("assign a; Select a such that ", ";");
    vector<string> answer1 = {"assign a", " Select a such that "};
    REQUIRE(s1[0] == answer1[0]);
    REQUIRE(s1[1] == answer1[1]);

    vector<string> s2 = QueryParser::tokenizeBy("Parent(3, a)", "(");
    vector<string> answer2 = {"Parent", "3, a)"};
    REQUIRE(s2[0] == answer2[0]);
    REQUIRE(s2[1] == answer2[1]);

    vector<string> s3 = QueryParser::tokenizeBy("3, a)", ",");
    vector<string> answer3 = {"3", " a)"};
    REQUIRE(s3[0] == answer3[0]);
    REQUIRE(s3[1] == answer3[1]);

    vector<string> s4 = QueryParser::tokenizeBy("assign        a         ", " ");
    vector<string> answer4 = {"assign", "a"};
    REQUIRE(s4[0] == answer4[0]);
    REQUIRE(s4[1] == answer4[1]);
}

TEST_CASE("is substring") {
    string sub = "such that";
    string full = " such that Follows(3,4)";
    REQUIRE(QueryParser::isSubString(full, sub));
}

TEST_CASE("remove spaces") {
    string s = "   \"    x    \"   ";
    REQUIRE(QueryParser::removeSpaces(s) == "\"x\"");
}

TEST_CASE("remove matched") {
    string query = "assign a; variable v; Select a such that Uses(a,_) pattern a(v, _)";
    string removed = "Select a such that Uses(a,_) pattern a(v, _)";
    REQUIRE(QueryParser::removeMatched(query, REGEX_DECL) == removed);
}
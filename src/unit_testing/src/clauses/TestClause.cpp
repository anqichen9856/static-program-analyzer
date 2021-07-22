#include <clauses/UsesClause.h>
#include "clauses/Clause.h"
#include "catch.hpp"

using namespace std;

TEST_CASE("strip"){
    Clause c = Clause("abc", "abc");

    string s = "\"procedureName\"";
    string strip = c.strip(s);
    REQUIRE(strip == "procedureName");
    REQUIRE(c.strip(strip) == "procedureName");

    string partial = "_\"x+y+z\"_";
    REQUIRE(c.strip(partial) == "_x+y+z_");

    string num = "\"100\"";
    REQUIRE(c.strip(num) == "100");

    string expr = "\"(easy + question) / mark\"";
    REQUIRE(c.strip(expr) == "(easy + question) / mark");

};


TEST_CASE("isName"){
    Clause c = Clause("abc", "abc");
    REQUIRE(c.isName("\"abc\"")==true);

    UsesClause u = UsesClause("s", "v");
    REQUIRE(u.isName(u.getLeftParam()) == false );
    REQUIRE(u.isName(u.getRightParam()) == false );
    REQUIRE((u.isName(u.getRightParam()) && u.isSynonym(u.getRightParam())) == false);
};

TEST_CASE("find intersect") {

    vector<string> res = {"1", "2", "3"};
    vector<string> temp = {"2", "4", "1"};
    vector<string> empty = {};

    vector<string> answer1 = {"1", "2"};
    vector<string> answer2 = {};

    REQUIRE(Clause::findIntersect(res, temp) == answer1);
    REQUIRE(Clause::findIntersect(res, empty) == answer2);

}

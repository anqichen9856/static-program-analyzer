
#include "query_processor/query_evaluator/QueryResult.h"
#include "catch.hpp"

TEST_CASE("Insert 1 Syn") {
    vector<string> initial_row = vector<string>({"1"});
    QueryResult query_result = QueryResult(unordered_map<string, int>({}), list<vector<string>>({}));
    vector<string> vector_to_add2 = vector<string>({"1"});
    query_result.addOneSyn("a", vector_to_add2);

    vector<string> vector_to_add = vector<string>({"1", "2"});
    query_result.addOneSyn("b", vector_to_add);
    REQUIRE(query_result.cols.size() == 2);
    REQUIRE(query_result.rows.size() == 2);

    query_result.addOneSyn("c", vector_to_add);
    REQUIRE(query_result.cols.size() == 3);
    REQUIRE(query_result.rows.size() == 4);

    query_result.addOneSyn("d", vector_to_add);
    REQUIRE(query_result.cols.size() == 4);
    REQUIRE(query_result.rows.size() == 8);

}

TEST_CASE("Add 2 syns") {
    vector<string> initial_row = vector<string>({"1"});
    QueryResult query_result = QueryResult(unordered_map<string, int>({{"a", 0}}), list<vector<string>>({initial_row}));

    vector<pair<string, string>> vector_to_add = vector<pair<string, string>>({{"1", "2"}, {"2", "3"}});
    query_result.addTwoSyn("b", "c", vector_to_add);
    REQUIRE(query_result.cols.size() == 3);
    REQUIRE(query_result.rows.size() == 2);

}

TEST_CASE("Filter 1 syn") {
    vector<string> initial_row = vector<string>({"1"});
    QueryResult query_result = QueryResult(unordered_map<string, int>({{"a", 0}}), list<vector<string>>({initial_row}));
    vector<string> vector_to_add = vector<string>({"1", "2"});
    query_result.addOneSyn("b", vector_to_add);
    query_result.addOneSyn("c", vector_to_add);
    query_result.filterOneSyn("c", [](string value) { return value != "2"; });
}

TEST_CASE("Get Result") {
    vector<string> initial_row = vector<string>({"1"});
    QueryResult query_result = QueryResult(unordered_map<string, int>({{"a", 0}}), list<vector<string>>({initial_row}));

    vector<string> vector_to_add = vector<string>({"0", "1"});
    query_result.addOneSyn("b", vector_to_add);
    query_result.addOneSyn("c", vector_to_add);
    query_result.addOneSyn("d", vector_to_add);

    vector<string> result = query_result.getResults(vector<string>({"a", "b", "c", "d"}));
    REQUIRE(result[4] == "1 1 0 0");
    vector<string> result2 = query_result.getResults(vector<string>({"a"}));
    REQUIRE(result2.size() == 1);

}

TEST_CASE("empty table") {
    QueryResult query_result =  QueryResult(unordered_map<string, int>({}), list<vector<string>>());
    vector<string> vector_to_add = vector<string>({"1", "2"});
    query_result.addOneSyn("b", vector_to_add);
    REQUIRE(query_result.cols.size() == 1);
    vector<string> answer = {"1", "2"};
    REQUIRE(query_result.getResults({"b"}) == answer);
    REQUIRE(query_result.rows.size() == 2);

}

TEST_CASE("test table") {
    QueryResult query_result =  QueryResult(unordered_map<string, int>({}), list<vector<string>>());
    vector<string> vector_to_add = vector<string>({"1", "2"});
    query_result.addOneSyn("b", vector_to_add);
    REQUIRE(query_result.cols.size() == 1);
    vector<string> answer = {"1", "2"};
    REQUIRE(query_result.getResults({"b"}) == answer);
    REQUIRE(query_result.rows.size() == 2);
    query_result.addAtrribute("b", "stmt#", [](string a){return "a";});
}

TEST_CASE("Filter and Add") {
    vector<string> initial_row = vector<string>({"1"});
    QueryResult query_result = QueryResult(unordered_map<string, int>({{"a", 0}}), list<vector<string>>({initial_row}));

    vector<pair<string, string>> vector_to_add = vector<pair<string, string>>({{"1", "2"}, {"2", "3"}});
    query_result.addTwoSyn("b", "c", vector_to_add);
    REQUIRE(query_result.cols.size() == 3);
    REQUIRE(query_result.rows.size() == 2);

    query_result.filterAndAdd("b", "d", [](string target){if (target == "1") {return vector<string>({"s", "q"});} return vector<string>();});

}
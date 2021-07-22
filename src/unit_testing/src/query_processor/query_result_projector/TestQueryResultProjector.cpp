#include "catch.hpp"
#include "query_processor/query_result_projector/QueryResultProjector.h"

TEST_CASE("format results") {

    list<string> results;
    Query query;
    query.synonyms = {
            {"s", DesignEntity::Stmt},
            {"v", DesignEntity::Variable}
    };
    query.selected = {"s"};
    query.clauses = vector<Clause*>();

    QueryResult query_res = QueryResult(unordered_map<string, int>({{"s", 0}, {"v", 1}}),
                                        {{"1", "x"}, {"1", "y"}, {"2", "z"}});

    QueryResultProjector::getInstance()->formatResults(results, &query_res, query, true);
    list<string> answer = {"1", "2"};
    REQUIRE(results == answer);

    query_res = QueryResult(unordered_map<string, int>({{"s", 0}, {"v", 1}}),
                                        {{"1", "x"}, {"1", "y"}, {"2", "z"}});

    query.selected = {"v"};
    results.clear();
    QueryResultProjector::getInstance()->formatResults(results, &query_res, query, true);
    list<string> answer1 = {"x", "y", "z"};
    REQUIRE(results == answer1);

    query_res = QueryResult(unordered_map<string, int>({{"s", 0}, {"v", 1}}),
                            {{"1", "x"}, {"1", "y"}, {"2", "z"}});

    query.selected = {"s", "v"};
    results.clear();
    QueryResultProjector::getInstance()->formatResults(results, &query_res, query, true);
    list<string> answer2 = {"1 x", "1 y", "2 z"};
    REQUIRE(results == answer2);

    query_res = QueryResult(unordered_map<string, int>({{"s", 0}, {"v", 1}}),
                            {{"1", "x"}, {"1", "y"}, {"2", "z"}});

    query.selected = {"s", "v"};
    results.clear();
    QueryResultProjector::getInstance()->formatResults(results, &query_res, query, false);
    list<string> answer3 = {};
    REQUIRE(results == answer3);
}

TEST_CASE("select boolean") {

    list<string> results;
    results.clear();
    Query query;
    query.synonyms = {
            {"s", DesignEntity::Stmt},
            {"v", DesignEntity::Variable}
    };
    query.selected = {"BOOLEAN"};
    query.clauses = vector<Clause*>();

    vector<string> initial_row = vector<string>({"1"});
    QueryResult query_res = QueryResult(unordered_map<string, int>({{"s", 0}, {"v", 1}}),
                                        {{"1", "x"}, {"1", "y"}, {"2", "z"}});

    QueryResultProjector::getInstance()->formatResults(results, &query_res, query, true);
    list<string> answer = {"TRUE"};
    REQUIRE(results == answer);

    results.clear();
    QueryResultProjector::getInstance()->formatResults(results, &query_res, query, false);
    list<string> answer1 = {"FALSE"};
    REQUIRE(results == answer1);

    query_res = QueryResult(unordered_map<string, int>({{"s", 0}, {"v", 1}}),
                            {});
    results.clear();
    QueryResultProjector::getInstance()->formatResults(results, &query_res, query, true);
    list<string> answer2 = {"TRUE"};
    REQUIRE(results == answer2);

}
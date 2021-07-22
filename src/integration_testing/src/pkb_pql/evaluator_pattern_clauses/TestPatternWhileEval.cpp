#include <vector>
#include "front_end/tokenizer/Tokenizer.h"
#include "front_end/parser/Parser.h"
#include "catch.hpp"
#include "query_processor/Query.h"
#include "utils/SPAEnums.h"
#include "clauses/PatternWhile.h"
#include "program_knowledge_base/pkb_storage/PKB.h"
#include "query_processor/query_evaluator/QueryEvaluator.h"

TEST_CASE("pattern while") {

    SECTION("wildcard") {
        string src = "procedure Example {\n"
                     "  while(x < z+y) { \n"  //1
                     "      x = x - 3;\n" //2
                     "  } \n"
                     "  if (m == j+k+l) then { \n" //3
                     "      z = x; }\n" //4
                     "  else {\n"
                     "      z = x; \n"
                     "  }\n" //5
                     "  while((x < z+y) && (m!=1)) { \n" //6
                     "      while (i == 1) { \n" //7
                     "          x = 1;"
                     "      }\n"  //8
                     "      x = x - 3;\n" //9
                     "  }\n"
                     "} \n";

        Tokenizer *tokenizer = Tokenizer::getInstance();
        vector<Token> tokenStream = tokenizer->tokenizeSource(src);
        Parser *parser = Parser::getInstance();
        AST root = parser->buildAST(tokenStream);
        PKB *pkb = PKB::getInstance();
        pkb->reset();
        pkb->initAST(root);

        QueryResult *query_res = QueryResult::init();
        Query query;
        query.synonyms = {
                {"w", DesignEntity::While}
        };
        query.selected = {"w"};
        query.clauses = vector<Clause *>();

        PatternWhile p = PatternWhile("w", "_", "_");
        query.clauses.push_back(&p);

        vector<string> answer = {"1", "6", "7"};
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, query_res));
        REQUIRE(query_res->getResults(query.selected) == answer);
    }

    SECTION("synonym var") {
        QueryResult *query_res = QueryResult::init();
        Query query;
        query.synonyms = {
                {"w", DesignEntity::While},
                {"v", DesignEntity::Variable}
        };
        query.selected = {"w"};
        query.clauses = vector<Clause *>();

        PatternWhile p = PatternWhile("w", "v", "_");
        query.clauses.push_back(&p);

        vector<string> answer = {"1", "6", "7"};
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, query_res));
        REQUIRE(query_res->getResults(query.selected) == answer);

        query.selected = {"w", "v"};
        QueryResult *query_res1 = QueryResult::init();
        vector<string> answer1 = {"1 x", "1 y", "1 z", "6 m", "6 x", "6 y", "6 z", "7 i"};
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, query_res1));
        REQUIRE(query_res1->getResults(query.selected) == answer1);
    }

    SECTION("var name") {
        QueryResult *query_res = QueryResult::init();
        Query query;
        query.synonyms = {
                {"w", DesignEntity::While},
        };
        query.selected = {"w"};
        query.clauses = vector<Clause *>();

        PatternWhile p = PatternWhile("w", "\"m\"", "_");
        query.clauses.push_back(&p);

        vector<string> answer = {"6"};
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, query_res));
        REQUIRE(query_res->getResults(query.selected) == answer);
    }

    SECTION("filter w") {
        QueryResult query_res = QueryResult(unordered_map<string, int>({{"w", 0},
                                                                        {"a", 1}}),
                                            {{"1",  "2"},
                                             {"1",  "3"},
                                             {"6",  "5"},
                                             {"13", "7"}});
        Query query;
        query.synonyms = {
                {"w", DesignEntity::While},
        };
        query.selected = {"w"};
        query.clauses = vector<Clause *>();

        PatternWhile p = PatternWhile("w", "\"m\"", "_");
        query.clauses.push_back(&p);

        vector<string> answer = {"6"};

        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, &query_res));
        REQUIRE(query_res.getResults(query.selected) == answer);
    }

    SECTION("filter w,v") {
        QueryResult query_res = QueryResult(unordered_map<string, int>({{"w", 0}, {"v", 1}}),
                                            {{"1", "l"}, {"1", "x"}, {"6", "m"}, {"13", "y"}});
        Query query;
        query.synonyms = {
                {"w", DesignEntity::While},
                {"v", DesignEntity::Variable}
        };
        query.selected = {"w", "v"};
        query.clauses = vector<Clause *>();

        PatternWhile p = PatternWhile("w", "v", "_");
        query.clauses.push_back(&p);

        vector<string> answer = {"1 x", "6 m"};
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, &query_res));
        REQUIRE(query_res.getResults(query.selected) == answer);


    }
}

TEST_CASE("while without control var") {
    string src = "procedure Example {\n"
                 "  while(1 < 2) { \n"  //1
                 "      x = x - 3;\n" //2
                 "  } \n"
                 "  if (m == j+k+l) then { \n" //3
                 "      z = x; }\n" //4
                 "  else {\n"
                 "      z = x; \n"
                 "  }\n" //5
                 "  while(2!=1) { \n" //6
                 "      while (i == 1) { \n" //7
                 "          x = 1;"
                 "      }\n"  //8
                 "      x = x - 3;\n" //9
                 "  }\n"
                 "} \n";

    Tokenizer *tokenizer = Tokenizer::getInstance();
    vector<Token> tokenStream = tokenizer->tokenizeSource(src);
    Parser *parser = Parser::getInstance();
    AST root = parser->buildAST(tokenStream);
    PKB *pkb = PKB::getInstance();
    pkb->reset();
    pkb->initAST(root);

    QueryResult *query_res = QueryResult::init();
    Query query;
    query.synonyms = {
            {"w", DesignEntity::While}
    };
    query.selected = {"w"};
    query.clauses = vector<Clause *>();

    PatternWhile p = PatternWhile("w", "_", "_");
    query.clauses.push_back(&p);

    vector<string> answer = {"7"};
    REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, query_res));
    REQUIRE(query_res->getResults(query.selected) == answer);

}
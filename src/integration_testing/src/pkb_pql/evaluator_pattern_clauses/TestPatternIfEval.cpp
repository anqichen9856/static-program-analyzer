#include <vector>
#include "front_end/tokenizer/Tokenizer.h"
#include "front_end/parser/Parser.h"
#include "catch.hpp"
#include "query_processor/Query.h"
#include "utils/SPAEnums.h"
#include "clauses/PatternIf.h"
#include "program_knowledge_base/pkb_storage/PKB.h"
#include "query_processor/query_evaluator/QueryEvaluator.h"

TEST_CASE("pattern if") {

    SECTION("wildcard") {
        string src = "procedure Example {\n"
                     "  while(x < z+y) { \n"  //1
                     "      x = x - 3;\n" //2
                     "  } \n"
                     "  if (m == j+k+l) then { \n" //3
                     "      z = x; }\n" //4
                     "  else {"
                     "      if (x < y) then {" //5
                     "          x = x+1;} " //6
                     "      else {"
                     "          x = 1;}\n" //7
                     "      z = x; }\n" //8
                     "  if(i!=1) then{ \n"  //9
                     "      x = x - 3;\n" //10
                     "  } else {"
                     "      y =1;}} \n"
        ;

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
                {"ifs", DesignEntity::If}
        };
        query.selected = {"ifs"};
        query.clauses = vector<Clause *>();

        PatternIf p = PatternIf("ifs", "_", "_", "_");
        query.clauses.push_back(&p);

        vector<string> answer = {"3", "5", "9"};
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, query_res));
        REQUIRE(query_res->getResults(query.selected) == answer);
    }

    SECTION("synonym var") {
        QueryResult *query_res = QueryResult::init();
        Query query;
        query.synonyms = {
                {"ifs", DesignEntity::If},
                {"v", DesignEntity::Variable}
        };
        query.selected = {"ifs"};
        query.clauses = vector<Clause *>();

        PatternIf p = PatternIf("ifs", "v", "_", "_");
        query.clauses.push_back(&p);

        vector<string> answer = {"3", "5", "9"};
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, query_res));
        REQUIRE(query_res->getResults(query.selected) == answer);

        query.selected = {"v"};
        vector<string> answer1 = {"i", "j", "k", "l", "m", "x", "y"};
        query_res->clear();
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, query_res));
        REQUIRE(query_res->getResults(query.selected) == answer1);

        query_res = QueryResult::init();

        query.selected = {"ifs", "v"};
        query_res->clear();
        vector<string> answer2 = {"3 j", "3 k", "3 l", "3 m", "5 x", "5 y", "9 i"};
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, query_res));
        REQUIRE(query_res->getResults(query.selected) == answer2);
    }

    SECTION("var name") {
        QueryResult *query_res = QueryResult::init();
        Query query;
        query.synonyms = {
                {"ifs", DesignEntity::If},
        };
        query.selected = {"ifs"};
        query.clauses = vector<Clause *>();

        PatternIf p = PatternIf("ifs", "\"x\"", "_", "_");
        query.clauses.push_back(&p);

        vector<string> answer = {"5"};
        QueryEvaluator::getInstance()->evaluateQuery(query, query_res);
        //REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, query_res));
        //REQUIRE(query_res->getResults(query.selected) == answer);
    }

    SECTION("filter ifs") {
        QueryResult query_res = QueryResult(unordered_map<string, int>({{"ifs", 0}, {"a", 1}}),
                                            {{"3", "2"}, {"3", "4"}, {"5", "6"}, {"9", "7"}});
        Query query;
        query.synonyms = {
                {"ifs", DesignEntity::If},
        };
        query.selected = {"ifs"};
        query.clauses = vector<Clause *>();

        PatternIf p = PatternIf("ifs", "\"x\"", "_", "_");
        query.clauses.push_back(&p);

        vector<string> answer = {"5"};
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, &query_res));
        REQUIRE(query_res.getResults(query.selected) == answer);
    }

    SECTION("filter ifs,v") {
        QueryResult query_res = QueryResult(unordered_map<string, int>({{"ifs", 0}, {"v", 1}}),
                                            {{"3", "x"}, {"5", "x"}, {"9", "x"}});
        Query query;
        query.synonyms = {
                {"ifs", DesignEntity::If},
                {"v", DesignEntity::Variable}
        };
        query.selected = {"ifs", "v"};
        query.clauses = vector<Clause *>();

        PatternIf p = PatternIf("ifs", "v", "_", "_");
        query.clauses.push_back(&p);

        vector<string> answer = {"5 x"};
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, &query_res));
        REQUIRE(query_res.getResults(query.selected) == answer);
    }

}

TEST_CASE("ifs without control var") {
    SECTION("wildcard") {
        string src = "procedure Example {\n"
                     "  while(x < z+y) { \n"  //1
                     "      x = x - 3;\n" //2
                     "  } \n"
                     "  if (m == j+k+l) then { \n" //3
                     "      z = x; }\n" //4
                     "  else {"
                     "      if (x < y) then {" //5
                     "          x = x+1;} " //6
                     "      else {"
                     "          x = 1;}\n" //7
                     "      z = x; }\n" //8
                     "  if(2!=1) then{ \n"  //9
                     "      x = x - 3;\n" //10
                     "  } else {"
                     "      y =1;}} \n";

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
                {"ifs", DesignEntity::If}
        };
        query.selected = {"ifs"};
        query.clauses = vector<Clause *>();

        PatternIf p = PatternIf("ifs", "_", "_", "_");
        query.clauses.push_back(&p);

        vector<string> answer = {"3", "5"};
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, query_res));
        REQUIRE(query_res->getResults(query.selected) == answer);
    }

    SECTION("get pair") {
        QueryResult *query_res = QueryResult::init();
        Query query;
        query.synonyms = {
                {"ifs", DesignEntity::If} ,
                {"v", DesignEntity::Variable}
        };
        query.selected = {"ifs"};
        query.clauses = vector<Clause *>();

        PatternIf p = PatternIf("ifs", "v", "_", "_");
        query.clauses.push_back(&p);

        vector<string> answer = {"3", "5"};
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, query_res));
        REQUIRE(query_res->getResults(query.selected) == answer);
    }

}

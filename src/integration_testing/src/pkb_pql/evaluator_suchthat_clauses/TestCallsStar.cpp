#include <vector>
#include "catch.hpp"
#include "front_end/tokenizer/Tokenizer.h"
#include "front_end/parser/Parser.h"
#include "query_processor/Query.h"
#include "utils/SPAEnums.h"
#include "clauses/CallsStarClause.h"
#include "query_processor/query_evaluator/QueryEvaluator.h"
#include "program_knowledge_base/pkb_storage/PKB.h"

TEST_CASE("Evaluate Calls*") {
    string src = "procedure Example {\n"
        "  x = 2;\n"  //1
        "  z = 3;\n" //2
        "  i = 5;\n" //3
        "  read x;\n" //4
        "  while (i!=0) {\n" //5
        "    x = x - 1;\n" //6
        "    if (x==1) then {\n" //7
        "      z = x + 1; \n" //8
        "      call p;}\n" //9
        "    else {\n"
        "      y = z + x; }\n" //10
        "    z = z + x + i;\n" //11
        "    i = i - 1; }\n" //12
        "  }\n"
        "\n"
        "procedure p {\n"
        "  if (k<9) then {\n" //13
        "    while (j>8) {\n" //14
        "      x = z * 3 + 2 * y;\n" //15
        "      call q; }\n" //16
        "    d = x + 1;\n" //17
        "    z = x + m; }\n"  //18
        "  else {\n"
        "    z = 1; }\n" //19
        "  z = z + x + i; }\n" //20
        "\n"
        "procedure q {\n"
        "  if (x==1) then {\n" //21
        "    z = x + 1; }\n" //22
        "  else {\n"
        "    x = z + x; }" //23
        "    x = z;}"; //24

    Tokenizer* tokenizer = Tokenizer::getInstance();
    vector<Token> tokenStream = tokenizer->tokenizeSource(src);
    Parser* parser = Parser::getInstance();
    AST root = parser->buildAST(tokenStream);
    PKB::getInstance()->reset();
    PKB::getInstance()->initAST(root);

    SECTION("Name, Name") {
        QueryResult* query_res = QueryResult::init();
        Query query;
        query.synonyms = {
            {"p",DesignEntity::Procedure},
            {"q",DesignEntity::Procedure}
        };
        query.clauses = vector<Clause*>();
        query.selected = { "p" };
        vector<string> answer = { "Example", "p", "q" };
        CallsStarClause p = CallsStarClause("\"p\"", "\"q\"");
        query.clauses.push_back(&p);
        p.evaluate(&query, query_res);
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, query_res));
        REQUIRE(query_res->getResults(query.selected) == answer);
    }

    SECTION("Name, Wildcard") {
        QueryResult* query_res = QueryResult::init();
        Query query;
        query.synonyms = {
            {"p",DesignEntity::Procedure},
            {"q",DesignEntity::Procedure}
        };
        query.clauses = vector<Clause*>();
        query.selected = { "p" };
        vector<string> answer = { "Example", "p", "q" };
        CallsStarClause p = CallsStarClause("\"p\"", "_");
        query.clauses.push_back(&p);
        p.evaluate(&query, query_res);
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, query_res));
        REQUIRE(query_res->getResults(query.selected) == answer);
    }

    SECTION("Name, Synonym") {
        QueryResult* query_res = QueryResult::init();
        Query query;
        query.synonyms = {
            {"p",DesignEntity::Procedure},
            {"q",DesignEntity::Procedure}
        };
        query.clauses = vector<Clause*>();
        query.selected = { "p" };
        vector<string> answer = { "Example", "p", "q" };
        CallsStarClause p = CallsStarClause("\"p\"", "q");
        query.clauses.push_back(&p);
        p.evaluate(&query, query_res);
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, query_res));
        REQUIRE(query_res->getResults(query.selected) == answer);
    }

    SECTION("Wildcard, Name") {
        QueryResult* query_res = QueryResult::init();
        Query query;
        query.synonyms = {
            {"p",DesignEntity::Procedure},
            {"q",DesignEntity::Procedure}
        };
        query.clauses = vector<Clause*>();
        query.selected = { "p" };
        vector<string> answer = { "Example", "p", "q" };
        CallsStarClause p = CallsStarClause("_", "\"q\"");
        query.clauses.push_back(&p);
        p.evaluate(&query, query_res);
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, query_res));
        REQUIRE(query_res->getResults(query.selected) == answer);
    }

    SECTION("Wildcard, Synonym") {
        QueryResult* query_res = QueryResult::init();
        Query query;
        query.synonyms = {
            {"p",DesignEntity::Procedure},
            {"q",DesignEntity::Procedure}
        };
        query.clauses = vector<Clause*>();
        query.selected = { "q" };
        vector<string> answer = { "p", "q" };
        CallsStarClause p = CallsStarClause("_", "q");
        query.clauses.push_back(&p);
        p.evaluate(&query, query_res);
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, query_res));
        REQUIRE(query_res->getResults(query.selected) == answer);
    }

    SECTION("Wildcard, Wildcard") {
        QueryResult* query_res = QueryResult::init();
        Query query;
        query.synonyms = {
            {"p",DesignEntity::Procedure},
            {"q",DesignEntity::Procedure}
        };
        query.clauses = vector<Clause*>();
        query.selected = { "p" };
        vector<string> answer = { "Example", "p", "q" };
        CallsStarClause p = CallsStarClause("_", "_");
        query.clauses.push_back(&p);
        p.evaluate(&query, query_res);
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, query_res));
        REQUIRE(query_res->getResults(query.selected) == answer);
    }

    SECTION("Synonym, Name") {
        QueryResult* query_res = QueryResult::init();
        Query query;
        query.synonyms = {
            {"p",DesignEntity::Procedure},
            {"q",DesignEntity::Procedure}
        };
        query.clauses = vector<Clause*>();
        query.selected = { "p" };
        vector<string> answer = { "Example", "p" };
        CallsStarClause p = CallsStarClause("p", "\"q\"");
        query.clauses.push_back(&p);
        p.evaluate(&query, query_res);
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, query_res));
        REQUIRE(query_res->getResults(query.selected) == answer);
    }

    SECTION("Synonym, Wildcard") {
        QueryResult* query_res = QueryResult::init();
        Query query;
        query.synonyms = {
            {"p",DesignEntity::Procedure},
            {"q",DesignEntity::Procedure}
        };
        query.clauses = vector<Clause*>();
        query.selected = { "p" };
        vector<string> answer = { "Example", "p" };
        CallsStarClause p = CallsStarClause("p", "_");
        query.clauses.push_back(&p);
        p.evaluate(&query, query_res);
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, query_res));
        REQUIRE(query_res->getResults(query.selected) == answer);
    }

    SECTION("Synonym, Synonym") {
        QueryResult* query_res = QueryResult::init();
        Query query;
        query.synonyms = {
            {"p",DesignEntity::Procedure},
            {"q",DesignEntity::Procedure}
        };
        query.clauses = vector<Clause*>();
        query.selected = { "p" };
        vector<string> answer = { "Example", "p" };
        CallsStarClause p = CallsStarClause("p", "q");
        query.clauses.push_back(&p);
        p.evaluate(&query, query_res);
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, query_res));
        REQUIRE(query_res->getResults(query.selected) == answer);
    }
}
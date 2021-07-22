#include <vector>
#include "catch.hpp"
#include "front_end/tokenizer/Tokenizer.h"
#include "front_end/parser/Parser.h"
#include "query_processor/Query.h"
#include "utils/SPAEnums.h"
#include "clauses/UsesClause.h"
#include "query_processor/query_evaluator/QueryEvaluator.h"
#include "program_knowledge_base/pkb_storage/PKB.h"

TEST_CASE("Evaluate Uses") {
    string src = "procedure Example {\n"
        "  x = 2;\n"  //1
        "  z = 3;\n" //2
        "  i = 5;\n" //3
        "  read x;\n" //4
        "  while (i!=0) {\n" //5
        "    x = x - 1;\n" //6
        "    if (x==1) then {\n" //7
        "      z = x + 1; \n" //8
        "      print y;}\n" //9
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
        "      i = i - 1; }\n" //16
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
        "   z=x;}"; //24

    Tokenizer* tokenizer = Tokenizer::getInstance();
    vector<Token> tokenStream = tokenizer->tokenizeSource(src);
    Parser* parser = Parser::getInstance();
    AST root = parser->buildAST(tokenStream);
    PKB::getInstance()->reset();
    PKB::getInstance()->initAST(root);

    SECTION("Integer, Name") {
        QueryResult* query_res = QueryResult::init();
        Query query;
        query.synonyms = {
            {"a",DesignEntity::Assign},
            {"v",DesignEntity::Variable},
            {"p",DesignEntity::Procedure}
        };
        query.clauses = vector<Clause*>();
        query.selected = { "p" };
        vector<string> answer = { "Example", "p", "q" };
        UsesClause p = UsesClause("17", "\"x\"");
        query.clauses.push_back(&p); p.evaluate(&query, query_res);
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, query_res));
        REQUIRE(query_res->getResults(query.selected) == answer);
    }

    SECTION("Integer, Wildcard") {
        QueryResult* query_res = QueryResult::init();
        Query query;
        query.synonyms = {
            {"a",DesignEntity::Assign},
            {"v",DesignEntity::Variable},
            {"p",DesignEntity::Procedure}
        };
        query.clauses = vector<Clause*>();
        query.selected = { "p" };
        vector<string> answer = { "Example", "p", "q" };
        UsesClause p = UsesClause("17", "_");
        query.clauses.push_back(&p); p.evaluate(&query, query_res);
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, query_res));
        REQUIRE(query_res->getResults(query.selected) == answer);
    }

    SECTION("Name, Wildcard") {
        QueryResult* query_res = QueryResult::init();
        Query query;
        query.synonyms = {
            {"a",DesignEntity::Assign},
            {"v",DesignEntity::Variable},
            {"p",DesignEntity::Procedure}
        };
        query.clauses = vector<Clause*>();
        query.selected = { "p" };
        vector<string> answer = { "Example", "p", "q" };
        UsesClause p = UsesClause("\"p\"", "_");
        query.clauses.push_back(&p); p.evaluate(&query, query_res);
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, query_res));
        REQUIRE(query_res->getResults(query.selected) == answer);
    }

    SECTION("Integer, Synonym") {
        QueryResult* query_res = QueryResult::init();
        Query query;
        query.synonyms = {
            {"a",DesignEntity::Assign},
            {"v",DesignEntity::Variable},
            {"p",DesignEntity::Procedure}
        };
        query.clauses = vector<Clause*>();
        query.selected = { "p" };
        vector<string> answer = { "Example", "p", "q" };
        UsesClause p = UsesClause("17", "a");
        query.clauses.push_back(&p); p.evaluate(&query, query_res);
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, query_res));
        REQUIRE(query_res->getResults(query.selected) == answer);
    }

    SECTION("Name, Synonym") {
        QueryResult* query_res = QueryResult::init();
        Query query;
        query.synonyms = {
            {"a",DesignEntity::Assign},
            {"v",DesignEntity::Variable},
            {"p",DesignEntity::Procedure}
        };
        query.clauses = vector<Clause*>();
        query.selected = { "p" };
        vector<string> answer = { "Example", "p", "q" };
        UsesClause p = UsesClause("\"p\"", "a");
        query.clauses.push_back(&p); p.evaluate(&query, query_res);
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, query_res));
        REQUIRE(query_res->getResults(query.selected) == answer);
    }

    SECTION("Name, Name") {
        QueryResult* query_res = QueryResult::init();
        Query query;
        query.synonyms = {
            {"a",DesignEntity::Assign},
            {"v",DesignEntity::Variable},
            {"p",DesignEntity::Procedure}
        };
        query.clauses = vector<Clause*>();
        query.selected = { "p" };
        vector<string> answer = { "Example", "p", "q" };
        UsesClause p = UsesClause("\"p\"", "\"x\"");
        query.clauses.push_back(&p); 
        p.evaluate(&query, query_res);
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, query_res));
        REQUIRE(query_res->getResults(query.selected) == answer);
    }

    SECTION("Synonym, Name") {
        QueryResult* query_res = QueryResult::init();
        Query query;
        query.synonyms = {
            {"a",DesignEntity::Assign},
            {"v",DesignEntity::Variable},
            {"p",DesignEntity::Procedure}
        };
        query.clauses = vector<Clause*>();
        query.selected = { "p" };
        vector<string> answer = { "Example", "p", "q" };
        UsesClause p = UsesClause("p", "\"x\"");
        query.clauses.push_back(&p);
        p.evaluate(&query, query_res);
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, query_res));
        REQUIRE(query_res->getResults(query.selected) == answer);
    }

    SECTION("Synonym, Wildcard") {
        QueryResult* query_res = QueryResult::init();
        Query query;
        query.synonyms = {
            {"a",DesignEntity::Assign},
            {"v",DesignEntity::Variable},
            {"p",DesignEntity::Procedure}
        };
        query.clauses = vector<Clause*>();
        query.selected = { "p" };
        vector<string> answer = { "Example", "p", "q" };
        UsesClause p = UsesClause("p", "_");
        query.clauses.push_back(&p);
        p.evaluate(&query, query_res);
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, query_res));
        REQUIRE(query_res->getResults(query.selected) == answer);
    }

    SECTION("Synonym, Synonym") {
        QueryResult* query_res = QueryResult::init();
        Query query;
        query.synonyms = {
            {"a",DesignEntity::Assign},
            {"v",DesignEntity::Variable}
        };
        query.clauses = vector<Clause*>();
        query.selected = { "v" };
        vector<string> answer = { "i", "m", "x", "y", "z" };
        UsesClause p = UsesClause("a", "v");
        query.clauses.push_back(&p);
        p.evaluate(&query, query_res);
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, query_res));
        REQUIRE(query_res->getResults(query.selected) == answer);
    }
}
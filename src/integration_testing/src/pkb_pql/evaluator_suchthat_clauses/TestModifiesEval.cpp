#include <vector>
#include "catch.hpp"
#include "front_end/tokenizer/Tokenizer.h"
#include "front_end/parser/Parser.h"
#include "query_processor/Query.h"
#include "utils/SPAEnums.h"
#include "clauses/ModifiesClause.h"
#include "query_processor/query_evaluator/QueryEvaluator.h"
#include "program_knowledge_base/pkb_storage/PKB.h"

TEST_CASE("Evaluate Modifies") {
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
            {"w",DesignEntity::While}
        };
        query.clauses = vector<Clause*>();
        query.selected = { "w" };
        vector<string> answer = { "14", "5" };
        ModifiesClause p = ModifiesClause("13", "\"d\"");
        query.clauses.push_back(&p);
        p.evaluate(&query, query_res);
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, query_res));
        REQUIRE(query_res->getResults(query.selected) == answer);
    }

    SECTION("Integer, Wildcard") {
        QueryResult* query_res = QueryResult::init();
        Query query;
        query.synonyms = {
            {"a",DesignEntity::Assign},
            {"w",DesignEntity::While}
        };
        query.clauses = vector<Clause*>();
        query.selected = { "w" };
        vector<string> answer = { "14", "5" };
        ModifiesClause p = ModifiesClause("13", "_");
        query.clauses.push_back(&p);
        p.evaluate(&query, query_res);
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, query_res));
        REQUIRE(query_res->getResults(query.selected) == answer);
    }

    SECTION("Name, Wildcard") {
        QueryResult* query_res = QueryResult::init();
        Query query;
        query.synonyms = {
            {"a",DesignEntity::Assign},
            {"w",DesignEntity::While}
        };
        query.clauses = vector<Clause*>();
        query.selected = { "w" };
        vector<string> answer = { "14", "5" };
        ModifiesClause p = ModifiesClause("\"p\"", "_");
        query.clauses.push_back(&p);
        p.evaluate(&query, query_res);
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, query_res));
        REQUIRE(query_res->getResults(query.selected) == answer);
    }

    SECTION("Integer, Synonym") {
        QueryResult* query_res = QueryResult::init();
        Query query;
        query.synonyms = {
            {"a",DesignEntity::Assign},
            {"w",DesignEntity::While},
            {"v",DesignEntity::Variable},
        };
        query.clauses = vector<Clause*>();
        query.selected = { "w" };
        vector<string> answer = { "14", "5" };
        ModifiesClause p = ModifiesClause("13", "v");
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
        vector<string> answer = { "d", "i", "x", "y", "z" };
        ModifiesClause p = ModifiesClause("a", "v");
        query.clauses.push_back(&p);
        p.evaluate(&query, query_res);
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, query_res));
        REQUIRE(query_res->getResults(query.selected) == answer);
    }

    SECTION("Name, Synonym") {
        QueryResult* query_res = QueryResult::init();
        Query query;
        query.synonyms = {
            {"a",DesignEntity::Assign},
            {"w",DesignEntity::While},
            {"v",DesignEntity::Variable}
        };
        query.clauses = vector<Clause*>();
        query.selected = { "w" };
        vector<string> answer = { "14", "5" };
        ModifiesClause p = ModifiesClause("\"q\"", "v");
        query.clauses.push_back(&p);
        p.evaluate(&query, query_res);
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, query_res));
        REQUIRE(query_res->getResults(query.selected) == answer);
    }

    SECTION("Name, Name") {
        QueryResult* query_res = QueryResult::init();
        Query query;
        query.synonyms = {
            {"a",DesignEntity::Assign},
            {"w",DesignEntity::While},
            {"v",DesignEntity::Variable}
        };
        query.clauses = vector<Clause*>();
        query.selected = { "w" };
        vector<string> answer = { "14", "5" };
        ModifiesClause p = ModifiesClause("\"Example\"", "\"x\"");
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
            {"w",DesignEntity::While},
            {"v",DesignEntity::Variable}
        };
        query.clauses = vector<Clause*>();
        query.selected = { "w" };
        vector<string> answer = { "14", "5" };
        ModifiesClause p = ModifiesClause("p", "\"x\"");
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
            {"w",DesignEntity::While},
            {"v",DesignEntity::Variable}
        };
        query.clauses = vector<Clause*>();
        query.selected = { "w" };
        vector<string> answer = { "14", "5" };
        ModifiesClause p = ModifiesClause("p", "_");
        query.clauses.push_back(&p);
        p.evaluate(&query, query_res);
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, query_res));
        REQUIRE(query_res->getResults(query.selected) == answer);
    }
}
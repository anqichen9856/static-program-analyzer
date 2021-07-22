#include <vector>
#include "front_end/tokenizer/Tokenizer.h"
#include "front_end/parser/Parser.h"
#include "catch.hpp"
#include "query_processor/Query.h"
#include "utils/SPAEnums.h"
#include "clauses/PatternAssign.h"
#include "program_knowledge_base/pkb_storage/PKB.h"
#include "query_processor/query_evaluator/QueryEvaluator.h"

TEST_CASE("pattern assign") {

    SECTION("LHS wildcard / var name") {
        string src = "procedure Example {\n"
                     "  x = 3;\n"  //1
                     "  x = x - 3;\n" //2
                     "  y = z + x; \n" //3
                     "  z = x; }\n"; //4

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
                {"a", DesignEntity::Assign}
        };
        query.selected = {"a"};
        query.clauses = vector<Clause *>();

        PatternAssign p = PatternAssign("a", "_", "_\"x-3\"_");
        query.clauses.push_back(&p);

        vector<string> answer = {"2"};
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, query_res));
        REQUIRE(query_res->getResults(query.selected) == answer);
    }

    SECTION("LHS var") {
        QueryResult *query_res = QueryResult::init();
        Query query;
        query.synonyms = {
                {"a", DesignEntity::Assign},
                {"v", DesignEntity::Variable},
        };
        query.selected = {"a", "v"};
        query.clauses = vector<Clause *>();

        PatternAssign p = PatternAssign("a", "v", "\"x\"");
        query.clauses.push_back(&p);

        vector<string> answer = {"4 z"};
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, query_res));
        REQUIRE(query_res->getResults(query.selected) == answer);

    }

    SECTION("RHS var") {
        QueryResult *query_res = QueryResult::init();
        Query query;
        query.synonyms = {
                {"a", DesignEntity::Assign},
                {"v", DesignEntity::Variable},
        };
        query.selected = {"a", "v"};
        query.clauses = vector<Clause *>();

        PatternAssign p = PatternAssign("a", "v", "\"x\"");
        query.clauses.push_back(&p);

        vector<string> answer = {"4 z"};
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, query_res));
        REQUIRE(query_res->getResults(query.selected) == answer);

    }

    SECTION("LHS var name") {
        QueryResult *query_res = QueryResult::init();
        Query query;
        query.synonyms = {
                {"a", DesignEntity::Assign},
                {"v", DesignEntity::Variable},
        };
        query.selected = {"a"};
        query.clauses = vector<Clause *>();

        PatternAssign p = PatternAssign("a", "\"x\"", "_");
        query.clauses.push_back(&p);

        vector<string> answer = {"1", "2"};
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, query_res));
        REQUIRE(query_res->getResults(query.selected) == answer);
    }

    SECTION("RHS 3") {
        QueryResult *query_res = QueryResult::init();
        Query query;
        query.synonyms = {
                {"a", DesignEntity::Assign},
                {"v", DesignEntity::Variable},
        };
        query.selected = {"a"};
        query.clauses = vector<Clause *>();

        PatternAssign p = PatternAssign("a", "v", "\"3\"");
        query.clauses.push_back(&p);

        vector<string> answer = {"1"};
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, query_res));
        REQUIRE(query_res->getResults(query.selected) == answer);
    }

    SECTION("filtering 2 syn"){
        QueryResult query_res = QueryResult(unordered_map<string, int>({{"a", 0}, {"v", 1}}),
                                               list<vector<string>>({{"1", "x"}, {"2", "x"}}));
        Query query;
        query.synonyms = {
                {"a", DesignEntity::Assign},
                {"v", DesignEntity::Variable},
        };
        query.selected = {"a"};
        query.clauses = vector<Clause *>();

        PatternAssign p = PatternAssign("a", "v", "_");
        query.clauses.push_back(&p);

        vector<string> answer = {"1", "2"};
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, &query_res));
        REQUIRE(query_res.getResults(query.selected) == answer);
    }

    SECTION("filtering a with v"){
        QueryResult query_res = QueryResult(unordered_map<string, int>({{"a", 0}, {"v1", 1}}),
                                            list<vector<string>>({{"1", "x"}, {"2", "y"}}));
        Query query;
        query.synonyms = {
                {"a", DesignEntity::Assign},
                {"v", DesignEntity::Variable},
        };
        query.selected = {"a"};
        query.clauses = vector<Clause *>();

        PatternAssign p = PatternAssign("a", "v", "_");
        query.clauses.push_back(&p);

        vector<string> answer = {"1", "2"};
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, &query_res));
        REQUIRE(query_res.getResults(query.selected) == answer);
    }

    SECTION("filtering a"){
        QueryResult query_res = QueryResult(unordered_map<string, int>({{"a", 0}, {"v1", 1}}),
                                            list<vector<string>>({{"1", "x"}, {"2", "x"}}));
        Query query;
        query.synonyms = {
                {"a", DesignEntity::Assign},
        };
        query.selected = {"a"};
        query.clauses = vector<Clause *>();

        PatternAssign p = PatternAssign("a", "_", "x-3");
        query.clauses.push_back(&p);

        vector<string> answer = {"2"};
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, &query_res));
        REQUIRE(query_res.getResults(query.selected) == answer);
    }

    SECTION("filtering v"){
        QueryResult query_res = QueryResult(unordered_map<string, int>({{"a1", 0}, {"v", 1}}),
                                            list<vector<string>>({{"1", "y"}, {"2", "x"}}));
        Query query;
        query.synonyms = {
                {"a", DesignEntity::Assign},
                {"v", DesignEntity::Variable}
        };
        query.selected = {"v"};
        query.clauses = vector<Clause *>();

        PatternAssign p = PatternAssign("a", "v", "x-3");
        query.clauses.push_back(&p);

        vector<string> answer = {"x"};
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, &query_res));
        REQUIRE(query_res.getResults(query.selected) == answer);
    }

    SECTION("partial match") {
        QueryResult *query_res = QueryResult::init();
        Query query;
        query.synonyms = {
                {"a", DesignEntity::Assign},
                {"v", DesignEntity::Variable},
        };
        query.selected = {"a", "v"};
        query.clauses = vector<Clause *>();

        PatternAssign p = PatternAssign("a", "v", "_\"z\"_");
        query.clauses.push_back(&p);

        vector<string> answer = {"3 y"};
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, query_res));
        REQUIRE(query_res->getResults(query.selected) == answer);
    }
}

#include "front_end/tokenizer/Tokenizer.h"
#include "front_end/parser/Parser.h"
#include "clauses/WithClause.h"
#include "clauses/ModifiesClause.h"
#include "clauses/FollowsStarClause.h"
#include "clauses/UsesClause.h"
#include "clauses/NextClause.h"
#include "query_processor/query_evaluator/QueryEvaluator.h"
#include "catch.hpp"
#include "query_processor/query_preprocessor/QueryPreProcessor.h"


using namespace std;

TEST_CASE("Evaluate query") {
    SECTION("no clause") {
        string src = "procedure Example {\n"
                     "  x = 3;\n"  //1
                     "  x = x - 3;\n" //2
                     "  y = z + x; \n" //3
                     "  z = x; \n" //4
                     "  read v; \n" //5
                     "  print s; \n" //6
                     "  read a;" //7
                     "  }\n";

        Tokenizer *tokenizer = Tokenizer::getInstance();
        vector<Token> tokenStream = tokenizer->tokenizeSource(src);
        Parser *parser = Parser::getInstance();
        AST root = parser->buildAST(tokenStream);
        PKB *pkb = PKB::getInstance();
        pkb->reset();
        pkb->initAST(root);

        Query query;
        query.synonyms = {
                {"a", DesignEntity::Assign}
        };
        query.selected = {"a"};
        query.clauses = vector<Clause *>();
        QueryResult* query_res = QueryResult::init();

        vector<string> answer = {"1", "2", "3", "4"};
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, query_res));
        REQUIRE(query_res->getResults(query.selected) == answer);

    }

    SECTION("1 such that clause") {

        // select a such that Follows*(a, 4)
        Query query;
        query.synonyms = {
                {"a", DesignEntity::Assign}
        };
        query.selected = {"a"};
        query.clauses = vector<Clause *>();

        QueryResult query_res = QueryResult(unordered_map<string, int>({{"s", 0}, {"v", 1}}),
                                            {{"1", "x"}, {"1", "y"}, {"2", "z"}});

        FollowsStarClause c = FollowsStarClause("a", "4");
        query.clauses.push_back(&c);

        vector<string> answer = {"1", "2", "3"};
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, &query_res));
        REQUIRE(query_res.getResults(query.selected) == answer);

    }

    SECTION("1 such that clause without synonym") {

        Query query;
        query.synonyms = {
                {"a", DesignEntity::Assign}
        };
        query.selected = {"BOOLEAN"};
        query.clauses = vector<Clause *>();

        QueryResult query_res = QueryResult(unordered_map<string, int>({{"s", 0}, {"v", 1}}),
                                            {});

        FollowsStarClause c = FollowsStarClause("1", "2");
        query.clauses.push_back(&c);

        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, &query_res));

    }

    SECTION("1 pattern clause") {

        Query query;
        query.synonyms = {
                {"a", DesignEntity::Assign}
        };
        query.selected = {"a"};
        query.clauses = vector<Clause *>();

        QueryResult query_res = QueryResult(unordered_map<string, int>(),
                                            {});

        PatternAssign c = PatternAssign("a", "\"x\"", "_");
        query.clauses.push_back(&c);

        vector<string> answer = {"1", "2"};
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, &query_res));
        REQUIRE(query_res.getResults(query.selected) == answer);

    }

    SECTION("select attribute") {

        Query query;
        query.synonyms = {
                {"a", DesignEntity::Assign}
        };
        query.selected = {"a.stmt#"};
        query.clauses = vector<Clause *>();

        QueryResult query_res = QueryResult(unordered_map<string, int>(),{});

        PatternAssign c = PatternAssign("a", "\"x\"", "_");
        query.clauses.push_back(&c);

        vector<string> answer = {"1", "2"};
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, &query_res));
        REQUIRE(query_res.isInTable(query.selected[0]));
        REQUIRE(query_res.getResults(query.selected) == answer);

    }

    SECTION("select BOOLEAN with clause") {

        Query query;
        query.synonyms = {
                {"n", DesignEntity::ProgLine},
        };
        query.selected = {"BOOLEAN"};
        query.clauses = vector<Clause *>();

        QueryResult* query_res = QueryResult::init();

        WithClause c = WithClause("n", "1");
        query.clauses.push_back(&c);
        c.validate(query.synonyms);
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, query_res));

    }

    SECTION("2 clauses") {
        Query query;
        query.synonyms = {
                {"a", DesignEntity::Assign},
                {"v", DesignEntity::Variable}
        };
        vector<Clause *> clauses;
        query.selected = {"a", "v.varName"};

        FollowsStarClause follows_s = FollowsStarClause("2", "a");
        PatternAssign pattern = PatternAssign("a", "v", R"(_"x"_)");
        clauses.push_back(&follows_s);
        clauses.push_back(&pattern);
        query.clauses = clauses;

        QueryResult query_res = QueryResult(unordered_map<string, int>(),{});

        vector<string> answer = {"3 y", "4 z"};
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, &query_res));
        REQUIRE(query_res.getResults(query.selected) == answer);
    }

}

TEST_CASE("multiclause") {
    SECTION("uses modifies next") {
        string source = "procedure main {\n"
                        "    read steps;\n"
                        "    call raymarch;\n"
                        "    print depth; }\n"
                        "\n"
                        "procedure raymarch {\n"
                        "    ro=13;\n"
                        "    rd=19;\n"
                        "    read depth;\n"
                        "    while (count < steps) {\n"
                        "        print depth;\n"
                        "        po = ro + rd * depth;\n"
                        "        call spheresdf;\n"
                        "        if (dist < epsilon) then {\n"
                        "            done = depth; }\n"
                        "        else {\n"
                        "            depth = depth + dist;}\n"
                        "        count = count + 1; }}\n"
                        "\n"
                        "procedure spheresdf {\n"
                        "    dist=x*x+y*y+z*z; x = dist;\n"
                        "    depth = depth;\n"
                        "    read p;\n"
                        "    while (x != p) {\n"
                        "        p = x;\n"
                        "        x = (dist / x + x) / 2; }\n"
                        "    dist = x - 1;\n"
                        "    x = x * x + y * y / 2; }";

        Tokenizer *tokenizer = Tokenizer::getInstance();
        vector<Token> tokenStream = tokenizer->tokenizeSource(source);
        Parser *parser = Parser::getInstance();
        AST root = parser->buildAST(tokenStream);
        PKB *pkb = PKB::getInstance();
        pkb->reset();
        pkb->initAST(root);

        Query query;
        query.synonyms = {
                {"pn", DesignEntity::Print},
                {"s", DesignEntity::Stmt},
                {"v", DesignEntity::Variable}
        };
        query.selected = {"pn"};
        query.clauses = vector<Clause *>();

        ModifiesClause m = ModifiesClause("s", "v");
        UsesClause u = UsesClause("pn", "v");
        NextClause n = NextClause("s", "pn");
        query.clauses.push_back(&m);
        query.clauses.push_back(&u);
        query.clauses.push_back(&n);

        query.no_select={};
        query.with_select={};
        QueryResult *query_res = QueryResult::init();

        vector<string> answer = {"3", "8"};
        QueryOptimizer* qo = new QueryOptimizer(&query);

        qo->optimizeQuery();
        REQUIRE(QueryEvaluator::getInstance()->evaluateWithOptimization(query, query_res));
        REQUIRE(query_res->getResults(query.selected) == answer);
    }
}

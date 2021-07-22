#include <vector>
#include "front_end/tokenizer/Tokenizer.h"
#include "front_end/parser/Parser.h"
#include "catch.hpp"
#include "query_processor/Query.h"
#include "query_processor/query_evaluator/QueryResult.h"
#include "utils/SPAEnums.h"
#include "clauses/AffectsClause.h"
#include "clauses/AffectsStarClause.h"
#include "program_knowledge_base/pkb_storage/PKB.h"
#include "../utils/CompareVectors.h"

TEST_CASE("Evaluate Affects") {
    string src = "procedure x {\n"
                 "  a = b;\n"  //1
                 "  b = a;\n" //2
                 "  c = b;\n" //3
                 "  c = a;\n" //4
                 "  a = c;\n"  //5
                 " }\n"
                 "procedure y {"
                 "x = y;"//6
                 "y = q;"//7
                 "x = x;"//8
                 "y = x;"//9
                 "}"
                 "procedure z {"
                 " t=1+3+5;" //10
                 "}";
    auto stream = Tokenizer::getInstance()->tokenizeSource(src);
    auto ast = Parser::getInstance()->buildAST(stream);
    PKB::getInstance()->reset();
    PKB::getInstance()->initAST(ast);

    Query query;
    query.synonyms = {
            {"a",DesignEntity::Assign},
            {"a2",DesignEntity::Assign}
    };
    vector<Clause*> clauses;
    query.clauses = clauses;
    QueryResult* query_result = QueryResult::init();

    SECTION("Blue graph test") {
        query.selected = {"a"};
        vector<string> answer = {"1", "2", "4", "6", "8"};
        AffectsClause affects_clause("a","a2");
        affects_clause.validate(query.synonyms);
        affects_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);
    }

}

TEST_CASE("Evaluate Affects*") {
    string src = "procedure x {\n"
                 "  a = b;\n"  //1
                 "  b = a;\n" //2
                 "  c = b;\n" //3
                 "  c = a;\n" //4
                 "  a = c;\n"  //5
                 " }\n"
                 "procedure y {"
                 "q = x;"//6
                 "y = q;"//7
                 "x = x;"//8
                 "y = y;"//9
                 "}"
                 "procedure z {"
                 " t=1+3+5;" //10
                 "}";
    auto stream = Tokenizer::getInstance()->tokenizeSource(src);
    auto ast = Parser::getInstance()->buildAST(stream);
    PKB::getInstance()->reset();
    PKB::getInstance()->initAST(ast);

    Query query;
    query.synonyms = {
            {"a",DesignEntity::Assign},
            {"a2",DesignEntity::Assign}
    };
    vector<Clause*> clauses;
    query.clauses = clauses;
    QueryResult* query_result = QueryResult::init();

    SECTION("Blue graph test") {
        query.selected = {"a"};
        vector<string> answer = {"6", "7"};
        AffectsStarClause affects_star_clause("a","9");
        affects_star_clause.validate(query.synonyms);
        affects_star_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);
    }

}

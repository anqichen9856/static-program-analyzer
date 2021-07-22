#include <vector>
#include "catch.hpp"
#include "front_end/tokenizer/Tokenizer.h"
#include "front_end/parser/Parser.h"
#include "query_processor/Query.h"
#include "utils/SPAEnums.h"
#include "clauses/NextClause.h"
#include "query_processor/query_evaluator/QueryEvaluator.h"
#include "program_knowledge_base/pkb_storage/PKB.h"


TEST_CASE("Evaluate Next Clause") {
    auto MakeClause = [](string left, string right) { return NextClause(left, right); };
    auto insertRel = [](int left, int right) { PKB::getInstance()->insertNext(left, right); };
    SECTION("Integer, Integer") { //ok
        PKB::getInstance()->reset();

        Query query;
        query.synonyms = {
                {"s",   DesignEntity::Stmt},
                {"a",   DesignEntity::Assign},
                {"w",   DesignEntity::While},
                {"ifs", DesignEntity::If}
        };
        vector<Clause *> clauses;
        query.selected = {"a"};
        query.clauses = clauses;

        insertRel(1, 2);
        insertRel(2, 3);
        PKB::getInstance()->while_list.push_back(1);
        PKB::getInstance()->assign_list.push_back(2);
        PKB::getInstance()->assign_list.push_back(3);
        PKB::getInstance()->assign_list.push_back(4);

        vector<string> answer1 = {"2", "3", "4"};
        vector<string> answer2 = {};

        auto qr = QueryResult::init();
        auto clause = MakeClause("1", "2");
        query.clauses.push_back(&clause);
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, qr));

        REQUIRE(qr->getResults(query.selected) == answer1);

        PKB::getInstance()->reset();
    }

    SECTION("Interger, Wildcard") { //ok
        PKB::getInstance()->reset();

        Query query;
        query.synonyms = {
                {"s", DesignEntity::Stmt},
                {"a", DesignEntity::Assign},
                {"w", DesignEntity::While}
        };

        query.selected = {"a"};

        insertRel(1, 2);
        insertRel(2, 3);
        PKB::getInstance()->while_list.push_back(1);
        PKB::getInstance()->assign_list.push_back(2);
        PKB::getInstance()->all_statements.push_back(3);
        PKB::getInstance()->assign_list.push_back(4);

        vector<string> answer1 = {"2", "4"};
        vector<string> answer2 = {"2", "4"};

        auto qr = QueryResult::init();
        auto clause = MakeClause("1", "_");
        query.clauses.push_back(&clause);
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, qr));
        REQUIRE(qr->getResults(query.selected) == answer1);

        qr = QueryResult::init();
        query.clauses.clear();
        clause = MakeClause("2", "_");
        query.clauses.push_back(&clause);
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, qr));
        REQUIRE(qr->getResults(query.selected) == answer2);

        PKB::getInstance()->reset();
    }

    SECTION("Integer, Synonym") { //ok
        PKB::getInstance()->reset();

        Query query;
        query.synonyms = {
                {"s",  DesignEntity::Stmt},
                {"a",  DesignEntity::Assign},
                {"w",  DesignEntity::While},
                {"r",  DesignEntity::Read},
                {"pn", DesignEntity::Print},
                {"p",  DesignEntity::Procedure}
        };
        vector<Clause *> clauses;
        query.selected = {"a"};
        query.clauses = clauses;

        insertRel(1, 2);
        insertRel(1, 5);
        PKB::getInstance()->while_list.push_back(1);
        PKB::getInstance()->assign_list.push_back(2);
        PKB::getInstance()->all_statements.push_back(3);
        PKB::getInstance()->assign_list.push_back(4);
        PKB::getInstance()->read_list.push_back(5);
        PKB::getInstance()->stmt_type_list.insert({1, DesignEntity::While});
        PKB::getInstance()->stmt_type_list.insert({2, DesignEntity::Assign});
        PKB::getInstance()->stmt_type_list.insert({3, DesignEntity::Stmt});
        PKB::getInstance()->stmt_type_list.insert({4, DesignEntity::Assign});
        PKB::getInstance()->stmt_type_list.insert({5, DesignEntity::Read});

        vector<string> answer1 = {"2"};
        vector<string> answer2 = {"3"};
        vector<string> answer3 = {};

        auto qr = QueryResult::init();
        auto clause = MakeClause("1", "a");
        query.clauses.push_back(&clause);
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, qr));
        REQUIRE(qr->getResults(query.selected) == answer1);

        qr = QueryResult::init();
        query.selected = {"s"};
        clause = MakeClause("1", "a");
        query.clauses.clear();
        query.clauses.push_back(&clause);
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, qr));
        REQUIRE(qr->getResults(query.selected) == answer2);

        qr = QueryResult::init();
        clause = MakeClause("1", "pn");
        query.clauses.clear();
        query.clauses.push_back(&clause);
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, qr) == false);
        REQUIRE(qr->getResults(query.selected) == answer3);

        PKB::getInstance()->reset();
    }

    SECTION("Wildcard, Integer") { //ok
        PKB::getInstance()->reset();

        Query query;
        query.synonyms = {
                {"s", DesignEntity::Stmt},
                {"a", DesignEntity::Assign},
                {"w", DesignEntity::While}
        };
        vector<Clause *> clauses;
        query.selected = {"a"};
        query.clauses = clauses;

        insertRel(1, 2);
        insertRel(2, 3);
        PKB::getInstance()->while_list.push_back(1);
        PKB::getInstance()->assign_list.push_back(2);
        PKB::getInstance()->all_statements.push_back(3);
        PKB::getInstance()->assign_list.push_back(4);

        vector<string> answer1 = {"2", "4"};
        vector<string> answer2 = {};

        auto qr = QueryResult::init();
        auto clause = MakeClause("_", "2");
        query.clauses.push_back(&clause);
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, qr));
        REQUIRE(qr->getResults(query.selected) == answer1);

        qr = QueryResult::init();
        clause = MakeClause("_", "1");
        query.clauses.clear();
        query.clauses.push_back(&clause);

        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, qr) == false);
        REQUIRE(qr->getResults(query.selected) == answer2);


        PKB::getInstance()->reset();
    }

    SECTION("Wildcard, Synonym") { //ok
        PKB::getInstance()->reset();

        Query query;
        query.synonyms = {
                {"c", DesignEntity::Call},
                {"a", DesignEntity::Assign},
                {"s", DesignEntity::Stmt},
                {"w", DesignEntity::While}
        };
        vector<Clause *> clauses;
        query.selected = {"a"};
        query.clauses = clauses;

        insertRel(1, 2);
        PKB::getInstance()->while_list.push_back(1);
        PKB::getInstance()->assign_list.push_back(2);
        PKB::getInstance()->call_list.push_back(3);
        PKB::getInstance()->assign_list.push_back(4);
        PKB::getInstance()->all_statements.push_back(1);
        PKB::getInstance()->all_statements.push_back(2);
        PKB::getInstance()->all_statements.push_back(3);
        PKB::getInstance()->all_statements.push_back(4);

        vector<string> answer1 = {"2"};
        vector<string> answer2 = {"1", "2", "3", "4"};

        auto qr = QueryResult::init();
        auto clause = MakeClause("_", "a");
        query.clauses.push_back(&clause);
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, qr));
        REQUIRE(qr->getResults(query.selected) == answer1);

        query.selected = {"s"};
        clause = MakeClause("_", "a");
        query.clauses.clear();
        query.clauses.push_back(&clause);
        qr = QueryResult::init();
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, qr));
        REQUIRE(qr->getResults(query.selected) == answer2);

        PKB::getInstance()->reset();
    }

    SECTION("Synonym, Integer") { //ok
        PKB::getInstance()->reset();

        Query query;
        query.synonyms = {
                {"s",   DesignEntity::Stmt},
                {"a",   DesignEntity::Assign},
                {"w",   DesignEntity::While},
                {"ifs", DesignEntity::If}
        };
        vector<Clause *> clauses;
        query.selected = {"w"};
        query.clauses = clauses;

        insertRel(1, 2);
        PKB::getInstance()->while_list.push_back(1);
        PKB::getInstance()->assign_list.push_back(2);
        PKB::getInstance()->all_statements.push_back(3);
        PKB::getInstance()->assign_list.push_back(4);
        PKB::getInstance()->stmt_type_list.insert({1, DesignEntity::While});
        PKB::getInstance()->stmt_type_list.insert({2, DesignEntity::Assign});
        PKB::getInstance()->stmt_type_list.insert({3, DesignEntity::Stmt});
        PKB::getInstance()->stmt_type_list.insert({4, DesignEntity::Assign});
        PKB::getInstance()->all_statements.push_back(5);
        PKB::getInstance()->all_statements.push_back(6);
        PKB::getInstance()->all_statements.push_back(7);

        vector<string> answer1 = {"1"};
        vector<string> answer2 = {"2", "4"};
        vector<string> answer3 = {};
        vector<string> answer4 = {};

        auto qr = QueryResult::init();
        auto clause = MakeClause("w", "2");
        query.clauses.push_back(&clause);
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, qr));
        REQUIRE(qr->getResults(query.selected) == answer1);

        qr = QueryResult::init();
        query.selected = {"a"};
        clause = MakeClause("w", "2");
        query.clauses.clear();
        query.clauses.push_back(&clause);
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, qr));
        REQUIRE(qr->getResults(query.selected) == answer2);

        qr = QueryResult::init();
        clause = MakeClause("1", "1");
        query.clauses.clear();
        query.clauses.push_back(&clause);
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, qr) == false);
        REQUIRE(qr->getResults(query.selected) == answer3);

        qr = QueryResult::init();
        clause = MakeClause("ifs", "2");
        query.clauses.clear();
        query.clauses.push_back(&clause);
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, qr) == false);
        REQUIRE(qr->getResults(query.selected) == answer4);

        PKB::getInstance()->reset();
    }

    SECTION("Wildcard, Wildcard") { //ok
        PKB::getInstance()->reset();

        Query query;
        query.synonyms = {
                {"c", DesignEntity::Call},
                {"a", DesignEntity::Assign},
                {"s", DesignEntity::Stmt},
                {"w", DesignEntity::While}
        };
        vector<Clause *> clauses;
        query.selected = {"a"};
        query.clauses = clauses;


        vector<string> answer1 = {};
        auto qr = QueryResult::init();
        auto clause = MakeClause("_", "_");
        query.clauses.push_back(&clause);
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, qr) == false);
        REQUIRE(qr->getResults(query.selected) == answer1);

        insertRel(1, 2);
        PKB::getInstance()->while_list.push_back(1);
        PKB::getInstance()->assign_list.push_back(2);
        PKB::getInstance()->call_list.push_back(3);
        PKB::getInstance()->assign_list.push_back(4);
        PKB::getInstance()->all_statements.push_back(1);
        PKB::getInstance()->all_statements.push_back(2);
        PKB::getInstance()->all_statements.push_back(3);
        PKB::getInstance()->all_statements.push_back(4);

        vector<string> answer2 = {"2", "4"};
         qr = QueryResult::init();
        clause = MakeClause("_", "_");
        query.clauses.clear();
        query.clauses.push_back(&clause);
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, qr) );
        REQUIRE(qr->getResults(query.selected) == answer2);

        PKB::getInstance()->reset();
    }

    SECTION("Synonym, Wildcard") { //ok
        PKB::getInstance()->reset();

        Query query;
        query.synonyms = {
                {"c", DesignEntity::Call},
                {"a", DesignEntity::Assign},
                {"s", DesignEntity::Stmt},
                {"w", DesignEntity::While}
        };
        vector<Clause *> clauses;
        query.selected = {"w"};
        query.clauses = clauses;


        insertRel(1, 2);
        PKB::getInstance()->while_list.push_back(1);
        PKB::getInstance()->assign_list.push_back(2);
        PKB::getInstance()->call_list.push_back(3);
        PKB::getInstance()->assign_list.push_back(4);
        PKB::getInstance()->all_statements.push_back(1);
        PKB::getInstance()->all_statements.push_back(2);
        PKB::getInstance()->all_statements.push_back(3);
        PKB::getInstance()->all_statements.push_back(4);

        vector<string> answer1 = {"1"};
        vector<string> answer2 = {"2", "4"};

        auto qr = QueryResult::init();
        auto clause = MakeClause("w", "_");
        query.clauses.push_back(&clause);
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, qr) );
        REQUIRE(qr->getResults(query.selected) == answer1);

        qr = QueryResult::init();
        query.selected = {"a"};
        clause = MakeClause("w", "_");
        query.clauses.clear();
        query.clauses.push_back(&clause);
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, qr) );
        REQUIRE(qr->getResults(query.selected) == answer2);


        PKB::getInstance()->reset();
    }

    SECTION("Synonym, Synonym") { //ok
        PKB::getInstance()->reset();

        Query query;
        query.synonyms = {
                {"c", DesignEntity::Call},
                {"a", DesignEntity::Assign},
                {"s", DesignEntity::Stmt},
                {"w", DesignEntity::While}
        };

        query.selected = {"w"};

        insertRel(1, 2);
        PKB::getInstance()->while_list.push_back(1);
        PKB::getInstance()->assign_list.push_back(2);
        PKB::getInstance()->call_list.push_back(3);
        PKB::getInstance()->assign_list.push_back(4);
        PKB::getInstance()->all_statements.push_back(1);
        PKB::getInstance()->all_statements.push_back(2);
        PKB::getInstance()->all_statements.push_back(3);
        PKB::getInstance()->all_statements.push_back(4);

        vector<string> answer1 = {"1"};
        vector<string> answer2 = {"2"};
        vector<string> answer3 = {"1", "2", "3", "4"};

        auto qr = QueryResult::init();
        auto clause = MakeClause("w", "a");
        query.clauses.push_back(&clause);
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, qr) );
        REQUIRE(qr->getResults(query.selected) == answer1);

        qr = QueryResult::init();
        query.selected = {"a"};
        clause = MakeClause("w", "a");
        query.clauses.clear();
        query.clauses.push_back(&clause);
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, qr) );
        REQUIRE(qr->getResults(query.selected) == answer2);

        qr = QueryResult::init();
        query.selected = {"s"};
        MakeClause("w", "a");
        query.clauses.clear();
        query.clauses.push_back(&clause);
        REQUIRE(QueryEvaluator::getInstance()->evaluateQuery(query, qr) );
        REQUIRE(qr->getResults(query.selected) == answer3);

        PKB::getInstance()->reset();
    }
}

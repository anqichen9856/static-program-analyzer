#include "catch.hpp"
#include "spa_controller/SPAController.h"
#include "exceptions/SPAException.h"

TEST_CASE("Run SPA controller") {
    SECTION("Invalid Filename") {
        string invalid_filename = "WrongFile.txt";

        string message_exception = "Invalid File Exception! " + invalid_filename + " cannot be found.";
        REQUIRE_THROWS_WITH(SPAController::runSPAController(invalid_filename), message_exception);
    }
}

TEST_CASE("Evaluate query - SPA") {
    SECTION("stmt") {
        string query = "stmt s; Select s";
        list<string> result;

        PKB::getInstance()->reset();

        PKB::getInstance()->all_statements.push_back(1);
        PKB::getInstance()->all_statements.push_back(2);
        PKB::getInstance()->all_statements.push_back(3);
        PKB::getInstance()->all_statements.push_back(4);
        PKB::getInstance()->stmt_type_list.insert({1, DesignEntity::Stmt});
        PKB::getInstance()->stmt_type_list.insert({2, DesignEntity::Stmt});
        PKB::getInstance()->stmt_type_list.insert({3, DesignEntity::Stmt});
        PKB::getInstance()->stmt_type_list.insert({4, DesignEntity::Stmt});

        list<string> answer = {"1", "2", "3", "4"};
        SPAController::evaluateQuery(query, result);
        REQUIRE(result == answer);
        PKB::getInstance()->reset();
    }
}

TEST_CASE("Invalid pattern while - SPA") {
        string query = "while w; stmt s; prog_line n; variable v;Select BOOLEAN pattern w(v,\"v1\")";
        list<string> result;
        SPAController::evaluateQuery(query, result);
        REQUIRE(result == list<string>());
}

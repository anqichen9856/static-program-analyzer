#include <vector>
#include "catch.hpp"
#include "query_processor/Query.h"
#include "utils/SPAEnums.h"
#include "clauses/FollowsStarClause.h"
#include "program_knowledge_base/pkb_storage/PKB.h"

TEST_CASE("FollowsStar - Validate") {
    SECTION("stmt, stmt") {
        FollowsStarClause clause = FollowsStarClause("s", "r");
        unordered_map<string,DesignEntity> map = {
                {"s", DesignEntity::Stmt},
                {"r", DesignEntity::Read}
        };
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("int, wildcard") {
        FollowsStarClause clause = FollowsStarClause("2", "_");
        unordered_map<string,DesignEntity> map = {
                {"s", DesignEntity::Stmt},
                {"s2", DesignEntity::Stmt}
        };
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("wildcard, wildcard") {
        FollowsStarClause clause = FollowsStarClause("_", "_");
        unordered_map<string,DesignEntity> map = {
                {"s", DesignEntity::Stmt},
                {"s2", DesignEntity::Stmt}
        };
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("int, int") {
        FollowsStarClause clause = FollowsStarClause("2", "7");
        unordered_map<string,DesignEntity> map = {};
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("stmt, int") {
        FollowsStarClause clause = FollowsStarClause("p", "7");
        unordered_map<string,DesignEntity> map = {
                {"p", DesignEntity::Print}
        };
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("stmt, variable") {
        FollowsStarClause clause = FollowsStarClause("x", "v");
        unordered_map<string,DesignEntity> map = {
                {"x", DesignEntity::Stmt},
                {"v", DesignEntity::Variable}
        };
        REQUIRE_THROWS(clause.validate(map));
    }

    SECTION("procedure, stmt") {
        FollowsStarClause clause = FollowsStarClause("p", "a");
        unordered_map<string,DesignEntity> map = {
                {"p", DesignEntity::Procedure},
                {"a", DesignEntity::Assign}
        };
        REQUIRE_THROWS(clause.validate(map));
    }
}


#include <vector>
#include "catch.hpp"
#include "query_processor/Query.h"
#include "utils/SPAEnums.h"
#include "clauses/FollowsClause.h"
#include "program_knowledge_base/pkb_storage/PKB.h"

TEST_CASE("Follows - Validate") {
    SECTION("stmt, stmt") {
        FollowsClause clause = FollowsClause("s", "r");
        unordered_map<string,DesignEntity> map = {
                {"s", DesignEntity::Stmt},
                {"r", DesignEntity::Read}
        };
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("int, wildcard") {
        FollowsClause clause = FollowsClause("2", "_");
        unordered_map<string,DesignEntity> map = {
                {"s", DesignEntity::Stmt},
                {"s2", DesignEntity::Stmt}
        };
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("wildcard, wildcard") {
        FollowsClause clause = FollowsClause("_", "_");
        unordered_map<string,DesignEntity> map = {
                {"s", DesignEntity::Stmt},
                {"s2", DesignEntity::Stmt}
        };
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("int, int") {
        FollowsClause clause = FollowsClause("2", "7");
        unordered_map<string,DesignEntity> map = {};
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("stmt, int") {
        FollowsClause clause = FollowsClause("p", "7");
        unordered_map<string,DesignEntity> map = {
                {"p", DesignEntity::Print}
        };
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("stmt, variable") {
        FollowsClause clause = FollowsClause("x", "v");
        unordered_map<string,DesignEntity> map = {
                {"x", DesignEntity::Stmt},
                {"v", DesignEntity::Variable}
        };
        REQUIRE_THROWS(clause.validate(map));
    }

    SECTION("procedure, stmt") {
        FollowsClause clause = FollowsClause("p", "a");
        unordered_map<string,DesignEntity> map = {
                {"p", DesignEntity::Procedure},
                {"a", DesignEntity::Assign}
        };
        REQUIRE_THROWS(clause.validate(map));
    }
}


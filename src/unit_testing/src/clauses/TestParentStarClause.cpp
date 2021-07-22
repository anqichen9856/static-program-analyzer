#include <vector>
#include "catch.hpp"
#include "query_processor/Query.h"
#include "utils/SPAEnums.h"
#include "clauses/ParentStarClause.h"
#include "program_knowledge_base/pkb_storage/PKB.h"

TEST_CASE("ParentStar - Validate") {
    SECTION("stmt, wildcard") {
        ParentStarClause clause = ParentStarClause("s", "_");
        unordered_map<string,DesignEntity> map = {
                {"s", DesignEntity::Stmt},
                {"r", DesignEntity::Read}
        };
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("if, int") {
        ParentStarClause clause = ParentStarClause("ifs", "9");
        unordered_map<string,DesignEntity> map = {
                {"ifs", DesignEntity::If},
                {"v", DesignEntity::Variable}
        };
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("while, stmt") {
        ParentStarClause clause = ParentStarClause("w", "s");
        unordered_map<string,DesignEntity> map = {
                {"s", DesignEntity::Print},
                {"w", DesignEntity::While}
        };
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("if, while") {
        ParentStarClause clause = ParentStarClause("ifs", "w");
        unordered_map<string,DesignEntity> map = {
                {"ifs", DesignEntity::If},
                {"w", DesignEntity::While}
        };
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("assign, stmt") {
        ParentStarClause clause = ParentStarClause("a", "x");
        unordered_map<string,DesignEntity> map = {
                {"x", DesignEntity::Read},
                {"a", DesignEntity::Assign}
        };
        REQUIRE_THROWS(clause.validate(map));
    }
}


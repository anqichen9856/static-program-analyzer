#include <vector>
#include "catch.hpp"
#include "utils/SPAEnums.h"
#include "clauses/PatternClause.h"
#include "clauses/PatternAssign.h"
#include "program_knowledge_base/pkb_storage/PKB.h"

TEST_CASE("Pattern - Validate") {
    SECTION("assign, var, wildcard") {
        unordered_map<string, DesignEntity> map = {
                {"a", DesignEntity::Assign},
                {"v", DesignEntity::Variable}
        };
        PatternClause clause = PatternClause("a", "v", "_");
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("assign, var name, partial wildcard") {
        unordered_map<string, DesignEntity> map = {
                {"a", DesignEntity::Assign},
                {"v", DesignEntity::Variable}
        };
        PatternClause clause = PatternClause("a", "\"var\"", "_\"x\"_");
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("left_syn not variable") {
        unordered_map<string, DesignEntity> map = {
                {"a", DesignEntity::Assign},
                {"v", DesignEntity::Constant}
        };
        PatternAssign clause = PatternAssign("a", "v", "_\"x\"_");
        REQUIRE_THROWS(clause.validate(map));
    }

    SECTION("param syn not declared") {
        unordered_map<string, DesignEntity> map = {
                {"a", DesignEntity::Assign},
                {"v", DesignEntity::Variable}
        };
        PatternAssign clause = PatternAssign("a", "var", "_\"x\"_");
        REQUIRE_THROWS(clause.validate(map));
    }
}

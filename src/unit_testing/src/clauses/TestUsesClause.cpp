#include <vector>
#include "catch.hpp"
#include "query_processor/Query.h"
#include "utils/SPAEnums.h"
#include "clauses/UsesClause.h"
#include "program_knowledge_base/pkb_storage/PKB.h"

TEST_CASE("Uses - Validate") {
    SECTION("procedure name, variable name") {
        string left = "\"s\"";
        string right = "\"v\"";
        unordered_map<string, DesignEntity> map = {
        };
        UsesClause clause = UsesClause(left, right);
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("stmt syn, variable syn") {
        string left = "s";
        string right = "v";
        unordered_map<string, DesignEntity> map = {
                {"s", DesignEntity::Assign},
                {"v", DesignEntity::Variable}
        };
        UsesClause clause = UsesClause(left, right);
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("stmt syn, syn not variable") {
        string left = "s";
        string right = "v";
        unordered_map<string, DesignEntity> map = {
                {"s", DesignEntity::Print},
                {"v", DesignEntity::Assign}
        };
        UsesClause clause = UsesClause(left, right);
        REQUIRE_THROWS(clause.validate(map));
    }
}

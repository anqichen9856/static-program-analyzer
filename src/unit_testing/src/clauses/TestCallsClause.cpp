#include <vector>
#include "catch.hpp"
#include "query_processor/Query.h"
#include "utils/SPAEnums.h"
#include "clauses/CallsClause.h"
#include "program_knowledge_base/pkb_storage/PKB.h"

TEST_CASE("Calls - Validate") {
    SECTION("procedure, procedure") {
        CallsClause clause = CallsClause("p1", "p2");
        unordered_map<string,DesignEntity> map = {
                {"p1", DesignEntity::Procedure},
                {"p2", DesignEntity::Procedure}
        };
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("procedure_name, wildcard") {
        CallsClause clause = CallsClause("\"DIO\"", "_");
        unordered_map<string,DesignEntity> map = {
                {"p1", DesignEntity::Procedure},
                {"p2", DesignEntity::Procedure}
        };
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("wildcard, wildcard") {
        CallsClause clause = CallsClause("_", "_");
        unordered_map<string,DesignEntity> map = {
                {"p1", DesignEntity::Procedure},
                {"p2", DesignEntity::Procedure}
        };
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("procedure name, procedure name") {
        CallsClause clause = CallsClause("\"jojo\"", "\"dio\"");
        unordered_map<string,DesignEntity> map = {};
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("proc, procedure name") {
        CallsClause clause = CallsClause("p", "\"main\"");
        unordered_map<string,DesignEntity> map = {
                {"p", DesignEntity::Procedure}
        };
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("proc, invalid") {
        CallsClause clause = CallsClause("p", "5");
        unordered_map<string,DesignEntity> map = {
                {"p", DesignEntity::Procedure}
        };
        REQUIRE_THROWS(clause.validate(map));
    }

    SECTION("invalid, invalid") {
        CallsClause clause = CallsClause("s", "\"main\"");
        unordered_map<string,DesignEntity> map = {
                {"s", DesignEntity::Stmt}
        };
        REQUIRE_THROWS(clause.validate(map));
    }
}


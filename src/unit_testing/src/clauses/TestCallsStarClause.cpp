#include <vector>
#include "catch.hpp"
#include "query_processor/Query.h"
#include "utils/SPAEnums.h"
#include "clauses/CallsStarClause.h"
#include "program_knowledge_base/pkb_storage/PKB.h"

TEST_CASE("CallsStar - Validate") {
    SECTION("procedure, procedure") {
        CallsStarClause clause = CallsStarClause("p1", "p2");
        unordered_map<string,DesignEntity> map = {
                {"p1", DesignEntity::Procedure},
                {"p2", DesignEntity::Procedure}
        };
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("procedure_name, wildcard") {
        CallsStarClause clause = CallsStarClause("\"DIO\"", "_");
        unordered_map<string,DesignEntity> map = {
                {"p1", DesignEntity::Procedure},
                {"p2", DesignEntity::Procedure}
        };
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("wildcard, wildcard") {
        CallsStarClause clause = CallsStarClause("_", "_");
        unordered_map<string,DesignEntity> map = {
                {"p1", DesignEntity::Procedure},
                {"p2", DesignEntity::Procedure}
        };
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("procedure name, procedure name") {
        CallsStarClause clause = CallsStarClause("\"jojo\"", "\"dio\"");
        unordered_map<string,DesignEntity> map = {};
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("proc, procedure name") {
        CallsStarClause clause = CallsStarClause("p", "\"main\"");
        unordered_map<string,DesignEntity> map = {
                {"p", DesignEntity::Procedure}
        };
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("proc, invalid") {
        CallsStarClause clause = CallsStarClause("p", "5");
        unordered_map<string,DesignEntity> map = {
                {"p", DesignEntity::Procedure}
        };
        REQUIRE_THROWS(clause.validate(map));
    }

    SECTION("invalid, invalid") {
        CallsStarClause clause = CallsStarClause("s", "\"main\"");
        unordered_map<string,DesignEntity> map = {
                {"s", DesignEntity::Stmt}
        };
        REQUIRE_THROWS(clause.validate(map));
    }
}


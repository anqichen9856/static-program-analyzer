#include <vector>
#include "catch.hpp"
#include "query_processor/Query.h"
#include "utils/SPAEnums.h"
#include "clauses/ModifiesClause.h"
#include "program_knowledge_base/pkb_storage/PKB.h"

TEST_CASE("Modifies - Validate") {
    SECTION("procedure name, wildcard") {
        ModifiesClause clause = ModifiesClause("\"jojo\"", "_");
        unordered_map<string,DesignEntity> map = {
                {"s", DesignEntity::Stmt},
                {"r", DesignEntity::Read}
        };
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("stmt, variable") {
        ModifiesClause clause = ModifiesClause("c", "v");
        unordered_map<string,DesignEntity> map = {
                {"c", DesignEntity::Call},
                {"v", DesignEntity::Variable}
        };
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("int, var name") {
        ModifiesClause clause = ModifiesClause("7", "\"y\"");
        unordered_map<string,DesignEntity> map = {
                {"s", DesignEntity::Stmt},
                {"s2", DesignEntity::Stmt}
        };
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("print, int") {
        ModifiesClause clause = ModifiesClause("p", "7");
        unordered_map<string,DesignEntity> map = {
                {"p", DesignEntity::Print}
        };
        REQUIRE_THROWS(clause.validate(map));
    }

    SECTION("wildcard, stmt") {
        ModifiesClause clause = ModifiesClause("_", "x");
        unordered_map<string,DesignEntity> map = {
                {"x", DesignEntity::Read}
        };
        REQUIRE_THROWS(clause.validate(map));
    }
}

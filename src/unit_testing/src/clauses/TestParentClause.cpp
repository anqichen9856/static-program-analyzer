#include <vector>
#include "catch.hpp"
#include "front_end/tokenizer/Tokenizer.h"
#include "front_end/parser/Parser.h"
#include "query_processor/Query.h"
#include "utils/SPAEnums.h"
#include "clauses/ParentClause.h"
#include "program_knowledge_base/pkb_storage/PKB.h"

TEST_CASE("Parent - Validate") {
    SECTION("stmt, wildcard") {
        ParentClause clause = ParentClause("s", "_");
        unordered_map<string,DesignEntity> map = {
                {"s", DesignEntity::Stmt},
                {"r", DesignEntity::Read}
        };
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("if, int") {
        ParentClause clause = ParentClause("ifs", "9");
        unordered_map<string,DesignEntity> map = {
                {"ifs", DesignEntity::If},
                {"v", DesignEntity::Variable}
        };
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("while, stmt") {
        ParentClause clause = ParentClause("w", "s");
        unordered_map<string,DesignEntity> map = {
                {"s", DesignEntity::Print},
                {"w", DesignEntity::While}
        };
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("if, while") {
        ParentClause clause = ParentClause("ifs", "w");
        unordered_map<string,DesignEntity> map = {
                {"ifs", DesignEntity::If},
                {"w", DesignEntity::While}
        };
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("assign, stmt") {
        ParentClause clause = ParentClause("a", "x");
        unordered_map<string,DesignEntity> map = {
                {"x", DesignEntity::Read},
                {"a", DesignEntity::Assign}
        };
        REQUIRE_THROWS(clause.validate(map));
    }
}


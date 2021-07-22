#include <vector>
#include "catch.hpp"
#include "query_processor/Query.h"
#include "utils/SPAEnums.h"
#include "clauses/WithClause.h"
#include "program_knowledge_base/pkb_storage/PKB.h"

TEST_CASE("With - Validate") {
    SECTION("stmt#, stmt#") {
        WithClause clause = WithClause("a1.stmt#", "a2.stmt#");
        unordered_map<string,DesignEntity> map = {
                {"a1", DesignEntity::Assign},
                {"a2", DesignEntity::Assign}
        };

        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("stmt#, value") {
        WithClause clause = WithClause("a1.stmt#", "c.value");
        unordered_map<string,DesignEntity> map = {
                {"a1", DesignEntity::Assign},
                {"c", DesignEntity::Constant}
        };
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("Invalid - stmt#, procName") {
        WithClause clause = WithClause("a1.stmt#", "p.procName");
        unordered_map<string,DesignEntity> map = {
                {"a1", DesignEntity::Assign},
                {"p", DesignEntity::Procedure}
        };
        REQUIRE_THROWS(clause.validate(map));
    }

    SECTION("Invalid - stmt#, varName") {
        WithClause clause = WithClause("a1.stmt#", "v.varName");
        unordered_map<string,DesignEntity> map = {
                {"a1", DesignEntity::Assign},
                {"v", DesignEntity::Variable}
        };
        REQUIRE_THROWS(clause.validate(map));
    }

    SECTION("varName, varName") {
        WithClause clause = WithClause("v.varName", "r.varName");
        unordered_map<string,DesignEntity> map = {
                {"v", DesignEntity::Variable},
                {"r", DesignEntity::Read}
        };

        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("varName, procName") {
        WithClause clause = WithClause("pn.varName", "p.procName");
        unordered_map<string,DesignEntity> map = {
                {"pn", DesignEntity::Print},
                {"p", DesignEntity::Procedure}
        };
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("Invalid - varName, stmtNo") {
        WithClause clause = WithClause("v.varName", "a1.stmt#");
        unordered_map<string,DesignEntity> map = {
                {"v", DesignEntity::Variable},
                {"a1", DesignEntity::Assign}
        };
        REQUIRE_THROWS(clause.validate(map));
    }

    SECTION("Invalid - varName, value") {
        WithClause clause = WithClause("v.varName", "c.value");
        unordered_map<string,DesignEntity> map = {
                {"v", DesignEntity::Variable},
                {"c", DesignEntity::Constant}
        };
        REQUIRE_THROWS(clause.validate(map));
    }

    SECTION("procName, procName") {
        WithClause clause = WithClause("p1.procName", "p2.procName");
        unordered_map<string,DesignEntity> map = {
                {"p1", DesignEntity::Procedure},
                {"p2", DesignEntity::Procedure}
        };

        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("procName, varName") {
        WithClause clause = WithClause("p1.procName", "v.varName");
        unordered_map<string,DesignEntity> map = {
                {"p1", DesignEntity::Procedure},
                {"v", DesignEntity::Variable}
        };

        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("Invalid - procName, stmtNo") {
        WithClause clause = WithClause("p1.procName", "a1.stmt#");
        unordered_map<string,DesignEntity> map = {
                {"p1", DesignEntity::Procedure},
                {"a1", DesignEntity::Assign}
        };
        REQUIRE_THROWS(clause.validate(map));
    }

    SECTION("Invalid - procName, value") {
        WithClause clause = WithClause("p1.procName", "c.value");
        unordered_map<string,DesignEntity> map = {
                {"p1", DesignEntity::Procedure},
                {"c", DesignEntity::Constant}
        };
        REQUIRE_THROWS(clause.validate(map));
    }

    SECTION("value, value") {
        WithClause clause = WithClause("c1.value", "c2.value");
        unordered_map<string,DesignEntity> map = {
                {"c1", DesignEntity::Constant},
                {"c2", DesignEntity::Constant}
        };

        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("Invalid - value, varName") {
        WithClause clause = WithClause("c1.value", "v.varName");
        unordered_map<string,DesignEntity> map = {
                {"c1", DesignEntity::Constant},
                {"v", DesignEntity::Variable}
        };

        REQUIRE_THROWS(clause.validate(map));
    }

    SECTION("value, stmtNo") {
        WithClause clause = WithClause("c1.value", "a1.stmt#");
        unordered_map<string,DesignEntity> map = {
                {"c1", DesignEntity::Constant},
                {"a1", DesignEntity::Assign}
        };
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("Invalid - value, procName") {
        WithClause clause = WithClause("c1.value", "p1.procName");
        unordered_map<string,DesignEntity> map = {
                {"p1", DesignEntity::Procedure},
                {"c", DesignEntity::Constant}
        };
        REQUIRE_THROWS(clause.validate(map));
    }

    SECTION("syn, int") {
        WithClause clause = WithClause("n", "2");
        unordered_map<string,DesignEntity> map = {
                {"n", DesignEntity::ProgLine},
        };
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("Invalid - syn,name") {
        WithClause clause = WithClause("n", "\"test\"");
        unordered_map<string,DesignEntity> map = {
                {"n", DesignEntity::ProgLine},
        };
        REQUIRE_THROWS(clause.validate(map));
    }

    SECTION("syn,syn") {
        WithClause clause = WithClause("n", "n2");
        unordered_map<string,DesignEntity> map = {
                {"n", DesignEntity::ProgLine},
                {"n2", DesignEntity::ProgLine},
        };
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("Invalid - syn,varName") {
        WithClause clause = WithClause("n", "v.varName");
        unordered_map<string,DesignEntity> map = {
                {"n", DesignEntity::ProgLine},
                {"v", DesignEntity::Variable},
        };
        REQUIRE_THROWS(clause.validate(map));
    }

    SECTION("Invalid - syn,procName") {
        WithClause clause = WithClause("n", "p.procName");
        unordered_map<string,DesignEntity> map = {
                {"n", DesignEntity::ProgLine},
                {"p", DesignEntity::Procedure},
        };
        REQUIRE_THROWS(clause.validate(map));
    }

    SECTION("syn,value") {
        WithClause clause = WithClause("n", "c.value");
        unordered_map<string,DesignEntity> map = {
                {"n", DesignEntity::ProgLine},
                {"c", DesignEntity::Constant},
        };
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("syn,stmt#") {
        WithClause clause = WithClause("n", "a.stmt#");
        unordered_map<string,DesignEntity> map = {
                {"n", DesignEntity::ProgLine},
                {"a", DesignEntity::Assign},
        };
        set<string> ans = set<string>{"n","a"};
        REQUIRE(clause.getSynonyms() == ans);
        REQUIRE(clause.getNumOfSyn() == 2);
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("Invalid - varName,syn") {
        WithClause clause = WithClause("v.varName","n");
        unordered_map<string,DesignEntity> map = {
                {"n", DesignEntity::ProgLine},
                {"v", DesignEntity::Variable},
        };
        REQUIRE_THROWS(clause.validate(map));
    }

    SECTION("Invalid - procName,syn") {
        WithClause clause = WithClause("p.procName","n");
        unordered_map<string,DesignEntity> map = {
                {"n", DesignEntity::ProgLine},
                {"p", DesignEntity::Procedure},
        };
        REQUIRE_THROWS(clause.validate(map));
    }

    SECTION("value syn") {
        WithClause clause = WithClause("c.value", "n");
        unordered_map<string,DesignEntity> map = {
                {"n", DesignEntity::ProgLine},
                {"c", DesignEntity::Constant},
        };
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("stmt#, syn") {
        WithClause clause = WithClause("a.stmt#", "n");
        unordered_map<string,DesignEntity> map = {
                {"n", DesignEntity::ProgLine},
                {"a", DesignEntity::Assign},
        };
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("Invalid - int,name") {
        WithClause clause = WithClause("1", "\"test\"");
        unordered_map<string,DesignEntity> map = {

        };
        REQUIRE_THROWS(clause.validate(map));
    }

    SECTION("int, syn") {
        WithClause clause = WithClause("1", "n");
        unordered_map<string,DesignEntity> map = {
                {"n", DesignEntity::ProgLine},
        };
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("int,int") {
        WithClause clause = WithClause("1", "2");
        unordered_map<string,DesignEntity> map = {

        };
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("Invalid - int,name") {
        WithClause clause = WithClause("1", "\"test\"");
        unordered_map<string,DesignEntity> map = {

        };
        REQUIRE_THROWS(clause.validate(map));
    }

    SECTION("int, syn") {
        WithClause clause = WithClause("1", "n");
        unordered_map<string,DesignEntity> map = {
                {"n", DesignEntity::ProgLine},
        };
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("Invalid - int,varName") {
        WithClause clause = WithClause("2", "v.varName");
        unordered_map<string,DesignEntity> map = {
                {"v", DesignEntity::Variable},
        };
        REQUIRE_THROWS(clause.validate(map));
    }

    SECTION("Invalid - int,procName") {
        WithClause clause = WithClause("2", "p.procName");
        unordered_map<string,DesignEntity> map = {
                {"p", DesignEntity::Procedure},
        };
        REQUIRE_THROWS(clause.validate(map));
    }

    SECTION("int,value") {
        WithClause clause = WithClause("2", "c.value");
        unordered_map<string,DesignEntity> map = {
                {"c", DesignEntity::Constant},
        };
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("int,stmt#") {
        WithClause clause = WithClause("2", "a.stmt#");
        unordered_map<string,DesignEntity> map = {
                {"a", DesignEntity::Assign},
        };
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("Invalid - varName,int") {
        WithClause clause = WithClause("v.varName","2");
        unordered_map<string,DesignEntity> map = {
                {"v", DesignEntity::Variable},
        };
        REQUIRE_THROWS(clause.validate(map));
    }

    SECTION("Invalid - procName,int") {
        WithClause clause = WithClause("p.procName","2");
        unordered_map<string,DesignEntity> map = {
                {"p", DesignEntity::Procedure},
        };
        REQUIRE_THROWS(clause.validate(map));
    }

    SECTION("value int") {
        WithClause clause = WithClause("c.value", "2");
        unordered_map<string,DesignEntity> map = {
                {"c", DesignEntity::Constant},
        };
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("stmt#, int") {
        WithClause clause = WithClause("a.stmt#", "2");
        unordered_map<string,DesignEntity> map = {
                {"a", DesignEntity::Assign},
        };
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("name, name") {
        WithClause clause = WithClause("\"test\"", "\"test\"");
        unordered_map<string,DesignEntity> map = {

        };
        set<string> ans = set<string>{};
        REQUIRE(clause.getSynonyms() == ans);
        REQUIRE(clause.getNumOfSyn() == 0);
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("Invalid - name , int") {
        WithClause clause = WithClause("\"test\"", "1");
        unordered_map<string,DesignEntity> map = {

        };
        REQUIRE_THROWS(clause.validate(map));
    }

    SECTION("Invalid - name , syn") {
        WithClause clause = WithClause("\"test\"", "n");
        unordered_map<string,DesignEntity> map = {
                {"n", DesignEntity::ProgLine},
        };
        REQUIRE_THROWS(clause.validate(map));
    }

    SECTION("name,varName") {
        WithClause clause = WithClause("\"test\"", "v.varName");
        unordered_map<string,DesignEntity> map = {
                {"v", DesignEntity::Variable},
        };
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("name,procName") {
        WithClause clause = WithClause("\"test\"", "p.procName");
        unordered_map<string,DesignEntity> map = {
                {"p", DesignEntity::Procedure},
        };
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("Invalid - name,value") {
        WithClause clause = WithClause("\"test\"", "c.value");
        unordered_map<string,DesignEntity> map = {
                {"c", DesignEntity::Constant},
        };
        REQUIRE_THROWS(clause.validate(map));
    }

    SECTION("Invalid - name,stmt#") {
        WithClause clause = WithClause("\"test\"", "a.stmt#");
        unordered_map<string,DesignEntity> map = {
                {"a", DesignEntity::Assign},
        };
        set<string> ans = set<string>{"a"};
        REQUIRE(clause.getSynonyms() == ans);
        REQUIRE(clause.getNumOfSyn() == 1);
        REQUIRE_THROWS(clause.validate(map));
    }

    SECTION("varName,name") {
        WithClause clause = WithClause("v.varName","\"test\"");
        unordered_map<string,DesignEntity> map = {
                {"v", DesignEntity::Variable},
        };

        set<string> ans = set<string>{"v"};
        REQUIRE(clause.getSynonyms() == ans);
        REQUIRE(clause.getNumOfSyn() == 1);
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("procName,name") {
        WithClause clause = WithClause("p.procName","\"test\"");
        unordered_map<string,DesignEntity> map = {
                {"p", DesignEntity::Procedure},
        };
        REQUIRE_NOTHROW(clause.validate(map));
    }

    SECTION("Invalid - value name") {
        WithClause clause = WithClause("c.value", "\"test\"");
        unordered_map<string,DesignEntity> map = {
                {"c", DesignEntity::Constant},
        };
        REQUIRE_THROWS(clause.validate(map));
    }

    SECTION("Invalid - stmt#, name") {
        WithClause clause = WithClause("a.stmt#", "\"test\"");
        unordered_map<string,DesignEntity> map = {
                {"a", DesignEntity::Assign},
        };
        REQUIRE_THROWS(clause.validate(map));
    }

    SECTION("Invalid - mismatched attribute and DE") {
        WithClause clause = WithClause("a.procName", "c.varName");
        unordered_map<string,DesignEntity> map = {
                {"a", DesignEntity::Assign},
                {"c", DesignEntity::Constant},
        };
        REQUIRE_THROWS(clause.validate(map));
    }

    SECTION("Invalid - mismatched attribute and DE") {
        WithClause clause = WithClause("a.value", "c.stmt#");
        unordered_map<string,DesignEntity> map = {
                {"a", DesignEntity::Assign},
                {"c", DesignEntity::Constant},
        };
        REQUIRE_THROWS(clause.validate(map));
    }

    SECTION("Invalid - undeclared syn") {
        WithClause clause = WithClause("a1.stmt#", "aaaaa2.stmt#");
        unordered_map<string,DesignEntity> map = {
                {"a1", DesignEntity::Assign},
        };
        REQUIRE_THROWS(clause.validate(map));
    }

    SECTION("Invalid - stmtNo Syn") {
        WithClause clause = WithClause("n.stmt#", "4");
        unordered_map<string,DesignEntity> map = {
                {"n", DesignEntity::ProgLine},
        };

        REQUIRE_THROWS(clause.validate(map));
    }
}
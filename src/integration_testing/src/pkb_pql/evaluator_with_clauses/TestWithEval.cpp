#include <vector>
#include "front_end/tokenizer/Tokenizer.h"
#include "front_end/parser/Parser.h"
#include "catch.hpp"
#include "query_processor/Query.h"
#include "query_processor/query_evaluator/QueryResult.h"
#include "utils/SPAEnums.h"
#include "clauses/WithClause.h"
#include "program_knowledge_base/pkb_storage/PKB.h"
#include "../utils/CompareVectors.h"

TEST_CASE("Evaluate With") {
    string src = "procedure x {\n"
                 "  read x;\n"  //1
                 "  print z;\n" //2
                 "  read k;\n" //3
                 "  print k;\n" //4
                 "  call z;\n"  //5
                 " }\n"
                 "procedure y {"
                 "call x;" //6
                 "}"
                 "procedure z {"
                 " t=1+3+5;" //7
                 "}";
    auto stream = Tokenizer::getInstance()->tokenizeSource(src);
    auto ast = Parser::getInstance()->buildAST(stream);
    PKB::getInstance()->reset();
    PKB::getInstance()->initAST(ast);

    Query query;
    query.synonyms = {
            {"p", DesignEntity::Procedure},
            {"p2", DesignEntity::Procedure},
            {"v", DesignEntity::Variable},
            {"v2",DesignEntity::Variable},
            {"r", DesignEntity::Read},
            {"r2", DesignEntity::Read},
            {"pn", DesignEntity::Print},
            {"pn2", DesignEntity::Print},
            {"cl", DesignEntity::Call},
            {"cl2", DesignEntity::Call},
            {"c",DesignEntity::Constant},
            {"c2",DesignEntity::Constant},
            {"n",DesignEntity::ProgLine},
            {"n2",DesignEntity::ProgLine},
            {"a",DesignEntity::Assign},
            {"a2",DesignEntity::Assign}
    };
    vector<Clause*> clauses;
    query.clauses = clauses;
    QueryResult* query_result = QueryResult::init();

    SECTION("ProcProcName = Ident") {
        query.selected = {"p"};
        vector<string> answer = {"x"};
        WithClause with_clause("p.procName","\"x\"");
        with_clause.validate(query.synonyms);
        with_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);
    }

    SECTION("Ident = ProcProcName") {
        query.selected = {"p"};
        vector<string> answer = {"x"};
        WithClause with_clause = WithClause("\"x\"", "p.procName");
        with_clause.validate(query.synonyms);
        with_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);
    }

    SECTION("VarVarName = Ident") {
        query.selected = {"v"};
        vector<string> answer = {"k"};
        WithClause with_clause("v.varName","\"k\"");
        with_clause.validate(query.synonyms);
        with_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);

    }

    SECTION("Ident = VarVarName") {
        query.selected = {"v"};
        vector<string> answer = {"k"};

        WithClause with_clause = WithClause("\"k\"", "v.varName");
        with_clause.validate(query.synonyms);
        with_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);
    }

    SECTION("ReadVarName = IDENT") {
        query.selected = {"r"};
        vector<string> answer = {"3"};
        WithClause with_clause = WithClause( "r.varName", "\"k\"");
        with_clause.validate(query.synonyms);
        with_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);
    }

    SECTION("IDENT = ReadVarName") {
        query.selected = {"r"};
        vector<string> answer = {"1"};
        WithClause with_clause = WithClause( "\"x\"","r.varName");
        with_clause.validate(query.synonyms);
        with_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);
    }

    SECTION("PrintVarName = IDENT") {
        query.selected = {"pn"};
        vector<string> answer = {"4"};
        WithClause with_clause = WithClause( "pn.varName", "\"k\"");
        with_clause.validate(query.synonyms);
        with_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);
    }

    SECTION("ProcProcName = ProcProcName (Same Syn)") {
        query.selected = {"p"};
        vector<string> answer = {"x", "y","z"};
        WithClause with_clause = WithClause( "p.procName", "p.procName");
        with_clause.validate(query.synonyms);
        with_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);
    }

    SECTION("ProcProcName = ProcProcName (Diff Syn)") {
        query.selected = {"p", "p2"};
        vector<string> answer = {"x x", "y y", "z z"};
        WithClause with_clause = WithClause( "p.procName", "p2.procName");
        with_clause.validate(query.synonyms);
        with_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);
    }

    SECTION("CallProcName = CallProcName (Same Syn)") {
        query.selected = {"cl"};
        vector<string> answer = {"5", "6"};
        WithClause with_clause = WithClause( "cl.procName", "cl.procName");
        with_clause.validate(query.synonyms);
        with_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);
    }

    SECTION("CallProcName = CallProcName (Diff Syn)") {
        query.selected = {"cl", "cl2"};
        vector<string> answer = {"5 5", "6 6"};
        WithClause with_clause = WithClause( "cl.procName", "cl2.procName");
        with_clause.validate(query.synonyms);
        with_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);
    }

    SECTION("VarVarName = VarVarName (Same Syn)") {
        query.selected = {"v"};
        vector<string> answer = {"k", "t", "x", "z"};
        WithClause with_clause = WithClause( "v.varName", "v.varName");
        with_clause.validate(query.synonyms);
        with_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);
    }

    SECTION("VarVarName = VarVarName (Diff Syn)") {
        query.selected = {"v", "v2"};
        vector<string> answer = {"k k", "t t", "x x", "z z"};
        WithClause with_clause = WithClause( "v.varName", "v2.varName");
        with_clause.validate(query.synonyms);
        with_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);
    }

    SECTION("ReadVarName = ReadVarName (Same Syn)") {
        query.selected = {"r"};
        vector<string> answer = {"1", "3"};
        WithClause with_clause = WithClause( "r.varName", "r.varName");
        with_clause.validate(query.synonyms);
        REQUIRE(with_clause.evaluate(&query, query_result) == true);
        REQUIRE(query_result->getResults(query.selected) == answer);
    }

    SECTION("ReadVarName = ReadVarName (Diff Syn)") {
        query.selected = {"r", "r2"};
        vector<string> answer = {"1 1", "3 3"};
        WithClause with_clause = WithClause( "r.varName", "r2.varName");
        with_clause.validate(query.synonyms);
        REQUIRE(with_clause.evaluate(&query, query_result) == true);
        REQUIRE(query_result->getResults(query.selected) == answer);
    }

    SECTION("PrintVarName = PrintVarName (Same Syn)") {
        query.selected = {"pn"};
        vector<string> answer = {"2", "4"};
        WithClause with_clause = WithClause( "pn.varName", "pn.varName");
        with_clause.validate(query.synonyms);
        with_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);
    }

    SECTION("PrintVarName = PrintVarName (Diff Syn)") {
        query.selected = {"pn", "pn2"};
        vector<string> answer = {"2 2", "4 4"};
        WithClause with_clause = WithClause( "pn.varName", "pn2.varName");
        with_clause.validate(query.synonyms);
        with_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);
    }

    SECTION("StmtNo = StmtNo (Same Syn)") {
        query.selected = {"r"};
        vector<string> answer = {"1","3"};
        WithClause with_clause = WithClause( "r.stmt#", "r.stmt#");
        with_clause.validate(query.synonyms);
        with_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);
    }

    SECTION("StmtNo = StmtNo (Diff Syn)") {
        query.selected = {"r", "a"};
        vector<string> answer = {};
        WithClause with_clause = WithClause( "r.stmt#", "a.stmt#");
        with_clause.validate(query.synonyms);
        with_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);
    }


    SECTION("Value = Value (Same Syn)") {
        query.selected = {"c"};
        vector<string> answer = {"1","3","5"};
        WithClause with_clause = WithClause( "c.value", "c.value");
        with_clause.validate(query.synonyms);
        with_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);
    }

    SECTION("Value = Value (Diff Syn)") {
        query.selected = {"c", "c2"};
        vector<string> answer = {"1 1","3 3", "5 5"};
        WithClause with_clause = WithClause( "c.value", "c2.value");
        with_clause.validate(query.synonyms);
        with_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);
    }

    SECTION("ProcProcName = ReadVarName") {
        WithClause with_clause( "p.procName", "r.varName");
        query.selected = {"p","r"};
        vector<string> answer = {"x 1"};
        with_clause.validate(query.synonyms);
        with_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);
    }

    SECTION("ReadVarName = ProcProcName") {
        query.selected = {"p","r"};
        vector<string> answer = {"x 1"};

        WithClause with_clause = WithClause( "r.varName", "p.procName");
        with_clause.validate(query.synonyms);
        with_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);
    }

    SECTION("ProcProcName = PrintVarName") {
        query.selected = {"p","pn"};
        vector<string> answer = {"z 2"};

        WithClause with_clause = WithClause( "p.procName", "pn.varName");
        with_clause.validate(query.synonyms);
        with_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);
    }

    SECTION("PrintVarName = ProcProcName") {
        query.selected = {"p","pn"};
        vector<string> answer = {"z 2"};

        WithClause with_clause = WithClause( "pn.varName", "p.procName");
        with_clause.validate(query.synonyms);
        with_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);

    }

    SECTION("ProcProcName = CallProcName") {
        query.selected = {"p","cl"};
        vector<string> answer = {"x 6", "z 5"};
        WithClause with_clause = WithClause( "p.procName", "cl.procName");
        with_clause.validate(query.synonyms);
        with_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);
    }

    SECTION("CallProcName = ProcProcName") {
        query.selected = {"p","cl"};
        vector<string> answer = {"x 6", "z 5"};
         WithClause with_clause = WithClause( "cl.procName", "p.procName");
         with_clause.validate(query.synonyms);
         with_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);
     }

     SECTION("VarVarName = CallProcName") {
        query.selected = {"v","cl"};
         vector<string> answer = {"x 6", "z 5"};
         WithClause with_clause = WithClause( "v.varName", "cl.procName");
         with_clause.validate(query.synonyms);
         with_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);
     }

    SECTION("CallProcName = VarVarName") {
        query.selected = {"v","cl"};
        vector<string> answer = {"x 6", "z 5"};
        WithClause with_clause = WithClause( "cl.procName", "v.varName");
        with_clause.validate(query.synonyms);
        with_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);
    }

    SECTION("ReadVarName = PrintVarName") {
        query.selected = {"r","pn"};
        vector<string> answer = {"3 4"};
        WithClause with_clause = WithClause( "r.varName", "pn.varName");
        with_clause.validate(query.synonyms);
        with_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);
    }

    SECTION("PrintVarName = ReadVarName") {
        query.selected = {"r","pn"};
        vector<string> answer = {"3 4"};

        WithClause with_clause = WithClause( "pn.varName", "r.varName");
        with_clause.validate(query.synonyms);
        with_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);
    }

    SECTION("ReadVarName = CallProcName") {
        query.selected = {"r","cl"};
        vector<string> answer = {"1 6"};

        WithClause with_clause = WithClause( "r.varName", "cl.procName");
        with_clause.validate(query.synonyms);
        with_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);

    }

    SECTION("CallProcName = ReadVarName") {
        query.selected = {"r","cl"};
        vector<string> answer = {"1 6"};

        WithClause with_clause = WithClause( "cl.procName", "r.varName");
        with_clause.validate(query.synonyms);
        with_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);

    }

    SECTION("PrintVarName = CallProcName") {
        query.selected = {"pn","cl"};
        vector<string> answer = {"2 5"};

        WithClause with_clause = WithClause( "cl.procName", "pn.varName");
        with_clause.validate(query.synonyms);
        with_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);
    }

    SECTION("PrintVarName = CallProcName") {
        query.selected = {"pn","cl"};
        vector<string> answer = {"2 5"};

        WithClause with_clause = WithClause( "pn.varName","cl.procName");
        with_clause.validate(query.synonyms);
        with_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);
    }

     SECTION("VarVarName = ReadVarName") {
        query.selected = {"v","r"};
        vector<string> answer = {"k 3","x 1"};
         WithClause with_clause = WithClause( "v.varName", "r.varName");
         with_clause.validate(query.synonyms);
         with_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);
     }

    SECTION("ReadVarName = VarVarName") {
        query.selected = {"v","r"};
        vector<string> answer = {"k 3","x 1"};
        WithClause with_clause = WithClause( "r.varName", "v.varName");
        with_clause.validate(query.synonyms);
        with_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);
    }

    SECTION("VarVarName = PrintVarName") {
        query.selected = {"v","pn"};
        vector<string> answer = {"k 4","z 2"};
        WithClause with_clause = WithClause( "v.varName", "pn.varName");
        with_clause.validate(query.synonyms);
        with_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);
    }

    SECTION("PrintVarName = VarVarName") {
        query.selected = {"v","pn"};
        vector<string> answer = {"k 4","z 2"};
        WithClause with_clause = WithClause( "pn.varName", "v.varName");
        with_clause.validate(query.synonyms);
        with_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);
    }

    SECTION("ProcProcName = VarVarName") {
        query.selected = {"p","v"};
        vector<string> answer = {"x x","z z"};
         WithClause with_clause( "p.procName", "v.varName");
         with_clause.validate(query.synonyms);
         with_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);
     }

    SECTION("VarVarName = ProcProcName") {
        query.selected = {"p","v"};
        vector<string> answer = {"x x","z z"};
        WithClause with_clause = WithClause( "v.varName", "p.procName");
        with_clause.validate(query.synonyms);
        with_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);

    }


    SECTION("IDENT = PrintVarName") {
        query.selected = {"pn"};
        vector<string> answer = {"2"};
        WithClause with_clause = WithClause( "\"z\"","pn.varName");
        with_clause.validate(query.synonyms);
        with_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);
    }

    SECTION("CallProcName = IDENT") {
        query.selected = {"cl"};
        vector<string> answer = {"5"};
        WithClause with_clause = WithClause( "cl.procName", "\"z\"");
        with_clause.validate(query.synonyms);
        with_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);
    }

    SECTION("IDENT = CallProcName") {
        query.selected = {"cl"};
        vector<string> answer = {"5"};
        WithClause with_clause = WithClause( "\"z\"","cl.procName");
        with_clause.validate(query.synonyms);
        with_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);
    }

    SECTION("StmtNo = Value") {
        query.selected = {"r","c"};
        vector<string> answer = {"1 1", "3 3"};
        WithClause with_clause = WithClause( "r.stmt#", "c.value");
        with_clause.validate(query.synonyms);
        with_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);
    }

    SECTION("Value = StmtNo") {
        query.selected = {"cl","c"};
        vector<string> answer = {"5 5"};
        WithClause with_clause = WithClause("c.value","cl.stmt#");
        with_clause.validate(query.synonyms);
        with_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);
    }

     SECTION("StmtNo = Syn") {
        query.selected = {"pn","n"};
        vector<string> answer = {"2 2", "4 4"};
         WithClause with_clause = WithClause( "pn.stmt#", "n");
         with_clause.validate(query.synonyms);
         with_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);
     }

    SECTION("Syn = StmtNo") {
        query.selected = {"a","n"};
        vector<string> answer = {"7 7"};
        WithClause  with_clause = WithClause("n", "a.stmt#");
        with_clause.validate(query.synonyms);
        with_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);
    }

    SECTION("Value = Syn") {
        query.selected = {"c","n"};
        vector<string> answer = {"1 1","3 3","5 5"};
         WithClause with_clause = WithClause( "c.value", "n");
         with_clause.validate(query.synonyms);
         with_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);
     }

    SECTION("Syn = Value") {
        query.selected = {"c","n"};
        vector<string> answer = {"1 1","3 3","5 5"};
        WithClause with_clause = WithClause("n","c.value");
        with_clause.validate(query.synonyms);
        with_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);
    }

      SECTION("StmtNo = Integer") {
        query.selected = {"pn"};
        vector<string> answer = {"4"};
         WithClause with_clause = WithClause( "pn.stmt#", "4");
         with_clause.validate(query.synonyms);
         with_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);
     }

    SECTION("Integer = StmtNo") {
        query.selected = {"a"};
        vector<string> answer = {"7"};
        WithClause with_clause = WithClause( "7","a.stmt#");
        with_clause.validate(query.synonyms);
        with_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);
    }

    SECTION("Value = Integer") {
        query.selected = {"c"};
        vector<string> answer = {"1"};
         WithClause with_clause = WithClause( "c.value", "1");
         with_clause.validate(query.synonyms);
         with_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);
     }

    SECTION("Integer = Value") {
        query.selected = {"c"};
        vector<string> answer = {"5"};
        WithClause with_clause = WithClause( "5","c.value");
        with_clause.validate(query.synonyms);
        with_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);
    }

    SECTION("Integer = Integer") {
       WithClause with_clause = WithClause( "1", "1");
       with_clause.validate(query.synonyms);
       REQUIRE(with_clause.evaluate(&query, query_result) == true);

       with_clause = WithClause( "5","1");
       with_clause.validate(query.synonyms);
       with_clause.evaluate(&query, query_result);
       REQUIRE(with_clause.evaluate(&query, query_result) == false);
   }

    SECTION("Syn = Syn") {
        query.selected = {"n"};
        vector<string> answer = {"1","2","3","4","5","6","7"};
         WithClause with_clause = WithClause( "n", "n");
         with_clause.validate(query.synonyms);
         with_clause.evaluate(&query, query_result);
        REQUIRE(query_result->getResults(query.selected) == answer);
     }

     SECTION("Syn = Integer(Valid)") {
        query.selected = {"n"};
        vector<string> answer = {"5"};
        WithClause with_clause = WithClause( "n", "5");
        with_clause.validate(query.synonyms);
        with_clause.evaluate(&query, query_result);
        REQUIRE(with_clause.evaluate(&query, query_result));
        REQUIRE(query_result->getResults(query.selected) == answer);
    }

    SECTION("Syn = Integer(Invalid)") {
        query.selected = {"n"};
        vector<string> answer = {};
        WithClause with_clause = WithClause( "n", "20");
        with_clause.validate(query.synonyms);
        REQUIRE(with_clause.evaluate(&query, query_result) == false);
        REQUIRE(query_result->getResults(query.selected) == answer);
    }

    SECTION("Integer = Syn(Valid)") {
        query.selected = {"n"};
        vector<string> answer = {"5"};
        WithClause with_clause = WithClause( "5", "n");
        with_clause.validate(query.synonyms);
        REQUIRE(with_clause.evaluate(&query, query_result) == true);
        REQUIRE(query_result->getResults(query.selected) == answer);
    }

    SECTION("Integer = Syn(Invalid)") {
        query.selected = {"n"};
        vector<string> answer = {};
        WithClause with_clause = WithClause( "20", "n");
        with_clause.validate(query.synonyms);
        REQUIRE(with_clause.evaluate(&query, query_result) == false);
        REQUIRE(query_result->getResults(query.selected) == answer);
    }

    SECTION("Syn = Integer (Case 0)") {
        query.selected = {"n"};
        vector<string> answer = {};
        WithClause with_clause = WithClause( "n", "0");
        with_clause.validate(query.synonyms);
        REQUIRE(with_clause.evaluate(&query, query_result) == false);
        REQUIRE(query_result->getResults(query.selected) == answer);
    }

}

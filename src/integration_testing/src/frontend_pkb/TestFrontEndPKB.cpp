#include "catch.hpp"
#include "front_end/tokenizer/Tokenizer.h"
#include "front_end/parser/Parser.h"
#include "program_knowledge_base/pkb_storage/PKB.h"
#include "../pkb_pql/utils/CompareVectors.h"

TEST_CASE("Front End to PKB Integration Test") {

    string src = "procedure Example {\n"
                      "  x = 2;\n"  //1
                      "  z = 3;\n" //2
                      "  i = 5;\n" //3
                      "  read x;\n" //4
                      "  while (i!=0) {\n" //5
                      "    x = x - 1;\n" //6
                      "    if (x==1) then {\n" //7
                      "      z = x + 1; \n" //8
                      "      print y;}\n" //9
                      "    else {\n"
                      "      y = z + x; }\n" //10
                      "    z = z + x + i;\n" //11
                      "    i = i - 1; }\n" //12
                      "  }\n"
                      "\n"
                      "procedure p {\n"
                      "  if (k<9) then {\n" //13
                      "    while (j>8) {\n" //14
                      "      x = z * 3 + 2 * y;\n" //15
                      "      i = i - 1; }\n" //16
                      "    d = x + 1;\n" //17
                      "    z = x + m; }\n"  //18
                      "  else {\n"
                      "    z = 1; }\n" //19
                      "  z = z + x + i; }\n" //20
                      "\n"
                      "procedure q {\n"
                      "  if (x==1) then {\n" //21
                      "    z = x + 1; }\n" //22
                      "  else {\n"
                      "    x = z + x; }" //23
                      "   z=x;" //24
                      "   call x;}" //25
                      "procedure x{"
                      "   call p;" //26
                      "}";

    Tokenizer *tokenizer = Tokenizer::getInstance();
    vector<Token> tokenStream = tokenizer->tokenizeSource(src);
    Parser* parser = Parser::getInstance();
    AST root = parser->buildAST(tokenStream);
    PKB *pkb = PKB::getInstance();
    pkb->reset();
    pkb->initAST(root);

/*Create sample-source.txt test file and check following tables*/
    SECTION("MASTER LIST") {

        vector<string> proc_ans = {"Example", "p", "q", "x"};
        vector<int> stmt_ans{};
        for (int i = 1; i <= 26; i++) {
            stmt_ans.emplace_back(i);
        }

        vector<int> assign_ans{1, 2, 3, 6, 8, 10, 11, 12, 15, 16, 17, 18, 19, 20, 22, 23, 24};
        vector<int> if_ans{7, 13, 21};
        vector<int> while_ans{5, 14};
        vector<int> read_ans{4};
        vector<int> print_ans{9};
        vector<int> call_ans{25,26};

        REQUIRE(CompareVectors::sorted(proc_ans) == CompareVectors::sorted(pkb->getAllProcedure()));
        REQUIRE(CompareVectors::sorted(stmt_ans) == CompareVectors::sorted(pkb->getAllStmt()));
        REQUIRE(CompareVectors::sorted(assign_ans) == CompareVectors::sorted(pkb->getAllAssign()));
        REQUIRE(CompareVectors::sorted(if_ans) == CompareVectors::sorted(pkb->getAllIf()));
        REQUIRE(CompareVectors::sorted(while_ans) == CompareVectors::sorted(pkb->getAllWhile()));
        REQUIRE(CompareVectors::sorted(read_ans) == CompareVectors::sorted(pkb->getAllRead()));
        REQUIRE(CompareVectors::sorted(print_ans) == CompareVectors::sorted(pkb->getAllPrint()));
        REQUIRE(CompareVectors::sorted(call_ans) == CompareVectors::sorted(pkb->getAllCall()));
    }

    SECTION("FOLLOWS") {
        //Direct Follows
        REQUIRE(pkb->isFollows(1, 2) == true);
        REQUIRE(pkb->isFollows(2, 3) == true);
        REQUIRE(pkb->isFollows(3, 4) == true);
        REQUIRE(pkb->isFollows(4, 5) == true);
        REQUIRE(pkb->isFollows(6, 7) == true);
        REQUIRE(pkb->isFollows(14, 17) == true);
        REQUIRE(pkb->isFollows(15, 16) == true);
        REQUIRE(pkb->isFollows(17, 18) == true);

        //Indirect Follows

        REQUIRE(pkb->isFollows(1, 3) == false);
        REQUIRE(pkb->isFollows(1, 4) == false);
        REQUIRE(pkb->isFollows(1, 5) == false);
        REQUIRE(pkb->isFollows(2, 4) == false);
        REQUIRE(pkb->isFollows(2, 5) == false);
        REQUIRE(pkb->isFollows(3, 5) == false);
        REQUIRE(pkb->isFollows(14, 18) == false);

        //Follows: While or if statement
        REQUIRE(pkb->isFollows(7, 11) == true);
        REQUIRE(pkb->isFollows(14, 17) == true);
        REQUIRE(pkb->isFollows(13, 20) == true);

        //Different Procedures Follows
        REQUIRE(pkb->isFollows(12, 13) == false);
        REQUIRE(pkb->isFollows(20, 21) == false);

        //Backwards Follows
        REQUIRE(pkb->isFollows(18, 17) == false);

        //Different Statement List
        REQUIRE(pkb->isFollows(5, 6) == false);
        REQUIRE(pkb->isFollows(7, 8) == false);
        REQUIRE(pkb->isFollows(9, 10) == false);
        REQUIRE(pkb->isFollows(13, 14) == false);

        //Non-existent Statement
        REQUIRE(pkb->isFollows(13, 200) == false);
    }

    SECTION("FOLLOW*") {
        //Direct Follows
        REQUIRE(pkb->isFollowsStar(1, 2) == true);
        REQUIRE(pkb->isFollowsStar(2, 3) == true);
        REQUIRE(pkb->isFollowsStar(3, 4) == true);
        REQUIRE(pkb->isFollowsStar(4, 5) == true);
        REQUIRE(pkb->isFollowsStar(6, 7) == true);
        REQUIRE(pkb->isFollows(14, 17) == true);
        REQUIRE(pkb->isFollowsStar(15, 16) == true);
        REQUIRE(pkb->isFollowsStar(17, 18) == true);

        //Indirect Follows
        REQUIRE(pkb->isFollowsStar(1, 3) == true);
        REQUIRE(pkb->isFollowsStar(1, 4) == true);
        REQUIRE(pkb->isFollowsStar(1, 5) == true);
        REQUIRE(pkb->isFollowsStar(2, 4) == true);
        REQUIRE(pkb->isFollowsStar(2, 5) == true);
        REQUIRE(pkb->isFollowsStar(3, 5) == true);
        REQUIRE(pkb->isFollowsStar(14, 18) == true);

        //Backwards Follows
        REQUIRE(pkb->isFollowsStar(18, 17) == false);

        //Different Procedures
        REQUIRE(pkb->isFollowsStar(12, 13) == false);
        REQUIRE(pkb->isFollowsStar(20, 21) == false);

        //Backwards Follows
        REQUIRE(pkb->isFollowsStar(18, 17) == false);

        //Different Statement List
        REQUIRE(pkb->isFollowsStar(5, 6) == false);
        REQUIRE(pkb->isFollowsStar(7, 8) == false);
        REQUIRE(pkb->isFollowsStar(9, 10) == false);
        REQUIRE(pkb->isFollowsStar(13, 14) == false);

        //Non-existent Statement
        REQUIRE(pkb->isFollowsStar(13, 200) == false);
    }

    SECTION("PARENT") {
        //Direct Parent
        REQUIRE(pkb->isParent(5, 6) == true);
        REQUIRE(pkb->isParent(5, 7) == true);
        REQUIRE(pkb->isParent(5, 11) == true);
        REQUIRE(pkb->isParent(5, 12) == true);
        REQUIRE(pkb->isParent(7, 8) == true);
        REQUIRE(pkb->isParent(7, 9) == true);
        REQUIRE(pkb->isParent(7, 10) == true);
        REQUIRE(pkb->isParent(13, 14) == true);
        REQUIRE(pkb->isParent(13, 17) == true);
        REQUIRE(pkb->isParent(13, 18) == true);
        REQUIRE(pkb->isParent(13, 19) == true);
        REQUIRE(pkb->isParent(21, 22) == true);
        REQUIRE(pkb->isParent(21, 23) == true);

        //Indirect Parents
        REQUIRE(pkb->isParent(5, 8) == false);
        REQUIRE(pkb->isParent(5, 9) == false);
        REQUIRE(pkb->isParent(5, 10) == false);
        REQUIRE(pkb->isParent(13, 15) == false);
        REQUIRE(pkb->isParent(13, 16) == false);

        //Backwards Parent
        REQUIRE(pkb->isParent(6, 5) == false);

        //Different Procedure
        REQUIRE(pkb->isParent(20, 21) == false);

        //Not a Parent
        REQUIRE(pkb->isParent(1, 2) == false);
        REQUIRE(pkb->isParent(7, 11) == false);
        REQUIRE(pkb->isParent(14, 18) == false);
        REQUIRE(pkb->isParent(13, 20) == false);

        //Non-existent Statement
        REQUIRE(pkb->isParent(13, 200) == false);
    }

    SECTION("PARENT*") {
        //Direct Parent
        REQUIRE(pkb->isParentStar(5, 6) == true);
        REQUIRE(pkb->isParentStar(5, 7) == true);
        REQUIRE(pkb->isParentStar(5, 11) == true);
        REQUIRE(pkb->isParentStar(5, 12) == true);
        REQUIRE(pkb->isParentStar(7, 8) == true);
        REQUIRE(pkb->isParentStar(7, 9) == true);
        REQUIRE(pkb->isParentStar(7, 10) == true);
        REQUIRE(pkb->isParentStar(13, 14) == true);
        REQUIRE(pkb->isParentStar(13, 17) == true);
        REQUIRE(pkb->isParentStar(13, 18) == true);
        REQUIRE(pkb->isParentStar(13, 19) == true);
        REQUIRE(pkb->isParentStar(21, 22) == true);
        REQUIRE(pkb->isParentStar(21, 23) == true);

        //Indirect Parents
        REQUIRE(pkb->isParentStar(5, 8) == true);
        REQUIRE(pkb->isParentStar(5, 9) == true);
        REQUIRE(pkb->isParentStar(5, 10) == true);
        REQUIRE(pkb->isParentStar(13, 15) == true);
        REQUIRE(pkb->isParentStar(13, 16) == true);

        //Backwards Parent
        REQUIRE(pkb->isParentStar(6, 5) == false);

        //Different Procedure
        REQUIRE(pkb->isParentStar(20, 21) == false);

        //Not a Parent
        REQUIRE(pkb->isParentStar(1, 2) == false);
        REQUIRE(pkb->isParentStar(7, 11) == false);
        REQUIRE(pkb->isParentStar(14, 18) == false);
        REQUIRE(pkb->isParentStar(13, 20) == false);

        //Non-existent Statement
        REQUIRE(pkb->isParentStar(13, 200) == false);
    }

    SECTION("USES (Variables)") {
        //Procedures
        vector<string> example_uses{"i", "z", "x", "y"};
        vector<string> p_uses{"z", "x", "i", "k", "m", "j", "y"};
        vector<string> q_uses{"z", "x", "i", "k", "m", "j", "y"};
        REQUIRE(pkb->getAllVariablesUsedBy("Example") == example_uses);
        REQUIRE(CompareVectors::sorted(pkb->getAllVariablesUsedBy("p")) == CompareVectors::sorted(p_uses));
        REQUIRE(CompareVectors::sorted(pkb->getAllVariablesUsedBy("q")) == CompareVectors::sorted(q_uses));

        //Read Uses
        REQUIRE(pkb->isUses(4, "x") == false);

        //Print Uses
        REQUIRE(pkb->isUses(9, "y") == true);

        //Statement Modifies
        vector<string> stmt_6 = {"x"};
        vector<string> stmt_8 = {"x"};
        vector<string> stmt_10 = {"z", "x"};
        vector<string> stmt_11 = {"z", "x", "i"};
        vector<string> stmt_12 = {"i"};
        REQUIRE(pkb->getAllVariablesUsedBy(6) == stmt_6);
        REQUIRE(pkb->getAllVariablesUsedBy(8) == stmt_8);
        REQUIRE(pkb->getAllVariablesUsedBy(10) == stmt_10);
        REQUIRE(pkb->getAllVariablesUsedBy(11) == stmt_11);
        REQUIRE(pkb->getAllVariablesUsedBy(12) == stmt_12);

        //Conditional Statements
        vector<string> stmt_5 = {"i", "z", "x", "y"};
        vector<string> stmt_7 = {"x", "z", "y"};
        vector<string> stmt_13 = {"k", "x", "m", "j", "i", "z", "y"};
        vector<string> stmt_14 = {"j", "i", "z", "y"};
        REQUIRE(pkb->getAllVariablesUsedBy(5) == stmt_5);
        REQUIRE(CompareVectors::sorted(pkb->getAllVariablesUsedBy(7)) == CompareVectors::sorted(stmt_7));
        REQUIRE(pkb->getAllVariablesUsedBy(13) == stmt_13);
        REQUIRE(pkb->getAllVariablesUsedBy(14) == stmt_14);

        //Is Used
        vector<string> var_ans{"j", "k", "i", "m", "y", "z", "x"};
        REQUIRE(CompareVectors::sorted(var_ans) == CompareVectors::sorted(pkb->getAllUsedVar()));

        //Fake variable
        REQUIRE(pkb->isUses("Example", "fake") == false);
        REQUIRE(pkb->isUses(2, "fake") == false);
        REQUIRE(pkb->isUsed("fake") == false);
    }

    SECTION("USES (Constants)") {
        //Procedures
        vector<string> example_uses{"0", "1", "5", "3", "2"};
        vector<string> p_uses{"9", "1", "8", "3", "2"};
        vector<string> q_uses{"9", "1", "8", "3", "2"};
        REQUIRE(pkb->getAllConstantsUsedBy("Example") == example_uses);
        REQUIRE(CompareVectors::sorted(pkb->getAllConstantsUsedBy("p")) == CompareVectors::sorted(p_uses));
        REQUIRE(CompareVectors::sorted(pkb->getAllConstantsUsedBy("q")) == CompareVectors::sorted(q_uses));

        //Statement Modifies
        vector<string> stmt_6 = {"1"};
        vector<string> stmt_8 = {"1"};
        vector<string> stmt_10 = {};
        vector<string> stmt_11 = {};
        vector<string> stmt_12 = {"1"};
        REQUIRE(pkb->getAllConstantsUsedBy(6) == stmt_6);
        REQUIRE(pkb->getAllConstantsUsedBy(8) == stmt_8);
        REQUIRE(pkb->getAllConstantsUsedBy(10) == stmt_10);
        REQUIRE(pkb->getAllConstantsUsedBy(11) == stmt_11);
        REQUIRE(pkb->getAllConstantsUsedBy(12) == stmt_12);

        //Conditional Statements
        vector<string> stmt_5 = {"0", "1"};
        vector<string> stmt_7 = {"1"};
        vector<string> stmt_13 = {"9", "1", "8", "3", "2"};
        vector<string> stmt_14 = {"8", "1", "3", "2"};
        REQUIRE(pkb->getAllConstantsUsedBy(5) == stmt_5);
        REQUIRE(pkb->getAllConstantsUsedBy(7) == stmt_7);
        REQUIRE(pkb->getAllConstantsUsedBy(13) == stmt_13);
        REQUIRE(pkb->getAllConstantsUsedBy(14) == stmt_14);

        //Is Used
        vector<string> const_ans{"5", "0", "2", "3", "8", "9", "1"};
        REQUIRE(CompareVectors::sorted(const_ans) == CompareVectors::sorted(pkb->getAllUsedConst()));
    }

    SECTION("MODIFIES") {
        //Procedures
        vector<string> example_mod{"i", "z", "y", "x"};
        vector<string> p_mod{"z", "d", "i", "x"};
        vector<string> q_mod{"z", "d", "i", "x"};
        REQUIRE(pkb->getAllVariablesModifiedBy("Example") == example_mod);
        REQUIRE(CompareVectors::sorted(pkb->getAllVariablesModifiedBy("p")) == CompareVectors::sorted(p_mod));
        REQUIRE(CompareVectors::sorted(pkb->getAllVariablesModifiedBy("q")) == CompareVectors::sorted(q_mod));

        //Statement Modifies
        vector<string> stmt_1 = {"x"};
        vector<string> stmt_2 = {"z"};
        vector<string> stmt_3 = {"i"};
        vector<string> stmt_6 = {"x"};
        vector<string> stmt_8 = {"z"};
        vector<string> stmt_11 = {"z"};
        vector<string> stmt_12 = {"i"};
        vector<string> stmt_15 = {"x"};
        REQUIRE(pkb->getAllVariablesModifiedBy(1) == stmt_1);
        REQUIRE(pkb->getAllVariablesModifiedBy(2) == stmt_2);
        REQUIRE(pkb->getAllVariablesModifiedBy(3) == stmt_3);
        REQUIRE(pkb->getAllVariablesModifiedBy(6) == stmt_6);
        REQUIRE(pkb->getAllVariablesModifiedBy(8) == stmt_8);
        REQUIRE(pkb->getAllVariablesModifiedBy(11) == stmt_11);
        REQUIRE(pkb->getAllVariablesModifiedBy(12) == stmt_12);
        REQUIRE(pkb->getAllVariablesModifiedBy(15) == stmt_15);

        //Read
        REQUIRE(pkb->isModifies(4, "x") == true);

        //Print
        REQUIRE(pkb->isModifies(9, "y") == false);

        //Conditional Statements
        vector<string> stmt_5 = {"i", "z", "y", "x"};
        vector<string> stmt_7 = {"y", "z",};
        vector<string> stmt_13 = {"z", "d", "i", "x"};
        vector<string> stmt_14 = {"i", "x"};
        vector<string> stmt_21 = {"x", "z"};
        REQUIRE(CompareVectors::sorted(pkb->getAllVariablesModifiedBy(5)) == CompareVectors::sorted(stmt_5));
        REQUIRE(CompareVectors::sorted(pkb->getAllVariablesModifiedBy(7) )== CompareVectors::sorted(stmt_7));
        REQUIRE(CompareVectors::sorted(pkb->getAllVariablesModifiedBy(13)) == CompareVectors::sorted(stmt_13));
        REQUIRE(CompareVectors::sorted(pkb->getAllVariablesModifiedBy(14) )== CompareVectors::sorted(stmt_14));
        REQUIRE(CompareVectors::sorted(pkb->getAllVariablesModifiedBy(21)) == CompareVectors::sorted(stmt_21));

        //Is Modified
        vector<string> all_mod = {"i", "y", "d", "x", "z"};
        REQUIRE(CompareVectors::sorted(pkb->getAllModified()) == CompareVectors::sorted(all_mod));

        //Fake variable
        REQUIRE(pkb->isModifies("Example", "fake") == false);
        REQUIRE(pkb->isModifies(2, "fake") == false);
        REQUIRE(pkb->isModified("fake") == false);
    }

    //Section Calls
    SECTION("CALLS"){
        vector<string> no_ans{};
        vector<string> get_q_calls_ans{"x"};
        vector<string> get_x_calls_ans{"p"};

        REQUIRE(pkb->getCalls("p") == no_ans);
        REQUIRE(pkb->getCalls("Example") == no_ans);
        REQUIRE(pkb->getCalls("q") ==get_q_calls_ans);
        REQUIRE(pkb->getCalls("x") ==get_x_calls_ans);
    }

    SECTION("CALLS*"){
        vector<string> no_ans{};
        vector<string> get_q_calls_ans{"x","p"};
        vector<string> get_x_calls_ans{"p"};

        REQUIRE(CompareVectors::sorted(pkb->getCallsStar("p")) == CompareVectors::sorted(no_ans));
        REQUIRE(CompareVectors::sorted(pkb->getCallsStar("Example")) == CompareVectors::sorted(no_ans));
        REQUIRE(CompareVectors::sorted(pkb->getCallsStar("q")) == CompareVectors::sorted(get_q_calls_ans));
        REQUIRE(CompareVectors::sorted(pkb->getCallsStar("x")) ==CompareVectors::sorted(get_x_calls_ans));
    }


    SECTION("NEXT") {
        vector<int> no_ans{};
        vector<int> one_ans{2};
        vector<int> five_ans{6};
        vector<int> seven_ans{8,10};
        vector<int> eight_ans{9};
        vector<int> ten_ans{11};
        vector<int> twentyone_ans{22,23};
        vector<int> twentytwo_ans{24};
        vector<int> twentythree_ans{24};

        REQUIRE(CompareVectors::sorted(pkb->getNextStatements(1)) == CompareVectors::sorted(one_ans));
        REQUIRE(CompareVectors::sorted(pkb->getNextStatements(5)) == CompareVectors::sorted(five_ans));
        REQUIRE(CompareVectors::sorted(pkb->getNextStatements(7)) == CompareVectors::sorted(seven_ans));
        REQUIRE(CompareVectors::sorted(pkb->getNextStatements(8)) == CompareVectors::sorted(eight_ans));
        REQUIRE(CompareVectors::sorted(pkb->getNextStatements(10)) == CompareVectors::sorted(ten_ans));
        REQUIRE(CompareVectors::sorted(pkb->getNextStatements(21)) == CompareVectors::sorted(twentyone_ans));
        REQUIRE(CompareVectors::sorted(pkb->getNextStatements(22)) == CompareVectors::sorted(twentytwo_ans));
        REQUIRE(CompareVectors::sorted(pkb->getNextStatements(23)) == CompareVectors::sorted(twentythree_ans));
    }


    SECTION("NEXT*") {
        vector<int> no_ans{};
        vector<int> one_ans{2,3,4,5,6,7,8,9,10,11,12};
        vector<int> five_ans{5,6,7,8,9,10,11,12};
        vector<int> seven_ans{5,6,7,8,9,10,11,12};
        vector<int> eight_ans{5,6,7,8,9,10,11,12};
        vector<int> ten_ans{5,6,7,8,9,10,11,12};
        vector<int> twentyone_ans{22,23,24,25};
        vector<int> twentytwo_ans{24,25};
        vector<int> twentythree_ans{24,25};
        REQUIRE(CompareVectors::sorted(pkb->getNextStar(1)) == CompareVectors::sorted(one_ans));
        REQUIRE(CompareVectors::sorted(pkb->getNextStar(5)) == CompareVectors::sorted(five_ans));
        REQUIRE(CompareVectors::sorted(pkb->getNextStar(7)) == CompareVectors::sorted(seven_ans));
        REQUIRE(CompareVectors::sorted(pkb->getNextStar(8)) == CompareVectors::sorted(eight_ans));
        REQUIRE(CompareVectors::sorted(pkb->getNextStar(10)) == CompareVectors::sorted(ten_ans));
        REQUIRE(CompareVectors::sorted(pkb->getNextStar(21)) == CompareVectors::sorted(twentyone_ans));
        REQUIRE(CompareVectors::sorted(pkb->getNextStar(22)) == CompareVectors::sorted(twentytwo_ans));
        REQUIRE(CompareVectors::sorted(pkb->getNextStar(23)) == CompareVectors::sorted(twentythree_ans));
    }

    // Evaluate Affects
    SECTION("AFFECTS") {
        vector<int> no_ans{};
        vector<int> two_ans{10,11};
        vector<int> three_ans{12,11};
        vector<int> eighteen_ans{20};
        vector<int> twentythree_ans{24};
        vector<int> twelve_ans{11,12};

        REQUIRE(CompareVectors::sorted(pkb->getAffects(1)) == CompareVectors::sorted(no_ans));
        REQUIRE(CompareVectors::sorted(pkb->getAffects(2)) == CompareVectors::sorted(two_ans));
        REQUIRE(CompareVectors::sorted(pkb->getAffects(3)) == CompareVectors::sorted(three_ans));
        REQUIRE(CompareVectors::sorted(pkb->getAffects(12)) == CompareVectors::sorted(twelve_ans));
        REQUIRE(CompareVectors::sorted(pkb->getAffects(18)) == CompareVectors::sorted(eighteen_ans));
        REQUIRE(CompareVectors::sorted(pkb->getAffects(22)) == CompareVectors::sorted(no_ans));
        REQUIRE(CompareVectors::sorted(pkb->getAffects(23)) == CompareVectors::sorted(twentythree_ans));
    }

    SECTION("AFFECTS*") {
        vector<int> no_ans{};
        vector<int> two_ans{10,11};
        vector<int> three_ans{10,12,11};
        vector<int> eighteen_ans{20};
        vector<int> twentythree_ans{24};
        vector<int> twelve_ans{10,11,12};

        //1 2 3 17 18 22
        REQUIRE(CompareVectors::sorted(pkb->getAffectsStar(1)) == CompareVectors::sorted(no_ans));
        REQUIRE(CompareVectors::sorted(pkb->getAffectsStar(2)) == CompareVectors::sorted(two_ans));
        REQUIRE(CompareVectors::sorted(pkb->getAffectsStar(3)) == CompareVectors::sorted(three_ans));
        REQUIRE(CompareVectors::sorted(pkb->getAffectsStar(12)) == CompareVectors::sorted(twelve_ans));
        REQUIRE(CompareVectors::sorted(pkb->getAffectsStar(18)) == CompareVectors::sorted(eighteen_ans));
        REQUIRE(CompareVectors::sorted(pkb->getAffectsStar(22)) == CompareVectors::sorted(no_ans));
        REQUIRE(CompareVectors::sorted(pkb->getAffectsStar(23)) == CompareVectors::sorted(twentythree_ans));
    }

}








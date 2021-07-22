#include "catch.hpp"
#include <vector>
#include "front_end/parser/TNode.h"
#include "program_knowledge_base/pkb_storage/PKB.h"
#include "front_end/tokenizer/Tokenizer.h"
#include "front_end/parser/Parser.h"

using namespace std;
vector<string> sorted(vector<string> v) {
    std::sort(v.begin(), v.end());
    return v;
}

vector<int> sorted(vector<int> v) {
    std::sort(v.begin(), v.end());
    return v;
}


TEST_CASE("Add Procedures") {
    shared_ptr<ProgramNode> root(new ProgramNode());
    std::vector<std::shared_ptr<ProcedureNode>> root_list= {};
    for (int i = 0; i < 10; i++) {
        root_list.emplace_back(std::make_shared<ProcedureNode>(ProcedureNode("proc" + std::to_string(i))));

    }
    root->addProcedureList(root_list);
    PKB test_pkb = PKB();
    test_pkb.initAST(root);
    vector<string> procList = test_pkb.getAllProcedure();
    vector<string> answer = {"proc0", "proc1", "proc2", "proc3", "proc4", "proc5", "proc6", "proc7", "proc8", "proc9"};
    REQUIRE(procList == answer);
    REQUIRE(procList[0] == answer[0]);
    REQUIRE(procList[4] == answer[4]);
    REQUIRE(procList[7] == answer[7]);
    REQUIRE(procList[9] == answer[9]);
}

TEST_CASE("Add Statements") {
    PKB test_pkb = PKB();
    shared_ptr<ProgramNode> root(new ProgramNode());
    std::vector<std::shared_ptr<ProcedureNode>> root_list= {};
    shared_ptr<ProcedureNode> procedure(new ProcedureNode("rootProc"));
    root_list.push_back(procedure);
    root->addProcedureList(root_list);

    std::vector<std::shared_ptr<StmtNode>> procList= {};
    procList.push_back(make_shared<ReadNode>(ReadNode(procedure, make_shared<VariableNode>(VariableNode("a1")))));
    procList.push_back(make_shared<ReadNode>(ReadNode(procedure, make_shared<VariableNode>(VariableNode("a2")))));
    procList.push_back(make_shared<ReadNode>(ReadNode(procedure, make_shared<VariableNode>(VariableNode("a3")))));


    shared_ptr<VariableNode> left = make_shared<VariableNode>(VariableNode("A"));
    shared_ptr<VariableNode> right = make_shared<VariableNode>(VariableNode("B"));


    shared_ptr<RelExpressionNode> cond = make_shared<RelExpressionNode>(RelExpressionNode("==", left, right));
    shared_ptr<IfNode> ifNode = make_shared<IfNode>(IfNode(procedure, cond));
    procList.push_back(ifNode);
    std::vector<std::shared_ptr<StmtNode>> if_list= {};
    std::vector<std::shared_ptr<StmtNode>> elseList= {};

    if_list.push_back(make_shared<ReadNode>(ReadNode(procedure, make_shared<VariableNode>(VariableNode("a5")))));
    if_list.push_back(make_shared<ReadNode>(ReadNode(procedure, make_shared<VariableNode>(VariableNode("a6")))));
    ifNode->addStatementList(if_list);
    ifNode->addElseStatementList(elseList);
    procList.push_back(make_shared<ReadNode>(ReadNode(procedure, make_shared<VariableNode>(VariableNode("a7")))));

    shared_ptr<CondExpressionNode> cond2 = make_shared<CondExpressionNode>("NULL");
    shared_ptr<IfNode> ifNode2 = make_shared<IfNode>(IfNode(procedure, cond));
    procList.push_back(ifNode2);
    std::vector<std::shared_ptr<StmtNode>> ifList2= {};
    std::vector<std::shared_ptr<StmtNode>> elseList2= {};

    shared_ptr<CondExpressionNode> cond3 = make_shared<CondExpressionNode>("NULL");
    shared_ptr<IfNode> ifNode3 = make_shared<IfNode>(IfNode(ifNode2, cond));
    ifList2.push_back(ifNode3);
    std::vector<std::shared_ptr<StmtNode>> ifList3= {};
    std::vector<std::shared_ptr<StmtNode>> elseList3= {};

    shared_ptr<CondExpressionNode> cond4 = make_shared<CondExpressionNode>("NULL");
    shared_ptr<IfNode> ifNode4 = make_shared<IfNode>(IfNode(ifNode3, cond));
    ifList3.push_back(ifNode4);
    std::vector<std::shared_ptr<StmtNode>> ifList4= {};
    std::vector<std::shared_ptr<StmtNode>> elseList4= {};

    ifNode3->addStatementList(ifList3);
    ifNode3->addElseStatementList(elseList3);

    ifNode2->addStatementList(ifList2);
    ifNode2->addElseStatementList(elseList2);

    procedure->addStatementList(procList);

    /*test_pkb.initAST(root);
    REQUIRE(test_pkb.isFollows(1, 2));
    REQUIRE(test_pkb.isFollows(3, 4));
    REQUIRE(test_pkb.isFollowsStar(1, 7));
    REQUIRE(test_pkb.isParent(4, 5));
    REQUIRE(test_pkb.isParent(8, 9));
    REQUIRE(test_pkb.isParentStar(9, 10));
    REQUIRE(test_pkb.isParentStar(8, 10));
     */

}

TEST_CASE("Add Statement") {
    //TODO
    REQUIRE(true);
}

TEST_CASE("Insert Follows") {
    PKB test_pkb = PKB();
    
    test_pkb.insertFollows(1, 2);
    test_pkb.insertFollows(2, 3);

    int result1 = test_pkb.follows_list.at(1);
    int result2 = test_pkb.inverted_follows_list.at(3);
    bool result3 = test_pkb.follows_list.count(1);

    int answer1 = 2;
    int answer2 = 2;

    REQUIRE(result1 == answer1);
    REQUIRE(result2 == answer2);
    REQUIRE(result3 == true);
}

TEST_CASE("isFollows") {
    PKB test_pkb = PKB();

    test_pkb.follows_list.insert({ 1,2 });
    test_pkb.follows_list.insert({ 2,3 });

    bool result1 = test_pkb.isFollows(1, 2);
    bool result2 = test_pkb.isFollows(3, 2);

    REQUIRE(result1 == true);
    REQUIRE(result2 == false);
}

TEST_CASE("Insert Follows Star") {
    PKB test_pkb = PKB();

    test_pkb.insertFollowsStar(2, 3);
    test_pkb.insertFollowsStar(2, 10);

    vector<int> result1 = test_pkb.follows_star_list.at(2);
    vector<int> result2 = test_pkb.inverted_follows_star_list.at(3);

    vector<int> answer1 = { 3,10 };
    vector<int> answer2 = { 2 };

    REQUIRE(result1 == answer1);
    REQUIRE(result2 == answer2);
}

TEST_CASE("Update Follows Star") {
    PKB test_pkb = PKB();
    test_pkb.follows_list.insert({1,2});
    test_pkb.follows_list.insert({2,3});
    test_pkb.follows_list.insert({3,4});
    test_pkb.follows_list.insert({4,-1});
    test_pkb.follows_list.insert({5,6});
    test_pkb.follows_list.insert({6,-1});
    test_pkb.follows_list.insert({7,8});
    test_pkb.follows_list.insert({8,-1});

    test_pkb.inverted_follows_list.insert({2,1});
    test_pkb.inverted_follows_list.insert({3,2});
    test_pkb.inverted_follows_list.insert({4,3});
    test_pkb.inverted_follows_list.insert({-4,4});
    test_pkb.inverted_follows_list.insert({6,5});
    test_pkb.inverted_follows_list.insert({-6,6});
    test_pkb.inverted_follows_list.insert({8,7});
    test_pkb.inverted_follows_list.insert({-8,8});

    test_pkb.updateFollowsStar();

    vector<int> result1 = test_pkb.follows_star_list.at(1);
    vector<int> result2 = test_pkb.follows_star_list.at(2);
    vector<int> result3 = test_pkb.follows_star_list.at(8);
    result1 = sorted(result1);
    result2 = sorted(result2);

    vector<int> answer1 = {2, 3, 4};
    vector<int> answer2 = {3, 4};
    vector<int> answer3 = {};
    answer1 = sorted(answer1);
    answer2 = sorted(answer2);

    REQUIRE(result1 == answer1);
    REQUIRE(result2 == answer2);
    REQUIRE(result3 == answer3);
}

TEST_CASE("isFollowsStar") {
    PKB test_pkb = PKB();

    test_pkb.follows_star_list[2].push_back(3);
    test_pkb.follows_star_list[3].push_back(10);
    test_pkb.follows_star_list[2].push_back(10);

    bool result1 = test_pkb.isFollowsStar(2, 3);
    bool result2 = test_pkb.isFollowsStar(3, 9);
    bool result3 = test_pkb.isFollowsStar(2, 10);

    REQUIRE(result1 == true);
    REQUIRE(result2 == false);
    REQUIRE(result3 == true);
}

TEST_CASE("Insert Parent") {
    PKB test_pkb = PKB();

    test_pkb.insertParent(1, 2);
    test_pkb.insertParent(1, 3);

    bool result1 = find(test_pkb.parent_list[1].begin(), test_pkb.parent_list[1].end(), 2) != 
        test_pkb.parent_list[1].end();
    bool result2 = find(test_pkb.parent_list[1].begin(), test_pkb.parent_list[1].end(), 3) !=
        test_pkb.parent_list[1].end();
    bool result3 = find(test_pkb.parent_list[3].begin(), test_pkb.parent_list[3].end(), 2) !=
        test_pkb.parent_list[3].end();;

    REQUIRE(result1 == true);
    REQUIRE(result2 == true);
    REQUIRE(result3 == false);

}

TEST_CASE("isParent") {
    PKB test_pkb = PKB();

    test_pkb.parent_list[1].push_back(2);
    test_pkb.inverted_parent_list.insert({ 2,1 });
    test_pkb.parent_list[2].push_back(3);
    test_pkb.inverted_parent_list.insert({ 3,2 });

    bool result1 = test_pkb.isParent(1, 2);
    bool result2 = test_pkb.isParent(3, 2);

    REQUIRE(result1 == true);
    REQUIRE(result2 == false);
}

TEST_CASE("Insert Parent Star") {
    PKB test_pkb = PKB();

    test_pkb.insertParentStar(2, 3);
    test_pkb.insertParentStar(3, 10);
    test_pkb.insertParentStar(2, 10);

    bool result1 = find(test_pkb.parent_star_list[2].begin(), test_pkb.parent_star_list[2].end(), 3) !=
        test_pkb.parent_star_list[2].end();
    bool result2 = find(test_pkb.parent_star_list[3].begin(), test_pkb.parent_star_list[3].end(), 10) !=
        test_pkb.parent_star_list[3].end();
    bool result3 = find(test_pkb.parent_star_list[2].begin(), test_pkb.parent_star_list[2].end(), 10) !=
        test_pkb.parent_star_list[2].end();

    REQUIRE(result1 == true);
    REQUIRE(result2 == true);
    REQUIRE(result3 == true);
}

TEST_CASE("Update Parent Star") {
    PKB test_pkb = PKB();
    for (int i = -1; i < 10; i++) {
        test_pkb.parent_list.insert({i, {}});
    }
    test_pkb.parent_list.at(-1).push_back(1);
    test_pkb.parent_list.at(1).push_back(2);
    test_pkb.parent_list.at(1).push_back(3);
    test_pkb.parent_list.at(3).push_back(4);
    test_pkb.parent_list.at(-1).push_back(5);
    test_pkb.parent_list.at(5).push_back(6);
    test_pkb.parent_list.at(6).push_back(7);
    test_pkb.parent_list.at(7).push_back(8);
    test_pkb.parent_list.at(-1).push_back(9);

    test_pkb.inverted_parent_list.insert({1,-1});
    test_pkb.inverted_parent_list.insert({2,1});
    test_pkb.inverted_parent_list.insert({3,1});
    test_pkb.inverted_parent_list.insert({4,3});
    test_pkb.inverted_parent_list.insert({5,-1});
    test_pkb.inverted_parent_list.insert({6,5});
    test_pkb.inverted_parent_list.insert({7,6});
test_pkb.inverted_parent_list.insert({ 8,7 });
test_pkb.inverted_parent_list.insert({ 9,-1 });
test_pkb.updateParentStar();


vector<int> result1 = test_pkb.inverted_parent_star_list.at(4);
vector<int> result2 = test_pkb.inverted_parent_star_list.at(7);
vector<int> result3 = test_pkb.inverted_parent_star_list.at(3);


vector<int> answer1 = { 3, 1 };
vector<int> answer2 = { 6, 5 };
vector<int> answer3 = { 1 };

REQUIRE(result1 == answer1);
REQUIRE(result2 == answer2);
REQUIRE(result3 == answer3);
}

TEST_CASE("isParentStar") {
    PKB test_pkb = PKB();

    test_pkb.parent_star_list[2].push_back(3);
    test_pkb.parent_star_list[3].push_back(10);
    test_pkb.parent_star_list[2].push_back(10);
    test_pkb.inverted_parent_star_list[3].push_back(2);
    test_pkb.inverted_parent_star_list[10].push_back(3);
    test_pkb.inverted_parent_star_list[10].push_back(2);

    bool result1 = test_pkb.isParentStar(2, 3);
    bool result2 = test_pkb.isParentStar(3, 11);
    bool result3 = test_pkb.isParentStar(2, 10);

    REQUIRE(result1 == true);
    REQUIRE(result2 == false);
    REQUIRE(result3 == true);
}

TEST_CASE("Insert Uses Statement") {
    PKB test_pkb = PKB();

    test_pkb.insertUses(1, "x");
    test_pkb.insertUses(1, "y");

    bool result1 = find(test_pkb.uses_stmt_list[1].begin(), test_pkb.uses_stmt_list[1].end(), "y") !=
        test_pkb.uses_stmt_list[1].end();
    bool result2 = find(test_pkb.uses_stmt_list[1].begin(), test_pkb.uses_stmt_list[1].end(), "z") !=
        test_pkb.uses_stmt_list[1].end();

    REQUIRE(result1 == true);
    REQUIRE(result2 == false);

    test_pkb.insertUses(1, "3");
    test_pkb.insertUses(1, "4");

    bool result3 = find(test_pkb.uses_stmt_const_list[1].begin(), test_pkb.uses_stmt_const_list[1].end(), "3") !=
        test_pkb.uses_stmt_const_list[1].end();
    bool result4 = find(test_pkb.uses_stmt_const_list[1].begin(), test_pkb.uses_stmt_const_list[1].end(), "5") !=
        test_pkb.uses_stmt_const_list[1].end();

    REQUIRE(result3 == true);
    REQUIRE(result4 == false);
}

TEST_CASE("Insert Uses Procedure") {
    PKB test_pkb = PKB();

    test_pkb.insertUses("P1", "x");
    test_pkb.insertUses("P1", "y");

    bool result1 = find(test_pkb.uses_proc_list["P1"].begin(), test_pkb.uses_proc_list["P1"].end(), "y") !=
        test_pkb.uses_proc_list["P1"].end();
    bool result2 = find(test_pkb.uses_proc_list["P1"].begin(), test_pkb.uses_proc_list["P1"].end(), "z") !=
        test_pkb.uses_proc_list["P1"].end();

    REQUIRE(result1 == true);
    REQUIRE(result2 == false);

    test_pkb.insertUses("P1", "3");
    test_pkb.insertUses("P1", "4");

    bool result3 = find(test_pkb.uses_proc_const_list["P1"].begin(), test_pkb.uses_proc_const_list["P1"].end(), "3") !=
        test_pkb.uses_proc_const_list["P1"].end();
    bool result4 = find(test_pkb.uses_proc_const_list["P1"].begin(), test_pkb.uses_proc_const_list["P1"].end(), "5") !=
        test_pkb.uses_proc_const_list["P1"].end();

    REQUIRE(result3 == true);
    REQUIRE(result4 == false);
}

TEST_CASE("isUses Statement") {
    PKB test_pkb = PKB();

    test_pkb.uses_stmt_list[1].push_back("x");
    test_pkb.uses_stmt_list[1].push_back("y");
    test_pkb.uses_stmt_list[2].push_back("y");

    bool result1 = test_pkb.isUses(1, "y");
    bool result2 = test_pkb.isUses(1, "z");
    bool result3 = test_pkb.isUses(2, "y");

    REQUIRE(result1 == true);
    REQUIRE(result2 == false);
    REQUIRE(result3 == true);

    test_pkb.uses_stmt_const_list[1].push_back("3");
    test_pkb.uses_stmt_const_list[1].push_back("4");
    test_pkb.uses_stmt_const_list[2].push_back("4");

    bool result4 = test_pkb.isUses(1, "4");
    bool result5 = test_pkb.isUses(1, "5");
    bool result6 = test_pkb.isUses(2, "4");

    REQUIRE(result4 == true);
    REQUIRE(result5 == false);
    REQUIRE(result6 == true);
}

TEST_CASE("isUses Procedure") {
    PKB test_pkb = PKB();

    test_pkb.uses_proc_list["P1"].push_back("x");
    test_pkb.uses_proc_list["P1"].push_back("y");
    test_pkb.uses_proc_list["P2"].push_back("y");

    bool result1 = test_pkb.isUses("P1", "y");
    bool result2 = test_pkb.isUses("P1", "z");
    bool result3 = test_pkb.isUses("P2", "y");

    REQUIRE(result1 == true);
    REQUIRE(result2 == false);
    REQUIRE(result3 == true);

    test_pkb.uses_proc_const_list["P1"].push_back("3");
    test_pkb.uses_proc_const_list["P1"].push_back("4");
    test_pkb.uses_proc_const_list["P2"].push_back("4");

    bool result4 = test_pkb.isUses("P1", "4");
    bool result5 = test_pkb.isUses("P1", "5");
    bool result6 = test_pkb.isUses("P2", "4");

    REQUIRE(result4 == true);
    REQUIRE(result5 == false);
    REQUIRE(result6 == true);
}

TEST_CASE("Get All Variables Used By Statement") {
    PKB test_pkb = PKB();

    test_pkb.uses_stmt_list[1].push_back("x");
    test_pkb.uses_stmt_list[1].push_back("y");
    test_pkb.uses_stmt_list[2].push_back("y");

    vector<string> result1 = test_pkb.getAllVariablesUsedBy(1);
    vector<string> result2 = test_pkb.getAllVariablesUsedBy(2);
    vector<string> result3 = test_pkb.getAllVariablesUsedBy(3);

    vector<string> answer1{ "x","y" };
    vector<string> answer2{ "y" };
    vector<string> answer3 = vector<string>();

    REQUIRE(result1 == answer1);
    REQUIRE(result2 == answer2);
    REQUIRE(result3 == answer3);
}

TEST_CASE("Get All Variables Used By Procedure") {
    PKB test_pkb = PKB();

    test_pkb.uses_proc_list["P1"].push_back("x");
    test_pkb.uses_proc_list["P1"].push_back("y");
    test_pkb.uses_proc_list["P2"].push_back("y");

    vector<string> result1 = test_pkb.getAllVariablesUsedBy("P1");
    vector<string> result2 = test_pkb.getAllVariablesUsedBy("P2");
    vector<string> result3 = test_pkb.getAllVariablesUsedBy("P3");

    vector<string> answer1{ "x","y" };
    vector<string> answer2{ "y" };
    vector<string> answer3 = vector<string>();

    REQUIRE(result1 == answer1);
    REQUIRE(result2 == answer2);
    REQUIRE(result3 == answer3);
}

TEST_CASE("Get All Constants Used By Statement") {
    PKB test_pkb = PKB();

    test_pkb.uses_stmt_const_list[1].push_back("3");
    test_pkb.uses_stmt_const_list[1].push_back("4");
    test_pkb.uses_stmt_const_list[2].push_back("4");

    vector<string> result1 = test_pkb.getAllConstantsUsedBy(1);
    vector<string> result2 = test_pkb.getAllConstantsUsedBy(2);
    vector<string> result3 = test_pkb.getAllConstantsUsedBy(3);

    vector<string> answer1{ "3", "4" };
    vector<string> answer2{ "4" };
    vector<string> answer3 = vector<string>();

    REQUIRE(result1 == answer1);
    REQUIRE(result2 == answer2);
    REQUIRE(result3 == answer3);
}

TEST_CASE("Get All Constants Used By Procedure") {
    PKB test_pkb = PKB();

    test_pkb.uses_proc_const_list["P1"].push_back("3");
    test_pkb.uses_proc_const_list["P1"].push_back("4");
    test_pkb.uses_proc_const_list["P2"].push_back("4");

    vector<string> result1 = test_pkb.getAllConstantsUsedBy("P1");
    vector<string> result2 = test_pkb.getAllConstantsUsedBy("P2");
    vector<string> result3 = test_pkb.getAllConstantsUsedBy("P3");

    vector<string> answer1{ "3","4" };
    vector<string> answer2{ "4" };
    vector<string> answer3 = vector<string>();

    REQUIRE(result1 == answer1);
    REQUIRE(result2 == answer2);
    REQUIRE(result3 == answer3);
}

TEST_CASE("UsesVar Statement") {
    PKB test_pkb = PKB();

    test_pkb.uses_stmt_list[1].push_back("x");
    test_pkb.uses_stmt_list[1].push_back("y");
    test_pkb.uses_stmt_list[2].push_back("y");

    bool result1 = test_pkb.usesVar(1);
    bool result2 = test_pkb.usesVar(2);
    bool result3 = test_pkb.usesVar(3);

    REQUIRE(result1 == true);
    REQUIRE(result2 == true);
    REQUIRE(result3 == false);
}

TEST_CASE("UsesVar Procedure") {
    PKB test_pkb = PKB();

    test_pkb.uses_proc_list["P1"].push_back("x");
    test_pkb.uses_proc_list["P1"].push_back("y");
    test_pkb.uses_proc_list["P2"].push_back("y");

    bool result1 = test_pkb.usesVar("P1");
    bool result2 = test_pkb.usesVar("P2");
    bool result3 = test_pkb.usesVar("P3");

    REQUIRE(result1 == true);
    REQUIRE(result2 == true);
    REQUIRE(result3 == false);
}

TEST_CASE("UsesConst Statement") {
    PKB test_pkb = PKB();

    test_pkb.uses_stmt_const_list[1].push_back("3");
    test_pkb.uses_stmt_const_list[1].push_back("4");
    test_pkb.uses_stmt_const_list[2].push_back("4");

    bool result1 = test_pkb.usesConst(1);
    bool result2 = test_pkb.usesConst(2);
    bool result3 = test_pkb.usesConst(3);

    REQUIRE(result1 == true);
    REQUIRE(result2 == true);
    REQUIRE(result3 == false);
}

TEST_CASE("UsesConst Procedure") {
    PKB test_pkb = PKB();

    test_pkb.uses_proc_const_list["P1"].push_back("3");
    test_pkb.uses_proc_const_list["P1"].push_back("4");
    test_pkb.uses_proc_const_list["P2"].push_back("4");

    bool result1 = test_pkb.usesConst("P1");
    bool result2 = test_pkb.usesConst("P2");
    bool result3 = test_pkb.usesConst("P3");

    REQUIRE(result1 == true);
    REQUIRE(result2 == true);
    REQUIRE(result3 == false);
}

TEST_CASE("isUsed") {
    PKB test_pkb = PKB();

    test_pkb.uses_stmt_list[1].push_back("x");
    test_pkb.uses_stmt_list[1].push_back("y");
    test_pkb.uses_stmt_list[2].push_back("y");

    test_pkb.inverted_uses_stmt_list["x"].push_back(1);
    test_pkb.inverted_uses_stmt_list["y"].push_back(1);
    test_pkb.inverted_uses_stmt_list["y"].push_back(2);

    bool result1 = test_pkb.isUsed("x");
    bool result2 = test_pkb.isUsed("y");
    bool result3 = test_pkb.isUsed("z");

    REQUIRE(result1 == true);
    REQUIRE(result2 == true);
    REQUIRE(result3 == false);

    test_pkb.uses_stmt_const_list[1].push_back("3");
    test_pkb.uses_stmt_const_list[1].push_back("4");
    test_pkb.uses_stmt_const_list[2].push_back("4");

    test_pkb.inverted_uses_stmt_const_list["3"].push_back(1);
    test_pkb.inverted_uses_stmt_const_list["4"].push_back(1);
    test_pkb.inverted_uses_stmt_const_list["4"].push_back(2);

    bool result4 = test_pkb.isUsed("3");
    bool result5 = test_pkb.isUsed("4");
    bool result6 = test_pkb.isUsed("5");

    REQUIRE(result4 == true);
    REQUIRE(result5 == true);
    REQUIRE(result6 == false);
}

TEST_CASE("Get All Used Variables") {
    PKB test_pkb = PKB();

    test_pkb.uses_proc_list["P1"].push_back("x");
    test_pkb.uses_proc_list["P1"].push_back("y");
    test_pkb.uses_proc_list["P2"].push_back("y");

    test_pkb.inverted_uses_proc_list["x"].push_back("P1");
    test_pkb.inverted_uses_proc_list["y"].push_back("P1");
    test_pkb.inverted_uses_proc_list["y"].push_back("P2");

    vector<string> result1 = test_pkb.getAllUsedVar();
    result1 = sorted(result1);

    vector<string> answer1{ "x", "y" };
    answer1 = sorted(answer1);

    REQUIRE(result1 == answer1);
}

TEST_CASE("Get All Used Constants") {
    PKB test_pkb = PKB();

    test_pkb.uses_proc_const_list["P1"].push_back("3");
    test_pkb.uses_proc_const_list["P1"].push_back("4");
    test_pkb.uses_proc_const_list["P2"].push_back("4");

    test_pkb.inverted_uses_proc_const_list["3"].push_back("P1");
    test_pkb.inverted_uses_proc_const_list["4"].push_back("P1");
    test_pkb.inverted_uses_proc_const_list["4"].push_back("P2");

    vector<string> result1 = test_pkb.getAllUsedConst();
    result1 = sorted(result1);

    vector<string> answer1{ "3","4" };
    answer1 = sorted(answer1);

    REQUIRE(result1 == answer1);
}

TEST_CASE("Add Modifies Relationship") {
    PKB test_pkb = PKB();
    shared_ptr<ProgramNode> root(new ProgramNode());
    std::vector<std::shared_ptr<ProcedureNode>> root_list= {};
    shared_ptr<ProcedureNode> procedure(new ProcedureNode("rootProc"));
    shared_ptr<ProcedureNode> procedure2(new ProcedureNode("proc2"));
    root_list.push_back(procedure);
    root_list.push_back(procedure2);
    root->addProcedureList(root_list);

    test_pkb.all_procedures.emplace_back("rootProc");
    test_pkb.all_procedures.emplace_back("proc2");


    std::vector<std::shared_ptr<StmtNode>> procList= {};
    shared_ptr<ReadNode> s1 = make_shared<ReadNode>(ReadNode(procedure, make_shared<VariableNode>(VariableNode("A"))));
    shared_ptr<ReadNode> s2 = make_shared<ReadNode>(ReadNode(procedure, make_shared<VariableNode>(VariableNode("B"))));
    shared_ptr<ReadNode> s3 = make_shared<ReadNode>(ReadNode(procedure, make_shared<VariableNode>(VariableNode("E"))));
    shared_ptr<ReadNode> s4 = make_shared<ReadNode>(ReadNode(procedure, make_shared<VariableNode>(VariableNode("D"))));
    shared_ptr<ReadNode> s5 = make_shared<ReadNode>(ReadNode(procedure2, make_shared<VariableNode>(VariableNode("E"))));

    for (int i = -2; i < 10; i++) {
        test_pkb.parent_list.insert({i, {}});
    }
    test_pkb.parent_list.at(-1).push_back(1);
    test_pkb.parent_list.at(1).push_back(2);
    test_pkb.parent_list.at(1).push_back(3);
    test_pkb.parent_list.at(3).push_back(4);
    test_pkb.parent_list.at(-2).push_back(5);


    test_pkb.inverted_parent_list.insert({1, -1});
    test_pkb.inverted_parent_list.insert({2, 1});
    test_pkb.inverted_parent_list.insert({3, 1});
    test_pkb.inverted_parent_list.insert({4, 3});
    test_pkb.inverted_parent_list.insert({5, -2});


    test_pkb.addModifiesRelationship(s1, 1);
    test_pkb.addModifiesRelationship(s2, 2);
    test_pkb.addModifiesRelationship(s3, 3);
    test_pkb.addModifiesRelationship(s4, 4);
    test_pkb.addModifiesRelationship(s5, 5);


    test_pkb.all_statements.push_back(1);
    test_pkb.all_statements.push_back(2);
    test_pkb.all_statements.push_back(3);
    test_pkb.all_statements.push_back(4);
    test_pkb.all_statements.push_back(5);

    REQUIRE(test_pkb.isModifies(1, "A"));
    REQUIRE(test_pkb.isModifies(2, "B"));
    REQUIRE(test_pkb.isModifies(3, "E"));
    REQUIRE(test_pkb.isModifies(4, "D"));
    REQUIRE(test_pkb.isModifies(5, "E"));
    REQUIRE(!test_pkb.isModifies(3, "D"));

    test_pkb.updateModifiesRelationship();

    REQUIRE(test_pkb.isModifies(1, "B"));
    REQUIRE(test_pkb.isModifies(3, "E"));
    REQUIRE(test_pkb.isModifies("rootProc", "A"));
    REQUIRE(test_pkb.isModifies("rootProc", "B"));
    REQUIRE(test_pkb.isModifies("rootProc", "E"));
    REQUIRE(test_pkb.isModifies("rootProc", "D"));
    REQUIRE(test_pkb.isModifies("proc2", "E"));


    vector<int> result1 = test_pkb.inverted_modifies_stmt_list.at("D");
    vector<string> result2 = test_pkb.inverted_modifies_proc_list.at("D");
    vector<string> result3 = test_pkb.inverted_modifies_proc_list.at("E");

    vector<int> answer1({4, 3, 1});
    vector<string> answer2({"rootProc"});
    vector<string> answer3({"proc2", "rootProc"});
    REQUIRE(result1 == answer1);
    REQUIRE(result2 == answer2);
    REQUIRE(result3 == answer3);
}

TEST_CASE("Insert Modifies Statement") {
    PKB test_pkb = PKB();

    test_pkb.insertModifies(1, "x");
    test_pkb.insertModifies(1, "y");

    bool result1 = find(test_pkb.modifies_stmt_list[1].begin(), test_pkb.modifies_stmt_list[1].end(), "y") !=
        test_pkb.modifies_stmt_list[1].end();
    bool result2 = find(test_pkb.modifies_stmt_list[1].begin(), test_pkb.modifies_stmt_list[1].end(), "z") !=
        test_pkb.modifies_stmt_list[1].end();

    REQUIRE(result1 == true);
    REQUIRE(result2 == false);
}

TEST_CASE("Insert Modifies Procedure") {
    PKB test_pkb = PKB();

    test_pkb.insertModifies("P1", "x");
    test_pkb.insertModifies("P1", "y");

    bool result1 = find(test_pkb.modifies_proc_list["P1"].begin(), test_pkb.modifies_proc_list["P1"].end(), "y") !=
        test_pkb.modifies_proc_list["P1"].end();
    bool result2 = find(test_pkb.modifies_proc_list["P1"].begin(), test_pkb.modifies_proc_list["P1"].end(), "z") !=
        test_pkb.modifies_proc_list["P1"].end();

    REQUIRE(result1 == true);
    REQUIRE(result2 == false);
}

TEST_CASE("isModifies Statement") {
    PKB test_pkb = PKB();

    test_pkb.modifies_stmt_list[1].push_back("x");
    test_pkb.modifies_stmt_list[1].push_back("y");
    test_pkb.modifies_stmt_list[2].push_back("y");

    bool result1 = test_pkb.isModifies(1, "y");
    bool result2 = test_pkb.isModifies(1, "z");
    bool result3 = test_pkb.isModifies(2, "y");

    REQUIRE(result1 == true);
    REQUIRE(result2 == false);
    REQUIRE(result3 == true);
}

TEST_CASE("isModifies Procedure") {
    PKB test_pkb = PKB();

    test_pkb.modifies_proc_list["P1"].push_back("x");
    test_pkb.modifies_proc_list["P1"].push_back("y");
    test_pkb.modifies_proc_list["P2"].push_back("y");

    bool result1 = test_pkb.isModifies("P1", "y");
    bool result2 = test_pkb.isModifies("P1", "z");
    bool result3 = test_pkb.isModifies("P2", "y");

    REQUIRE(result1 == true);
    REQUIRE(result2 == false);
    REQUIRE(result3 == true);
}

TEST_CASE("Get All Variables Modified By Statement") {
    PKB test_pkb = PKB();

    test_pkb.modifies_stmt_list[1].push_back("x");
    test_pkb.modifies_stmt_list[1].push_back("y");
    test_pkb.modifies_stmt_list[2].push_back("y");

    vector<string> result1 = test_pkb.getAllVariablesModifiedBy(1);
    vector<string> result2 = test_pkb.getAllVariablesModifiedBy(2);
    vector<string> result3 = test_pkb.getAllVariablesModifiedBy(3);

    vector<string> answer1{ "x","y" };
    vector<string> answer2{ "y" };
    vector<string> answer3 = vector<string>();

    REQUIRE(result1 == answer1);
    REQUIRE(result2 == answer2);
    REQUIRE(result3 == answer3);
}

TEST_CASE("Get All Variables Modified By Procedure") {
    PKB test_pkb = PKB();

    test_pkb.modifies_proc_list["P1"].push_back("x");
    test_pkb.modifies_proc_list["P1"].push_back("y");
    test_pkb.modifies_proc_list["P2"].push_back("y");

    vector<string> result1 = test_pkb.getAllVariablesModifiedBy("P1");
    vector<string> result2 = test_pkb.getAllVariablesModifiedBy("P2");
    vector<string> result3 = test_pkb.getAllVariablesModifiedBy("P3");

    vector<string> answer1{ "x","y" };
    vector<string> answer2{ "y" };
    vector<string> answer3 = vector<string>();

    REQUIRE(result1 == answer1);
    REQUIRE(result2 == answer2);
    REQUIRE(result3 == answer3);
}

TEST_CASE("ModifiesVar Statement") {
    PKB test_pkb = PKB();

    test_pkb.modifies_stmt_list[1].push_back("x");
    test_pkb.modifies_stmt_list[1].push_back("y");
    test_pkb.modifies_stmt_list[2].push_back("y");

    bool result1 = test_pkb.modifiesVar(1);
    bool result2 = test_pkb.modifiesVar(2);
    bool result3 = test_pkb.modifiesVar(3);

    REQUIRE(result1 == true);
    REQUIRE(result2 == true);
    REQUIRE(result3 == false);
}

TEST_CASE("ModifiesVar Procedure") {
    PKB test_pkb = PKB();

    test_pkb.modifies_proc_list["P1"].push_back("x");
    test_pkb.modifies_proc_list["P1"].push_back("y");
    test_pkb.modifies_proc_list["P2"].push_back("y");

    bool result1 = test_pkb.modifiesVar("P1");
    bool result2 = test_pkb.modifiesVar("P2");
    bool result3 = test_pkb.modifiesVar("P3");

    REQUIRE(result1 == true);
    REQUIRE(result2 == true);
    REQUIRE(result3 == false);
}

TEST_CASE("isModified") {
    PKB test_pkb = PKB();

    test_pkb.modifies_stmt_list[1].push_back("x");
    test_pkb.modifies_stmt_list[1].push_back("y");
    test_pkb.modifies_stmt_list[2].push_back("y");

    test_pkb.inverted_modifies_stmt_list["x"].push_back(1);
    test_pkb.inverted_modifies_stmt_list["y"].push_back(1);
    test_pkb.inverted_modifies_stmt_list["y"].push_back(2);

    bool result1 = test_pkb.isModified("x");
    bool result2 = test_pkb.isModified("y");
    bool result3 = test_pkb.isModified("z");

    REQUIRE(result1 == true);
    REQUIRE(result2 == true);
    REQUIRE(result3 == false);
}

TEST_CASE("Get All Modified") {
    PKB test_pkb = PKB();

    test_pkb.modifies_proc_list["P1"].push_back("x");
    test_pkb.modifies_proc_list["P1"].push_back("y");
    test_pkb.modifies_proc_list["P2"].push_back("y");

    test_pkb.inverted_modifies_proc_list["x"].push_back("P1");
    test_pkb.inverted_modifies_proc_list["y"].push_back("P1");
    test_pkb.inverted_modifies_proc_list["y"].push_back("P2");

    vector<string> result1 = test_pkb.getAllModified();

    result1 = sorted(result1);
    vector<string> answer1{ "x", "y" };
    answer1 = sorted(answer1);


    REQUIRE(result1 == answer1);
}


TEST_CASE("Insert Variable") {
    PKB test_pkb = PKB();
    shared_ptr<TNode> v1 = make_shared<TNode>(VariableNode("a"));
    shared_ptr<TNode> v2 = make_shared<TNode>(VariableNode("a"));
    shared_ptr<TNode> v3 = make_shared<TNode>(VariableNode("b"));
    shared_ptr<TNode> v4 = make_shared<TNode>(VariableNode("c"));

    test_pkb.insertVariable(v1);
    test_pkb.insertVariable(v2);
    test_pkb.insertVariable(v3);
    test_pkb.insertVariable(v4);

    REQUIRE(test_pkb.getVar("a") != v2);
    REQUIRE(test_pkb.getVar("b") != v4);

}

TEST_CASE("Add Uses Relationship") {
    PKB test_pkb = PKB();
    shared_ptr<ProgramNode> root(new ProgramNode());
    std::vector<std::shared_ptr<ProcedureNode>> root_list= {};
    shared_ptr<ProcedureNode> procedure(new ProcedureNode("rootProc"));
    shared_ptr<ProcedureNode> procedure2(new ProcedureNode("proc2"));
    root_list.push_back(procedure);
    root_list.push_back(procedure2);
    root->addProcedureList(root_list);

    test_pkb.all_procedures.emplace_back("rootProc");
    test_pkb.all_procedures.emplace_back("proc2");


    std::vector<std::shared_ptr<StmtNode>> procList= {};
    std::vector<std::shared_ptr<StmtNode>> procList2= {};

    //refactor later too lazy to write separate function
    shared_ptr<VariableNode> left = make_shared<VariableNode>(VariableNode("A"));
    shared_ptr<VariableNode> right = make_shared<VariableNode>(VariableNode("B"));
    shared_ptr<RelExpressionNode> cond = make_shared<RelExpressionNode>(RelExpressionNode("==", left, right));
    shared_ptr<IfNode> s1 = make_shared<IfNode>(IfNode(procedure, cond));

    shared_ptr<VariableNode> left2 = make_shared<VariableNode>(VariableNode("C"));
    shared_ptr<VariableNode> right2 = make_shared<VariableNode>(VariableNode("D"));
    shared_ptr<RelExpressionNode> cond2 = make_shared<RelExpressionNode>(RelExpressionNode("==", left2, right2));
    shared_ptr<IfNode> s2 = make_shared<IfNode>(IfNode(s1, cond2));

    shared_ptr<VariableNode> left3 = make_shared<VariableNode>(VariableNode("E"));
    shared_ptr<VariableNode> right3 = make_shared<VariableNode>(VariableNode("F"));
    shared_ptr<RelExpressionNode> cond3 = make_shared<RelExpressionNode>(RelExpressionNode("==", left3, right3));
    shared_ptr<IfNode> s3 = make_shared<IfNode>(IfNode(s1, cond3));

    shared_ptr<ConstantNode> left4 = make_shared<ConstantNode>(ConstantNode("1"));
    shared_ptr<ConstantNode> right4 = make_shared<ConstantNode>(ConstantNode("2"));
    shared_ptr<RelExpressionNode> cond4 = make_shared<RelExpressionNode>(RelExpressionNode("==", left4, right4));
    shared_ptr<IfNode> s4 = make_shared<IfNode>(IfNode(s3, cond4));

    shared_ptr<VariableNode> left5 = make_shared<VariableNode>(VariableNode("I"));
    shared_ptr<VariableNode> right5 = make_shared<VariableNode>(VariableNode("J"));
    shared_ptr<RelExpressionNode> cond5 = make_shared<RelExpressionNode>(RelExpressionNode("==", left5, right5));
    shared_ptr<IfNode> s5 = make_shared<IfNode>(IfNode(procedure2, cond5));


    procList.push_back(s1);
    procedure->addStatementList(procList);

    std::vector<std::shared_ptr<StmtNode>> if_list= {};
    std::vector<std::shared_ptr<StmtNode>> elseList= {};

    if_list.push_back(s2);
    elseList.push_back(s3);
    s1->addStatementList(if_list);
    s1->addElseStatementList(elseList);

    std::vector<std::shared_ptr<StmtNode>> ifList2= {};
    std::vector<std::shared_ptr<StmtNode>> elseList2= {};

    ifList2.push_back(s4);
    s3->addStatementList(ifList2);
    s3->addElseStatementList(elseList2);

    procList2.push_back(s5);
    procedure2->addStatementList(procList2);


    for (int i = -2; i < 10; i++) {
        test_pkb.parent_list.insert({i, {}});
    }
    test_pkb.parent_list.at(-1).push_back(1);
    test_pkb.parent_list.at(1).push_back(2);
    test_pkb.parent_list.at(1).push_back(3);
    test_pkb.parent_list.at(3).push_back(4);
    test_pkb.parent_list.at(-2).push_back(5);


    test_pkb.inverted_parent_list.insert({1, -1});
    test_pkb.inverted_parent_list.insert({2, 1});
    test_pkb.inverted_parent_list.insert({3, 1});
    test_pkb.inverted_parent_list.insert({4, 3});
    test_pkb.inverted_parent_list.insert({5, -2});

    test_pkb.all_statements.push_back(1);
    test_pkb.all_statements.push_back(2);
    test_pkb.all_statements.push_back(3);
    test_pkb.all_statements.push_back(4);
    test_pkb.all_statements.push_back(5);

    test_pkb.addUsesRelationship(s1, 1);
    test_pkb.addUsesRelationship(s2, 2);
    test_pkb.addUsesRelationship(s3, 3);
    test_pkb.addUsesRelationship(s4, 4);
    test_pkb.addUsesRelationship(s5, 5);

    REQUIRE(test_pkb.isUses(1, "B"));
    REQUIRE(test_pkb.isUses(2, "C"));
    REQUIRE(test_pkb.isUses(2, "D"));
    REQUIRE(test_pkb.isUses(3, "E"));
    REQUIRE(test_pkb.isUses(4, "1"));
    REQUIRE(test_pkb.isUses(5, "I"));


    test_pkb.updateUsesRelationship();

    REQUIRE(test_pkb.isUses(1, "C"));
    REQUIRE(test_pkb.isUses(3, "1"));
    REQUIRE(test_pkb.isUses(1, "1"));
    REQUIRE(test_pkb.isUses(1, "D"));
    REQUIRE(test_pkb.isUses(1, "C"));
    REQUIRE(test_pkb.isUses("rootProc", "C"));
    REQUIRE(test_pkb.isUses("rootProc", "1"));
    REQUIRE(test_pkb.isUses("proc2", "I"));

}


TEST_CASE("Get Follows") {
    PKB test_pkb = PKB();

    test_pkb.follows_list.insert({ 1,2 });

    int result1 = test_pkb.getFollows(1);
    int result2 = test_pkb.getFollows(3);

    int answer1 = 2;
    int answer2 = 0;

    REQUIRE(result1 == answer1);
    REQUIRE(result2 == answer2);
}

TEST_CASE("Get Followed By") {
    PKB test_pkb = PKB();

    test_pkb.inverted_follows_list.insert({ 2,1 });

    int result1 = test_pkb.getFollowedBy(2);
    int result2 = test_pkb.getFollowedBy(3);

    int answer1 = 1;
    int answer2 = 0;

    REQUIRE(result1 == answer1);
    REQUIRE(result2 == answer2);
}

TEST_CASE("Get Follows Star") {
    PKB test_pkb = PKB();

    test_pkb.follows_star_list[1].push_back(2);
    test_pkb.follows_star_list[1].push_back(3);
    test_pkb.follows_star_list[2].push_back(3);

    vector<int> result1 = test_pkb.getFollowsStar(1);
    vector<int> result2 = test_pkb.getFollowsStar(4);
    
    vector<int> answer1 = { 2,3 };
    vector<int> answer2 = {};

    REQUIRE(result1 == answer1);
    REQUIRE(result2 == answer2);
}

TEST_CASE("Get Followed By Star") {
    PKB test_pkb = PKB();

    test_pkb.inverted_follows_star_list[2].push_back(1);
    test_pkb.inverted_follows_star_list[3].push_back(1);
    test_pkb.inverted_follows_star_list[3].push_back(2);

    vector<int> result1 = test_pkb.getFollowedByStar(3);
    vector<int> result2 = test_pkb.getFollowedByStar(4);

    vector<int> answer1 = { 1,2 };
    vector<int> answer2 = {};

    REQUIRE(result1 == answer1);
    REQUIRE(result2 == answer2);
}

TEST_CASE("Get Parent") {
    PKB test_pkb = PKB();

    test_pkb.inverted_parent_list.insert({ 2,1 });

    int result1 = test_pkb.getParent(2);
    int result2 = test_pkb.getParent(3);

    int answer1 = 1;
    int answer2 = 0;

    REQUIRE(result1 == answer1);
    REQUIRE(result2 == answer2);
}

TEST_CASE("Get Children") {
    PKB test_pkb = PKB();

    test_pkb.parent_list[1].push_back(2);
    test_pkb.parent_list[1].push_back(3);

    vector<int> result1 = test_pkb.getChildren(1);
    vector<int> result2 = test_pkb.getChildren(2);

    vector<int> answer1 = { 2,3 };
    vector<int> answer2 = {};

    REQUIRE(result1 == answer1);
    REQUIRE(result2 == answer2);
}

TEST_CASE("Get Parent Star") {
    PKB test_pkb = PKB();

    test_pkb.inverted_parent_star_list[2].push_back(1);
    test_pkb.inverted_parent_star_list[3].push_back(1);
    test_pkb.inverted_parent_star_list[3].push_back(2);

    vector<int> result1 = test_pkb.getParentStar(3);
    vector<int> result2 = test_pkb.getParentStar(4);
    
    vector<int> answer1 = { 1,2 };
    vector<int> answer2 = {};

    REQUIRE(result1 == answer1);
    REQUIRE(result2 == answer2);
}

TEST_CASE("Get Children Star") {
    PKB test_pkb = PKB();

    test_pkb.parent_star_list[1].push_back(2);
    test_pkb.parent_star_list[1].push_back(3);

    vector<int> result1 = test_pkb.getChildrenStar(1);
    vector<int> result2 = test_pkb.getChildrenStar(2);

    vector<int> answer1 = { 2,3 };
    vector<int> answer2 = {};

    REQUIRE(result1 == answer1);
    REQUIRE(result2 == answer2);
}

TEST_CASE("Get Statement Type") {
    PKB test_pkb = PKB();

    test_pkb.stmt_type_list.insert({ 1, DesignEntity::While });

    DesignEntity result1 = test_pkb.getStmtType(1);

    DesignEntity answer1 = DesignEntity::While;

    REQUIRE(result1 == answer1);
}

TEST_CASE("Get All Matched Entity") {
    PKB test_pkb = PKB();

    test_pkb.if_list.push_back(1);
    test_pkb.if_list.push_back(4);

    vector<string> result1 = test_pkb.getAllMatchedEntity(DesignEntity::If);
    vector<string> result2 = test_pkb.getAllMatchedEntity(DesignEntity::Assign);

    vector<string> answer1 = { "1", "4" };
    vector<string> answer2 = { };

    REQUIRE(result1 == answer1);
    REQUIRE(result2 == answer2);
}

TEST_CASE("Insert Next") {
    PKB test_pkb = PKB();

    test_pkb.insertNext(1, 2);
    test_pkb.insertNext(2, 3);
    test_pkb.insertNext(5, 3);
    test_pkb.insertNext(1, 4);
    vector<int> ans;
    ans.push_back(2);
    ans.push_back(4);

    vector<int> ans2;
    ans2.push_back(2);
    ans2.push_back(5);

    REQUIRE(sorted(test_pkb.next_list[1]) == sorted(ans));
    REQUIRE(sorted(test_pkb.inverted_next_list[3]) == sorted(ans2));

    REQUIRE(test_pkb.isNextStatement(1, 2));
    REQUIRE(test_pkb.isNextStatement(2, 3));
    REQUIRE(test_pkb.isNextStatement(5, 3));

    REQUIRE(sorted(test_pkb.getNextStatements(1)) == sorted(ans));
    REQUIRE(sorted(test_pkb.getPreviousStatements(3)) == sorted(ans2));
    REQUIRE(test_pkb.getNextStatements(15).empty());


}


TEST_CASE("Insert NextStar") {
    PKB test_pkb = PKB();

    test_pkb.insertNext(1, 2);
    test_pkb.insertNext(2, 3);
    test_pkb.insertNext(5, 3);
    test_pkb.insertNext(1, 4);
    vector<int> ans;
    ans.push_back(2);
    ans.push_back(4);
    ans.push_back(3);

    vector<int> ans2;
    ans2.push_back(1);
    ans2.push_back(2);
    ans2.push_back(5);

    REQUIRE(test_pkb.isNextStar(1, 2));
    REQUIRE(test_pkb.isNextStar(2, 3));
    REQUIRE(test_pkb.isNextStar(1, 3));

    REQUIRE(sorted(test_pkb.getNextStar(1)) == sorted(ans));
    test_pkb.getPreviousStar(3);
    REQUIRE(sorted(test_pkb.getPreviousStar(3)) == sorted(ans2));
    REQUIRE(test_pkb.getNextStar(15).empty());

}

TEST_CASE("Calls") {
    string src = "procedure p1 {"
                 "call p2;"
                 "}"
                 "procedure p2 {"
                 "call p3;"
                 "}"
                 "procedure p3 {"
                 "call p4;"
                 "call p5;"
                 "}"
                 "procedure p4 {"
                 "call p5;"
                 "}"
                 "procedure p5 {"
                 "a = a;"
                 "}";

    Tokenizer *tokenizer = Tokenizer::getInstance();
    vector<Token> tokenStream = tokenizer->tokenizeSource(src);
    Parser* parser = Parser::getInstance();
    AST root = parser->buildAST(tokenStream);
    PKB *pkb = PKB::getInstance();
    pkb->reset();
    pkb->initAST(root);

    REQUIRE(pkb->getCalledBy("p5") == vector<string>({"p3", "p4"}));

}

TEST_CASE("Calls Star") {
    string src = "procedure p1 {"
                 "call p2;"
                 "}"
                 "procedure p2 {"
                 "call p3;"
                 "}"
                 "procedure p3 {"
                 "call p4;"
                 "call p5;"
                 "}"
                 "procedure p4 {"
                 "call p5;"
                 "}"
                 "procedure p5 {"
                 "a = a;"
                 "}";

    Tokenizer *tokenizer = Tokenizer::getInstance();
    vector<Token> tokenStream = tokenizer->tokenizeSource(src);
    Parser* parser = Parser::getInstance();
    AST root = parser->buildAST(tokenStream);
    PKB *pkb = PKB::getInstance();
    pkb->reset();
    pkb->initAST(root);

    REQUIRE(!pkb->isCalls("p1", "p5"));
    REQUIRE(pkb->isCallsStar("p1", "p5"));
}

TEST_CASE("Next") {
    string src = "procedure Example {"
                 "if (x == x) then { " // 1
                 "while (x == x) {"//2
                 " a = a;"//3
                 "}"
                 "}"
                 "else {"
                 "a = b;" //4
                 "b = c;" //5
                 "}"
                 "b = x;"//6
                 "if (x == x) then { " // 7
                 "while (x == x) {"//8
                 " a = a;"//9
                 "}"
                 "a = c;"//10
                 "}"
                 "else {"
                 "a = b;" //11
                 "b = c;" //12
                 "}"
                 "b = x;"//13
                 "}";


    Tokenizer *tokenizer = Tokenizer::getInstance();
    vector<Token> tokenStream = tokenizer->tokenizeSource(src);
    Parser* parser = Parser::getInstance();
    AST root = parser->buildAST(tokenStream);
    PKB *pkb = PKB::getInstance();
    pkb->reset();
    pkb->initAST(root);

    pkb->updateNext();
    REQUIRE(pkb->isNextStatement(1, 2));
    REQUIRE(pkb->isNextStatement(1, 4));
    REQUIRE(pkb->isNextStatement(2, 3));
    REQUIRE(pkb->isNextStatement(4, 5));
    REQUIRE(pkb->isNextStatement(5, 6));
    REQUIRE(pkb->isNextStatement(2, 6));
    REQUIRE(pkb->isNextStatement(8, 10));
    REQUIRE(pkb->isNextStatement(12, 13));


}

TEST_CASE("Next Star") {
    string src = "procedure Example {\n"
                 "while (x == x) { "
                 "x = x; \n"
                 "y = y;"
                 "z = z;"
                 "while (z == z) {"
                 "z = s;"
                 "}"
                 "}"
                 "a= a;"
                 "if (x == x) then {" //8
                 "a = a;"
                 "b = b;"//10
                 "} else {"
                 "b = b;"
                 "c = c;"//12
                 "}"
                 "d = d;" // 13
                 "if (x == x) then {" //14
                 "if (y == y) then {" //15
                 "y = y;" //16
                 "} else {"
                 "y = y;}" //17
                 "} else {"
                 "if (z == z) then {" //18
                 "z = z;" //19
                 "} else {"
                 "z = z;}" //20
                 "}"
                 ""
                 "a = a;" //21
                 "}";

    Tokenizer *tokenizer = Tokenizer::getInstance();
    vector<Token> tokenStream = tokenizer->tokenizeSource(src);
    Parser* parser = Parser::getInstance();
    AST root = parser->buildAST(tokenStream);
    PKB *pkb = PKB::getInstance();
    pkb->reset();
    pkb->initAST(root);

    REQUIRE(pkb->isNextStar(10 , 21));
    REQUIRE(pkb->isNextStar(2 , 21));
    REQUIRE(pkb->isNextStar(14 , 21));
    REQUIRE(pkb->isNextStar(7 , 21));
    REQUIRE(pkb->isNextStar(1, 1));
}

TEST_CASE("with clause API test") {
    string src = "procedure p1 {"
                 "call p2;"//1
                 "read x1;"//2
                 "}"
                 "procedure p2 {"
                 "call p3;"//3
                 "read x2;"//4
                 "}"
                 "procedure p3 {"
                 "call p4;"//5
                 "call p5;"//6
                 "print x1;"//7
                 "}"
                 "procedure p4 {"
                 "call p5;"//8
                 "print x2;"//9
                 "}"
                 "procedure p5 {"
                 "a = a;"//10
                 "}";

    Tokenizer *tokenizer = Tokenizer::getInstance();
    vector<Token> tokenStream = tokenizer->tokenizeSource(src);
    Parser* parser = Parser::getInstance();
    AST root = parser->buildAST(tokenStream);
    PKB *pkb = PKB::getInstance();
    pkb->reset();
    pkb->initAST(root);

    REQUIRE(pkb->isProcedure("p1"));
    REQUIRE(pkb->isProcedure("p2"));
    REQUIRE(pkb->isProcedure("p3"));
    REQUIRE(pkb->isProcedure("p4"));
    REQUIRE(pkb->isProcedure("p5"));
    REQUIRE(!pkb->isProcedure("x1"));

    REQUIRE(pkb->isVariable("x1"));
    REQUIRE(pkb->isVariable("x2"));
    REQUIRE(!pkb->isVariable("x4"));

    REQUIRE(pkb->getAllStatementsThatCall("p3") == vector<int>({3}));
    REQUIRE(pkb->getAllStatementsThatCall("p2") == vector<int>({1}));
    REQUIRE(pkb->getAllStatementsThatPrint("x2") == vector<int>({9}));
    REQUIRE(pkb->getAllStatementsThatRead("x2") == vector<int>({4}));

    REQUIRE(pkb->getProcedureCalledBy(3) == "p3");
    REQUIRE(pkb->getProcedureCalledBy(1) == "p2");
    REQUIRE(pkb->getVariablePrintedBy(9) == "x2");
    REQUIRE(pkb->getVariableReadBy(4) == "x2");
}

TEST_CASE("Matching While ") {
    string src = "procedure p1 {"
                 "while (x < z+y) {"
                 "x = x-3;"
                 "}"
                 "if ( m == j +k+l) then {"
                 "z = x;}"
                 "else {"
                 "z = x;}"
                 "while ((x<z+y) && (m!=1)) {"
                 "x = x-3;}}"
                 ""
                 ""
                 ""
                 ""
                 ""
                 ""
                 ""
                 "";

    Tokenizer *tokenizer = Tokenizer::getInstance();
    vector<Token> tokenStream = tokenizer->tokenizeSource(src);
    Parser* parser = Parser::getInstance();
    AST root = parser->buildAST(tokenStream);
    PKB *pkb = PKB::getInstance();
    pkb->reset();
    pkb->initAST(root);
    REQUIRE(pkb->isMatch(2,"_", "_x_"));
}

TEST_CASE("IFWHILEPAIR") {
    string src = "procedure simple {\n"
                 "    a = b + c;\n"
                 "    while (v1 == v2) {\n"
                 "        same = value - diferent;\n"
                 "    }\n"
                 "    if (((easy)!=(what+4)) && (level==2)) then{\n"
                 "        too = (easy + question) / mark;\n"
                 "        call complex;\n"
                 "    } else {\n"
                 "        while(!(stage==superNoob)) {\n"
                 "            exit = 1;\n"
                 "            throw = error * 10000;\n"
                 "        }\n"
                 "    }\n"
                 "}\n"
                 "\n"
                 "procedure complex {\n"
                 "    while((j <= (max*10)) || (j >= sum*2)) {\n"
                 "        g1 = 1 + 2 + 3 + g2*8 + g3/9 + g4%3;\n"
                 "        if (v100 == sum) then {\n"
                 "            x = i-j+k/30*25 + l;\n"
                 "        } else {\n"
                 "            if ((x!=a) && (j==a)) then {\n"
                 "                count = count + 15;\n"
                 "            } else {\n"
                 "                while((!(i == 12)) && (j != 13)) {\n"
                 "                    a = b%c;\n"
                 "                    if = (while + ((d + sum) / f)) - 20;\n"
                 "                }\n"
                 "            }\n"
                 "        }\n"
                 "        while (((a<s)&&((d>f)||((55/aa       )<=66)))   && (((!(true==(a+2))) && (0==0)) && (11>=(q*(22+44)/w%e)))) {\n"
                 "            insideWhile = beforeWhile - 0;\n"
                 "            while (left<=       right) {\n"
                 "                if ((!(d==e   )) && ((f+g*2) != h)) then {\n"
                 "                    while (i >= true) {\n"
                 "                        exactLeft = exactRight;\n"
                 "                        if (if==else) then {\n"
                 "                            exactLeft1 = else / ((((2+3)-four)/five)*((six+six2)%seven));\n"
                 "                            left = left % (middle -(right * (left + (middle/(left + (middle - right) * right)))));\n"
                 "                        } else {\n"
                 "                            else = else + 10;\n"
                 "                        }\n"
                 "                    }\n"
                 "                    print abc;\n"
                 "                } else {\n"
                 "                    f = e + 12;\n"
                 "                    g = (2+3)-4*(5+6)+8*(10%2);\n"
                 "                }\n"
                 "                afterWhile = (i*(i-(i/2))+i)+3;\n"
                 "            }\n"
                 "        }\n"
                 "    }\n"
                 "    add = complicated + here + ltr;\n"
                 "}";


    Tokenizer *tokenizer = Tokenizer::getInstance();
    vector<Token> tokenStream = tokenizer->tokenizeSource(src);
    Parser* parser = Parser::getInstance();
    AST root = parser->buildAST(tokenStream);
    PKB *pkb = PKB::getInstance();
    pkb->reset();
    pkb->initAST(root);

    auto pairs = pkb->getMatchedIfPair();
    //REQUIRE(pairs.size() == 2);
    vector<pair<string, string>> test = pkb->getMatchedAssignPair("b%c");
}

TEST_CASE("Affects") {
    string src = "procedure DISNEY {\n"
                 "    disney = 100;\n"
                 "    excitement = 0;\n"
                 "    while((new+movie) == disney) {\n"
                 "        hype = 1000;\n"
                 "        read hype;\n"
                 "        excitement = excitement + hype;\n"
                 "        snow = snow%1+ 2*white;\n"
                 "        white = apple + die;\n"
                 "        call ratings;\n"
                 "        snow = excitement + 101;\n"
                 "    }\n"
                 "    beautiful = very;\n"
                 "    if(princess==beautiful) then {\n"
                 "        snow = 3 - white;\n"
                 "    } else {\n"
                 "        beauty = beast;\n"
                 "    }\n"
                 "    beauty = beauty + snow + white;\n"
                 "    end = evil + queen;\n"
                 "    call marvels;\n"
                 "    disney = end;\n"
                 "    end = disney;\n"
                 "}\n"
                 "\n"
                 "procedure ratings {\n"
                 "    reviews = good / bad;\n"
                 "    snow = ok;\n"
                 "    beauty = bad;\n"
                 "    bad = white;\n"
                 "}\n"
                 "\n"
                 "procedure marvels {\n"
                 "    read end;\n"
                 "    avengers = 200;\n"
                 "    last = scene + end;\n"
                 "    avengers = avengers + 100000;\n"
                 "}";


    Tokenizer *tokenizer = Tokenizer::getInstance();
    vector<Token> tokenStream = tokenizer->tokenizeSource(src);
    Parser* parser = Parser::getInstance();
    AST root = parser->buildAST(tokenStream);
    PKB *pkb = PKB::getInstance();
    pkb->reset();
    pkb->initAST(root);

    vector<int> test = pkb->getAffects(2);
    for (auto i : test) {
        cout<<i<<endl;
    }
    //REQUIRE(find(test.begin(), test.end(), 3) != test.end());
    //test = pkb->getAffects(9);
    //REQUIRE(find(test.begin(), test.end(), 10) != test.end());
    //test = pkb->getAffected(11);
}

TEST_CASE("Affects Star") {
    string src = "procedure Example {"
                 "while (x == x) {"//1
                 "a = c;"//2
                 "b = a;"//3
                 "c = b;"//4
                 "}"
                 "x = x;"//5
                 "y = x;"//6
                 "z = y + b;"//7
                 "}";


    Tokenizer *tokenizer = Tokenizer::getInstance();
    vector<Token> tokenStream = tokenizer->tokenizeSource(src);
    Parser* parser = Parser::getInstance();
    AST root = parser->buildAST(tokenStream);
    PKB *pkb = PKB::getInstance();
    pkb->reset();
    pkb->initAST(root);

    vector<int> test = pkb->getAffectsStar(3);
    REQUIRE(find(test.begin(), test.end(), 2) != test.end());
    test = pkb->getAffectedStar(7);
    REQUIRE(find(test.begin(), test.end(), 3) != test.end());
    REQUIRE(find(test.begin(), test.end(), 5) != test.end());
}

TEST_CASE("NextBIP") {
    string src = "procedure p1 {"
                 "call p3;" //1
                 "x = 5;" //2
                 "}"
                 "procedure p2 {"
                 "call p3;" //3
                 "}"
                 "procedure p3 {"
                 "if (x == x) then {" //4
                 "a = 1;" //5
                 "}else{"
                 "b = 2;" //6
                 "}}"
                 ;


    Tokenizer *tokenizer = Tokenizer::getInstance();
    vector<Token> tokenStream = tokenizer->tokenizeSource(src);
    Parser* parser = Parser::getInstance();
    AST root = parser->buildAST(tokenStream);
    PKB *pkb = PKB::getInstance();
    pkb->reset();
    pkb->initAST(root);
    vector<vector<int>> empty_vector = vector<vector<int>>();
    vector<int> test = pkb->getNextBIP(3, empty_vector);
    REQUIRE(find(test.begin(), test.end(), 4) != test.end());
    test = pkb->getNextBIP(6, empty_vector);
    REQUIRE(find(test.begin(), test.end(), 2) != test.end());
    test = pkb->getNextBIP(5, empty_vector);
    REQUIRE(find(test.begin(), test.end(), 2) != test.end());

    vector<int> prev_empty = vector<int>();
    test = pkb->getPrevBIP(4, prev_empty);
    REQUIRE(find(test.begin(), test.end(), 3) != test.end());
    REQUIRE(find(test.begin(), test.end(), 1) != test.end());

}
#include "catch.hpp"
#include "front_end/parser/TNode.h"

void require(bool b) {
    REQUIRE(b);
}

TEST_CASE("Test TNode Methods") {
    TNode node = TNode();
    auto prog_node = make_shared<ProgramNode>(ProgramNode());
    auto proc_node = make_shared<ProcedureNode>(ProcedureNode("main"));
    auto proc_lst = vector<shared_ptr<ProcedureNode>>{proc_node};
    auto stmt_node =  make_shared<StmtNode>(StmtNode(DesignEntity::Undefined, "", nullptr));
    auto stmt_lst = vector<shared_ptr<StmtNode>>{stmt_node};
    auto var_node = make_shared<VariableNode>(VariableNode("var"));
    auto const_node = make_shared<ConstantNode>(ConstantNode("1"));


    auto op_node = make_shared<OpNode>("+", var_node,const_node);

    auto read_node = make_shared<ReadNode>(ReadNode(proc_node, var_node));
    auto print_node = make_shared<PrintNode>(PrintNode(proc_node, var_node));

    auto rel_expr_node_1 = make_shared<RelExpressionNode>("<", var_node, const_node);
    auto rel_expr_node_2 = make_shared<RelExpressionNode>("<", const_node, var_node);
    auto norm_cond_expr_node_1 = make_shared<NormCondExpressionNode>(rel_expr_node_1);
    auto norm_cond_expr_node_2 = make_shared<NormCondExpressionNode>("&&",rel_expr_node_1, rel_expr_node_2);

    auto if_node = make_shared<IfNode>(proc_node, norm_cond_expr_node_1);
    auto while_node = make_shared<WhileNode>(proc_node, norm_cond_expr_node_2);
    auto assign_node = make_shared<AssignNode>(proc_node, var_node, const_node);
    auto call_node = make_shared<CallNode>(CallNode(proc_node, var_node));

    SECTION("TNode") {
        REQUIRE(node.getType() == DesignEntity::Undefined);
        REQUIRE(node.getValue() == "");
        REQUIRE_THROWS_WITH(node.getProcedures(), "Node of type 'Undefined' and value '' cannot call getProcedures().");
        REQUIRE_THROWS_WITH(node.getParent(), "Node of type 'Undefined' and value '' cannot call getParent().");
        REQUIRE_THROWS_WITH(node.getStatementList(),
                            "Node of type 'Undefined' and value '' cannot call getStatementList().");
        REQUIRE_THROWS_WITH(node.getElseStatementList(),
                            "Node of type 'Undefined' and value '' cannot call getElseStatementList().");
        REQUIRE_THROWS_WITH(node.getExpression(), "Node of type 'Undefined' and value '' cannot call getExpression().");
        REQUIRE_THROWS_WITH(node.getVariable(), "Node of type 'Undefined' and value '' cannot call getVariable().");
        REQUIRE_THROWS_WITH(node.getLeftNode(), "Node of type 'Undefined' and value '' cannot call getLeftNode().");
        REQUIRE_THROWS_WITH(node.getRightNode(), "Node of type 'Undefined' and value '' cannot call getRightNode().");
        REQUIRE_THROWS_WITH(node.addStatementList(stmt_lst),
                            "Node of type 'Undefined' and value '' cannot call addStatementList().");
        REQUIRE_THROWS_WITH(node.addElseStatementList(stmt_lst),
                            "Node of type 'Undefined' and value '' cannot call addElseStatementList().");
        REQUIRE_THROWS_WITH(node.addProcedureList(proc_lst),
                            "Node of type 'Undefined' and value '' cannot call addToProcedureList().");
    }

    SECTION("Program Node") {
        REQUIRE(prog_node->getType() == DesignEntity::Program);
        REQUIRE(prog_node->getValue() == "root");
        REQUIRE_NOTHROW(prog_node->getProcedures(),
                        "Node of type 'Program' and value 'root' cannot call getProcedures().");
        REQUIRE_THROWS_WITH(prog_node->getParent(), "Node of type 'Program' and value 'root' cannot call getParent().");
        REQUIRE_THROWS_WITH(prog_node->getStatementList(),
                            "Node of type 'Program' and value 'root' cannot call getStatementList().");
        REQUIRE_THROWS_WITH(prog_node->getElseStatementList(),
                            "Node of type 'Program' and value 'root' cannot call getElseStatementList().");
        REQUIRE_THROWS_WITH(prog_node->getExpression(),
                            "Node of type 'Program' and value 'root' cannot call getExpression().");
        REQUIRE_THROWS_WITH(prog_node->getVariable(),
                            "Node of type 'Program' and value 'root' cannot call getVariable().");
        REQUIRE_THROWS_WITH(prog_node->getLeftNode(),
                            "Node of type 'Program' and value 'root' cannot call getLeftNode().");
        REQUIRE_THROWS_WITH(prog_node->getRightNode(),
                            "Node of type 'Program' and value 'root' cannot call getRightNode().");
        REQUIRE_THROWS_WITH(prog_node->addStatementList(stmt_lst),
                            "Node of type 'Program' and value 'root' cannot call addStatementList().");
        REQUIRE_THROWS_WITH(prog_node->addElseStatementList(stmt_lst),
                            "Node of type 'Program' and value 'root' cannot call addElseStatementList().");
        REQUIRE_NOTHROW(prog_node->addProcedureList(proc_lst),
                        "Node of type 'Program' and value 'root' cannot call addToProcedureList().");
    }

    SECTION("Procedure Node") {
        REQUIRE(proc_node->getType() == DesignEntity::Procedure);
        REQUIRE(proc_node->getValue() == "main");
        REQUIRE_THROWS_WITH(proc_node->getParent(), "Node of type 'Procedure' and value 'main' cannot call getParent().");
        REQUIRE_NOTHROW(proc_node->getStatementList(),
                        "Node of type 'Procedure' and value 'main' cannot call getStatementList().");
        REQUIRE_THROWS_WITH(proc_node->getElseStatementList(),
                            "Node of type 'Procedure' and value 'main' cannot call getElseStatementList().");
        REQUIRE_THROWS_WITH(proc_node->getExpression(),
                            "Node of type 'Procedure' and value 'main' cannot call getExpression().");
        REQUIRE_THROWS_WITH(proc_node->getVariable(),
                            "Node of type 'Procedure' and value 'main' cannot call getVariable().");
        REQUIRE_THROWS_WITH(proc_node->getLeftNode(),
                            "Node of type 'Procedure' and value 'main' cannot call getLeftNode().");
        REQUIRE_THROWS_WITH(proc_node->getRightNode(),
                            "Node of type 'Procedure' and value 'main' cannot call getRightNode().");
        REQUIRE_NOTHROW(proc_node->addStatementList(stmt_lst),
                        "Node of type 'Procedure' and value 'main' cannot call addStatementList().");
        REQUIRE_THROWS_WITH(proc_node->addElseStatementList(stmt_lst),
                            "Node of type 'Procedure' and value 'main' cannot call addElseStatementList().");
        REQUIRE_THROWS_WITH(proc_node->addProcedureList(proc_lst),
                            "Node of type 'Procedure' and value 'main' cannot call addToProcedureList().");
    }

    SECTION("Read Node") {
        REQUIRE(read_node->getType() == DesignEntity::Read);
        REQUIRE(read_node->getValue() == "read");
        REQUIRE(read_node->getParent() != nullptr);
        REQUIRE_THROWS_WITH(read_node->getStatementList(),
                            "Node of type 'Read' and value 'read' cannot call getStatementList().");
        REQUIRE_THROWS_WITH(read_node->getElseStatementList(),
                            "Node of type 'Read' and value 'read' cannot call getElseStatementList().");
        REQUIRE_THROWS_WITH(read_node->getExpression(),
                            "Node of type 'Read' and value 'read' cannot call getExpression().");
        REQUIRE(read_node->getVariable() != nullptr);
        REQUIRE_THROWS_WITH(read_node->getLeftNode(),
                            "Node of type 'Read' and value 'read' cannot call getLeftNode().");
        REQUIRE_THROWS_WITH(read_node->getRightNode(),
                            "Node of type 'Read' and value 'read' cannot call getRightNode().");
        REQUIRE_THROWS_WITH(read_node->addStatementList(stmt_lst),
                            "Node of type 'Read' and value 'read' cannot call addStatementList().");
        REQUIRE_THROWS_WITH(read_node->addElseStatementList(stmt_lst),
                            "Node of type 'Read' and value 'read' cannot call addElseStatementList().");
        REQUIRE_THROWS_WITH(read_node->addProcedureList(proc_lst),
                            "Node of type 'Read' and value 'read' cannot call addToProcedureList().");
    }

    SECTION("Print Node") {
        REQUIRE(print_node->getType() == DesignEntity::Print);
        REQUIRE(print_node->getValue() == "print");
        REQUIRE(print_node->getParent() != nullptr);
        REQUIRE_THROWS_WITH(print_node->getStatementList(),
                            "Node of type 'Print' and value 'print' cannot call getStatementList().");
        REQUIRE_THROWS_WITH(print_node->getElseStatementList(),
                            "Node of type 'Print' and value 'print' cannot call getElseStatementList().");
        REQUIRE_THROWS_WITH(print_node->getExpression(),
                            "Node of type 'Print' and value 'print' cannot call getExpression().");
        REQUIRE(print_node->getVariable() != nullptr);
        REQUIRE_THROWS_WITH(print_node->getLeftNode(),
                            "Node of type 'Print' and value 'print' cannot call getLeftNode().");
        REQUIRE_THROWS_WITH(print_node->getRightNode(),
                            "Node of type 'Print' and value 'print' cannot call getRightNode().");
        REQUIRE_THROWS_WITH(print_node->addStatementList(stmt_lst),
                            "Node of type 'Print' and value 'print' cannot call addStatementList().");
        REQUIRE_THROWS_WITH(print_node->addElseStatementList(stmt_lst),
                            "Node of type 'Print' and value 'print' cannot call addElseStatementList().");
        REQUIRE_THROWS_WITH(print_node->addProcedureList(proc_lst),
                            "Node of type 'Print' and value 'print' cannot call addToProcedureList().");
    }

    SECTION("Variable Node") {
        REQUIRE(var_node->getType() == DesignEntity::Variable);
        REQUIRE(var_node->getValue() == "var");
        REQUIRE_THROWS_WITH(var_node->getParent(),
                            "Node of type 'Variable' and value 'var' cannot call getParent().");
        REQUIRE_THROWS_WITH(var_node->getStatementList(),
                            "Node of type 'Variable' and value 'var' cannot call getStatementList().");
        REQUIRE_THROWS_WITH(var_node->getElseStatementList(),
                            "Node of type 'Variable' and value 'var' cannot call getElseStatementList().");
        REQUIRE_THROWS_WITH(var_node->getExpression(),
                            "Node of type 'Variable' and value 'var' cannot call getExpression().");
        REQUIRE_THROWS_WITH(var_node->getVariable(),
                            "Node of type 'Variable' and value 'var' cannot call getVariable().");
        REQUIRE_THROWS_WITH(var_node->getRightNode(),
                            "Node of type 'Variable' and value 'var' cannot call getRightNode().");
        REQUIRE_THROWS_WITH(var_node->addStatementList(stmt_lst),
                            "Node of type 'Variable' and value 'var' cannot call addStatementList().");
        REQUIRE_THROWS_WITH(var_node->addElseStatementList(stmt_lst),
                            "Node of type 'Variable' and value 'var' cannot call addElseStatementList().");
        REQUIRE_THROWS_WITH(var_node->addProcedureList(proc_lst),
                            "Node of type 'Variable' and value 'var' cannot call addToProcedureList().");
    }

    SECTION("Constant Node") {
        REQUIRE(const_node->getType() == DesignEntity::Constant);
        REQUIRE(const_node->getValue() == "1");
        REQUIRE_THROWS_WITH(const_node->getParent(),
                            "Node of type 'Constant' and value '1' cannot call getParent().");
        REQUIRE_THROWS_WITH(const_node->getStatementList(),
                            "Node of type 'Constant' and value '1' cannot call getStatementList().");
        REQUIRE_THROWS_WITH(const_node->getElseStatementList(),
                            "Node of type 'Constant' and value '1' cannot call getElseStatementList().");
        REQUIRE_THROWS_WITH(const_node->getExpression(),
                            "Node of type 'Constant' and value '1' cannot call getExpression().");
        REQUIRE_THROWS_WITH(const_node->getVariable(),
                            "Node of type 'Constant' and value '1' cannot call getVariable().");
        REQUIRE_THROWS_WITH(const_node->getRightNode(),
                            "Node of type 'Constant' and value '1' cannot call getRightNode().");
        REQUIRE_THROWS_WITH(const_node->addStatementList(stmt_lst),
                            "Node of type 'Constant' and value '1' cannot call addStatementList().");
        REQUIRE_THROWS_WITH(const_node->addElseStatementList(stmt_lst),
                            "Node of type 'Constant' and value '1' cannot call addElseStatementList().");
        REQUIRE_THROWS_WITH(const_node->addProcedureList(proc_lst),
                            "Node of type 'Constant' and value '1' cannot call addToProcedureList().");
    }

    SECTION("Op Node") {
        REQUIRE(op_node->getType() == DesignEntity::Op);
        REQUIRE(op_node->getValue() == "+");
        REQUIRE_THROWS_WITH(op_node->getParent(),
                            "Node of type 'Op' and value '+' cannot call getParent().");
        REQUIRE_THROWS_WITH(op_node->getStatementList(),
                            "Node of type 'Op' and value '+' cannot call getStatementList().");
        REQUIRE_THROWS_WITH(op_node->getElseStatementList(),
                            "Node of type 'Op' and value '+' cannot call getElseStatementList().");
        REQUIRE_THROWS_WITH(op_node->getExpression(),
                            "Node of type 'Op' and value '+' cannot call getExpression().");
        REQUIRE_THROWS_WITH(op_node->getVariable(),
                            "Node of type 'Op' and value '+' cannot call getVariable().");
        REQUIRE(op_node->getLeftNode() != nullptr);
        REQUIRE(op_node->getRightNode() != nullptr);
        REQUIRE_THROWS_WITH(op_node->addStatementList(stmt_lst),
                            "Node of type 'Op' and value '+' cannot call addStatementList().");
        REQUIRE_THROWS_WITH(op_node->addElseStatementList(stmt_lst),
                            "Node of type 'Op' and value '+' cannot call addElseStatementList().");
        REQUIRE_THROWS_WITH(op_node->addProcedureList(proc_lst),
                            "Node of type 'Op' and value '+' cannot call addToProcedureList().");
    }

    SECTION("Rel Expression Node") {
        REQUIRE(rel_expr_node_1->getType() == DesignEntity::Op);
        REQUIRE(rel_expr_node_1->getValue() == "<");
        REQUIRE_THROWS_WITH(rel_expr_node_1->getParent(),
                            "Node of type 'Op' and value '<' cannot call getParent().");
        REQUIRE_THROWS_WITH(rel_expr_node_1->getStatementList(),
                            "Node of type 'Op' and value '<' cannot call getStatementList().");
        REQUIRE_THROWS_WITH(rel_expr_node_1->getElseStatementList(),
                            "Node of type 'Op' and value '<' cannot call getElseStatementList().");
        REQUIRE_THROWS_WITH(rel_expr_node_1->getExpression(),
                            "Node of type 'Op' and value '<' cannot call getExpression().");
        REQUIRE_THROWS_WITH(rel_expr_node_1->getVariable(),
                            "Node of type 'Op' and value '<' cannot call getVariable().");
        REQUIRE(rel_expr_node_1->getLeftNode() != nullptr);
        REQUIRE(rel_expr_node_1->getRightNode() != nullptr);
        REQUIRE_THROWS_WITH(rel_expr_node_1->addStatementList(stmt_lst),
                            "Node of type 'Op' and value '<' cannot call addStatementList().");
        REQUIRE_THROWS_WITH(rel_expr_node_1->addElseStatementList(stmt_lst),
                            "Node of type 'Op' and value '<' cannot call addElseStatementList().");
        REQUIRE_THROWS_WITH(rel_expr_node_1->addProcedureList(proc_lst),
                            "Node of type 'Op' and value '<' cannot call addToProcedureList().");
    }

    SECTION("Normal Conditional Expression Node (&& and ||)") {
        REQUIRE(norm_cond_expr_node_2->getType() == DesignEntity::Op);
        REQUIRE(norm_cond_expr_node_2->getValue() == "&&");
        REQUIRE_THROWS_WITH(norm_cond_expr_node_2->getParent(),
                            "Node of type 'Op' and value '&&' cannot call getParent().");
        REQUIRE_THROWS_WITH(norm_cond_expr_node_2->getStatementList(),
                            "Node of type 'Op' and value '&&' cannot call getStatementList().");
        REQUIRE_THROWS_WITH(norm_cond_expr_node_2->getElseStatementList(),
                            "Node of type 'Op' and value '&&' cannot call getElseStatementList().");
        REQUIRE_THROWS_WITH(norm_cond_expr_node_2->getExpression(),
                            "Node of type 'Op' and value '&&' cannot call getExpression().");
        REQUIRE_THROWS_WITH(norm_cond_expr_node_2->getVariable(),
                            "Node of type 'Op' and value '&&' cannot call getVariable().");
        REQUIRE(norm_cond_expr_node_2->getLeftNode() != nullptr);
        REQUIRE(norm_cond_expr_node_2->getRightNode() != nullptr);
        REQUIRE_THROWS_WITH(norm_cond_expr_node_2->addStatementList(stmt_lst),
                            "Node of type 'Op' and value '&&' cannot call addStatementList().");
        REQUIRE_THROWS_WITH(norm_cond_expr_node_2->addElseStatementList(stmt_lst),
                            "Node of type 'Op' and value '&&' cannot call addElseStatementList().");
        REQUIRE_THROWS_WITH(norm_cond_expr_node_2->addProcedureList(proc_lst),
                            "Node of type 'Op' and value '&&' cannot call addToProcedureList().");
    }

    SECTION("If Node") {
        REQUIRE(if_node->getType() == DesignEntity::If);
        REQUIRE(if_node->getValue() == "if");
        REQUIRE(if_node->getParent() != nullptr);
        REQUIRE(if_node->getExpression() != nullptr);
        REQUIRE_THROWS_WITH(if_node->getVariable() , "Node of type 'If' and value 'if' cannot call getVariable().");
        REQUIRE_THROWS_WITH(if_node->getLeftNode(),
                            "Node of type 'If' and value 'if' cannot call getLeftNode().");
        REQUIRE_THROWS_WITH(if_node->getRightNode(),
                            "Node of type 'If' and value 'if' cannot call getRightNode().");
        REQUIRE_NOTHROW(if_node->addStatementList(stmt_lst),
                        "Node of type 'If' and value 'if' cannot call addStatementList().");
        REQUIRE_NOTHROW(if_node->addElseStatementList(stmt_lst),
                        "Node of type 'If' and value 'if' cannot call addElseStatementList().");
        REQUIRE(if_node->getStatementList().size() == 1);
        REQUIRE(if_node->getElseStatementList().size() == 1);
        REQUIRE_THROWS_WITH(if_node->addProcedureList(proc_lst),
                            "Node of type 'If' and value 'if' cannot call addToProcedureList().");
    }

    SECTION("While Node") {
        REQUIRE(while_node->getType() == DesignEntity::While);
        REQUIRE(while_node->getValue() == "while");
        REQUIRE(while_node->getParent() != nullptr);
        REQUIRE(while_node->getExpression() != nullptr);
        REQUIRE_THROWS_WITH(while_node->getVariable() , "Node of type 'While' and value 'while' cannot call getVariable().");
        REQUIRE_THROWS_WITH(while_node->getLeftNode(),
                            "Node of type 'While' and value 'while' cannot call getLeftNode().");
        REQUIRE_THROWS_WITH(while_node->getRightNode(),
                            "Node of type 'While' and value 'while' cannot call getRightNode().");
        REQUIRE_NOTHROW(while_node->addStatementList(stmt_lst),
                        "Node of type 'While' and value 'while' cannot call addStatementList().");
        REQUIRE_THROWS_WITH(while_node->addElseStatementList(stmt_lst),
                            "Node of type 'While' and value 'while' cannot call addElseStatementList().");
        REQUIRE(while_node->getStatementList().size() == 1);
        REQUIRE_THROWS_WITH(while_node->getElseStatementList(),  "Node of type 'While' and value 'while' cannot call getElseStatementList().");
        REQUIRE_THROWS_WITH(while_node->addProcedureList(proc_lst),
                            "Node of type 'While' and value 'while' cannot call addToProcedureList().");
    }

    SECTION("Assign Node") {
        REQUIRE(assign_node->getType() == DesignEntity::Assign);
        REQUIRE(assign_node->getValue() == "assign");
        REQUIRE(assign_node->getParent() != nullptr);
        REQUIRE_THROWS_WITH(assign_node->getStatementList(),
                            "Node of type 'Assign' and value 'assign' cannot call getStatementList().");
        REQUIRE_THROWS_WITH(assign_node->getElseStatementList(),
                            "Node of type 'Assign' and value 'assign' cannot call getElseStatementList().");
        REQUIRE(assign_node->getExpression() != nullptr);
        REQUIRE(assign_node->getVariable() != nullptr);
        REQUIRE_THROWS_WITH(assign_node->getLeftNode(),
                            "Node of type 'Assign' and value 'assign' cannot call getLeftNode().");
        REQUIRE_THROWS_WITH(assign_node->getRightNode(),
                            "Node of type 'Assign' and value 'assign' cannot call getRightNode().");
        REQUIRE_THROWS_WITH(assign_node->addStatementList(stmt_lst),
                            "Node of type 'Assign' and value 'assign' cannot call addStatementList().");
        REQUIRE_THROWS_WITH(assign_node->addElseStatementList(stmt_lst),
                            "Node of type 'Assign' and value 'assign' cannot call addElseStatementList().");
        REQUIRE_THROWS_WITH(assign_node->addProcedureList(proc_lst),
                            "Node of type 'Assign' and value 'assign' cannot call addToProcedureList().");
    }

    SECTION("CallNode") {
        REQUIRE(call_node->getType() == DesignEntity::Call);
        REQUIRE(call_node->getValue() == "call");
        REQUIRE_THROWS_WITH(call_node->getProcedures(), "Node of type 'Call' and value 'call' cannot call getProcedures().");
        REQUIRE_NOTHROW(call_node->getParent());
        REQUIRE_THROWS_WITH(call_node->getStatementList(),
                            "Node of type 'Call' and value 'call' cannot call getStatementList().");
        REQUIRE_THROWS_WITH(call_node->getElseStatementList(),
                            "Node of type 'Call' and value 'call' cannot call getElseStatementList().");
        REQUIRE_THROWS_WITH(call_node->getExpression(), "Node of type 'Call' and value 'call' cannot call getExpression().");
        REQUIRE_NOTHROW(call_node->getVariable());
        REQUIRE_THROWS_WITH(call_node->getLeftNode(), "Node of type 'Call' and value 'call' cannot call getLeftNode().");
        REQUIRE_THROWS_WITH(call_node->getRightNode(), "Node of type 'Call' and value 'call' cannot call getRightNode().");
        REQUIRE_THROWS_WITH(call_node->addStatementList(stmt_lst),
                            "Node of type 'Call' and value 'call' cannot call addStatementList().");
        REQUIRE_THROWS_WITH(call_node->addElseStatementList(stmt_lst),
                            "Node of type 'Call' and value 'call' cannot call addElseStatementList().");
        REQUIRE_THROWS_WITH(call_node->addProcedureList(proc_lst),
                            "Node of type 'Call' and value 'call' cannot call addToProcedureList().");
    }

}

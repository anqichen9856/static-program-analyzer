#include "TNode.h"

TNode::TNode() {}

TNode::TNode(DesignEntity type, string value) : type(type), value(move(value)) {}

DesignEntity TNode::getType() {
    return type;
}

string TNode::getValue() {
    return value;
}

shared_ptr<TNode> TNode::getParent() {
    throw InvalidNodeException(getErrorMsg() + "getParent().");
}

vector<shared_ptr<TNode>> TNode::getProcedures() {
    throw InvalidNodeException(getErrorMsg() + "getProcedures().");
}

shared_ptr<TNode> TNode::getExpression() {
    throw InvalidNodeException(getErrorMsg() + "getExpression().");
}

vector<shared_ptr<TNode>> TNode::getStatementList() {
    throw InvalidNodeException(getErrorMsg() + "getStatementList().");
}

vector<shared_ptr<TNode>> TNode::getElseStatementList() {
    throw InvalidNodeException(getErrorMsg() + "getElseStatementList().");
}

shared_ptr<TNode> TNode::getVariable() {
    throw InvalidNodeException(getErrorMsg() + "getVariable().");
}

shared_ptr<TNode> TNode::getLeftNode() {
    throw InvalidNodeException(getErrorMsg() + "getLeftNode().");
}

shared_ptr<TNode> TNode::getRightNode() {
    throw InvalidNodeException(getErrorMsg() + "getRightNode().");
}

bool TNode::addStatementList(vector<shared_ptr<StmtNode>> stmt_lst) {
    throw InvalidNodeException(getErrorMsg() + "addStatementList().");
}

bool TNode::addElseStatementList(vector<shared_ptr<StmtNode>> stmt_lst) {
    throw InvalidNodeException(getErrorMsg() + "addElseStatementList().");
}

bool TNode::addProcedureList(vector<shared_ptr<ProcedureNode>> proc) {
    throw InvalidNodeException(getErrorMsg() + "addToProcedureList().");
}

bool TNode::addWildcard() {
    throw InvalidNodeException(getErrorMsg() + "getWildcard().");
}

string TNode::getErrorMsg() {
    string type_str;
    switch (type) {
        case DesignEntity::Program :
            type_str = "Program";
            break;
        case DesignEntity::Procedure:
            type_str = "Procedure";
            break;
        case DesignEntity::If :
            type_str = "If";
            break;
        case DesignEntity::While :
            type_str = "While";
            break;
        case DesignEntity::Assign :
            type_str = "Assign";
            break;
        case DesignEntity::Read:
            type_str = "Read";
            break;
        case DesignEntity::Print:
            type_str = "Print";
            break;
        case DesignEntity::Op:
            type_str = "Op";
            break;
        case DesignEntity::Call:
            type_str = "Call";
            break;
        case DesignEntity::Variable:
            type_str = "Variable";
            break;
        case DesignEntity::Constant:
            type_str = "Constant";
            break;
        default:
            type_str = "Undefined";
    }
    return "Node of type '" + type_str + "' and value '" + value + "' cannot call ";
}

ProgramNode::ProgramNode() : TNode(DesignEntity::Program, "root"),
                             proc_lst(vector<shared_ptr<ProcedureNode>>{}) {}

vector<shared_ptr<TNode>> ProgramNode::getProcedures() {
    vector<shared_ptr<TNode>> temp(proc_lst.begin(), proc_lst.end());
    return temp;
}

bool ProgramNode::addProcedureList(vector<shared_ptr<ProcedureNode>> proc) {
    this->proc_lst = move(proc);
    return true;
}

ProcedureNode::ProcedureNode(string value) : TNode(DesignEntity::Procedure,
                                                   move(value)), stmt_lst(
        vector<shared_ptr<StmtNode>>{}) {}

vector<shared_ptr<TNode>> ProcedureNode::getStatementList() {
    vector<shared_ptr<TNode>> temp(stmt_lst.begin(), stmt_lst.end());
    return temp;
}

bool ProcedureNode::addStatementList(vector<shared_ptr<StmtNode>> _stmt_lst) {
    this->stmt_lst = move(_stmt_lst);
    return true;
}

StmtNode::StmtNode(DesignEntity type, string value, const shared_ptr<TNode> &parent) : TNode(type, move(
        value)),
                                                                                       parent(parent) {}

shared_ptr<TNode> StmtNode::getParent() {
    return parent.lock();
}

IfNode::IfNode(const shared_ptr<TNode> &parent,
               shared_ptr<CondExpressionNode> expr) : StmtNode(DesignEntity::If, "if", parent),
                                                      expr(move(expr)),
                                                      then_stmt_lst(vector<shared_ptr<StmtNode>>{}),
                                                      else_stmt_lst(vector<shared_ptr<StmtNode>>{}) {}

shared_ptr<TNode> IfNode::getExpression() {
    return expr;
}

vector<shared_ptr<TNode>> IfNode::getStatementList() {
    vector<shared_ptr<TNode>> temp(then_stmt_lst.begin(), then_stmt_lst.end());
    return temp;
}

vector<shared_ptr<TNode>> IfNode::getElseStatementList() {
    vector<shared_ptr<TNode>> temp(else_stmt_lst.begin(), else_stmt_lst.end());
    return temp;
}

bool IfNode::addStatementList(vector<shared_ptr<StmtNode>> stmt_lst) {
    then_stmt_lst = move(stmt_lst);
    return true;
}

bool IfNode::addElseStatementList(vector<shared_ptr<StmtNode>> stmt_lst) {
    else_stmt_lst = move(stmt_lst);
    return true;
}

WhileNode::WhileNode(const shared_ptr<TNode> &parent, shared_ptr<CondExpressionNode> expr) : StmtNode(
        DesignEntity::While, "while", parent), expr(move(expr)), then_stmt_lst(
        vector<shared_ptr<StmtNode>>{}) {}

shared_ptr<TNode> WhileNode::getExpression() {
    return expr;
}

vector<shared_ptr<TNode>> WhileNode::getStatementList() {
    vector<shared_ptr<TNode>> temp(then_stmt_lst.begin(), then_stmt_lst.end());
    return temp;
}

bool WhileNode::addStatementList(vector<shared_ptr<StmtNode>> stmt_lst) {
    then_stmt_lst = move(stmt_lst);
    return true;
}

ReadNode::ReadNode(const shared_ptr<TNode> &parent,
                   shared_ptr<VariableNode> var) : StmtNode(DesignEntity::Read, "read", parent),
                                                   var(move(var)) {}

shared_ptr<TNode> ReadNode::getVariable() {
    return var;
}

PrintNode::PrintNode(const shared_ptr<TNode> &parent,
                     shared_ptr<VariableNode> var) : StmtNode(DesignEntity::Print, "print", parent),
                                                     var(move(var)) {}

shared_ptr<TNode> PrintNode::getVariable() {
    return var;
}

AssignNode::AssignNode(const shared_ptr<TNode> &parent,
                       shared_ptr<VariableNode> var, shared_ptr<ExpressionNode> expr) : StmtNode(
        DesignEntity::Assign, "assign", parent), var(move(var)), expr(move(expr)) {}

shared_ptr<TNode> AssignNode::getVariable() {
    return var;
}

shared_ptr<TNode> AssignNode::getExpression() {
    return expr;
}

CallNode::CallNode(const shared_ptr<TNode> &parent,
                   shared_ptr<VariableNode> var) : StmtNode(DesignEntity::Call, "call", parent),
                                                   var(move(var)) {}

shared_ptr<TNode> CallNode::getVariable() {
    return var;
}

ExpressionNode::ExpressionNode(DesignEntity type, string value) : TNode(type, std::move(value)) {}


OpNode::OpNode(string value, shared_ptr<ExpressionNode> lhs, shared_ptr<ExpressionNode> rhs) : ExpressionNode(
        DesignEntity::Op, move(value)), lhs(move(lhs)), rhs(move(rhs)) {}

shared_ptr<TNode> OpNode::getLeftNode() {
    return lhs;
}

shared_ptr<TNode> OpNode::getRightNode() {
    return rhs;
}

ConstantNode::ConstantNode(string value) : ExpressionNode(DesignEntity::Constant, move(value)) {}

shared_ptr<TNode> ConstantNode::getLeftNode() {
    return wildcard;
}

bool ConstantNode::addWildcard() {
    wildcard = make_shared<WildcardNode>(WildcardNode());
    return true;
}

VariableNode::VariableNode(string value) : ExpressionNode(DesignEntity::Variable, move(value)) {}

bool VariableNode::addWildcard() {
    wildcard = make_shared<WildcardNode>(WildcardNode());
    return true;
}

shared_ptr<TNode> VariableNode::getLeftNode() {
    return wildcard;
}

CondExpressionNode::CondExpressionNode(string value) : TNode(DesignEntity::Op, move(value)) {}

NormCondExpressionNode::NormCondExpressionNode(string value, shared_ptr<CondExpressionNode> lhs,
                                               shared_ptr<CondExpressionNode> rhs) : CondExpressionNode(
        move(value)), lhs(move(lhs)), rhs(move(rhs)) {}

NormCondExpressionNode::NormCondExpressionNode(shared_ptr<CondExpressionNode> lhs) : CondExpressionNode("!"),
                                                                                     lhs(move(lhs)) {}

shared_ptr<TNode> NormCondExpressionNode::getLeftNode() {
    return lhs;
}

shared_ptr<TNode> NormCondExpressionNode::getRightNode() {
    return rhs;
}

RelExpressionNode::RelExpressionNode(string value, shared_ptr<ExpressionNode> lhs,
                                     shared_ptr<ExpressionNode> rhs) : CondExpressionNode(move(value)),
                                                                       lhs(move(lhs)),
                                                                       rhs(move(rhs)) {}

shared_ptr<TNode> RelExpressionNode::getLeftNode() {
    return lhs;
}

shared_ptr<TNode> RelExpressionNode::getRightNode() {
    return rhs;
}

WildcardNode::WildcardNode() : ExpressionNode(DesignEntity::Wildcard, "_") {}

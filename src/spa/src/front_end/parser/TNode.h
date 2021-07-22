#ifndef AUTOTESTER_TNODE_H
#define AUTOTESTER_TNODE_H

#include "utils/SPAEnums.h"
#include "exceptions/SPAException.h"
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <optional>

using namespace std;

class TNode;

class ProgramNode;

class ProcedureNode;

class OpNode;

class StmtNode;

class IfNode;

class WhileNode;

class AssignNode;

class ReadNode;

class PrintNode;

class CondExpressionNode;

class NormCondExpressionNode;

class RelExpressionNode;

class VariableNode;

class ConstantNode;

class ExpressionNode;

class WildcardNode;

using AST = shared_ptr<TNode>;


class TNode {
public:
    TNode();

    TNode(DesignEntity type, string value);

    //API for Design Extractor
    virtual DesignEntity getType();

    virtual string getValue();

    virtual shared_ptr<TNode> getParent();

    virtual vector<shared_ptr<TNode> > getProcedures();

    virtual shared_ptr<TNode> getExpression();

    virtual vector<shared_ptr<TNode> > getStatementList();

    virtual vector<shared_ptr<TNode> > getElseStatementList();

    virtual shared_ptr<TNode> getVariable();

    virtual shared_ptr<TNode> getLeftNode();

    virtual shared_ptr<TNode> getRightNode();

    //API for Parser
    virtual bool addStatementList(vector<shared_ptr<StmtNode> > stmt_lst);

    virtual bool addElseStatementList(vector<shared_ptr<StmtNode> > stmt_lst);

    virtual bool addProcedureList(vector<shared_ptr<ProcedureNode>> proc);

    virtual bool addWildcard();

    virtual ~TNode() = default;

protected:
    string getErrorMsg();

private:
    string value;
    DesignEntity type = DesignEntity::Undefined;
};

class ProgramNode : public TNode {
public:
    ProgramNode();

    vector<shared_ptr<TNode> > getProcedures() override;

    bool addProcedureList(vector<shared_ptr<ProcedureNode>> proc) override;

private:
    vector<shared_ptr<ProcedureNode> > proc_lst;
};

class ProcedureNode : public TNode {
public:
    ProcedureNode(string value);

    vector<shared_ptr<TNode> > getStatementList() override;

    bool addStatementList(vector<shared_ptr<StmtNode> > stmt_lst) override;

private:
    vector<shared_ptr<StmtNode> > stmt_lst;
    weak_ptr<TNode> parent;
};

class StmtNode : public TNode {
public:
    StmtNode(DesignEntity type, string value, const shared_ptr<TNode> &parent);

    shared_ptr<TNode> getParent() override;

private:
    weak_ptr<TNode> parent;
};

class IfNode : public StmtNode {
public:
    IfNode(const shared_ptr<TNode> &parent,
           shared_ptr<CondExpressionNode> expr);

    shared_ptr<TNode> getExpression() override;

    vector<shared_ptr<TNode> > getStatementList() override;

    vector<shared_ptr<TNode> > getElseStatementList() override;

    bool addStatementList(vector<shared_ptr<StmtNode> > stmt_lst) override;

    bool addElseStatementList(vector<shared_ptr<StmtNode> > stmt_lst) override;

private:
    shared_ptr<CondExpressionNode> expr;
    vector<shared_ptr<StmtNode> > then_stmt_lst;
    vector<shared_ptr<StmtNode> > else_stmt_lst;
};

class WhileNode : public StmtNode {
public:
    WhileNode(const shared_ptr<TNode> &parent,
              shared_ptr<CondExpressionNode> expr);

    shared_ptr<TNode> getExpression() override;

    vector<shared_ptr<TNode> > getStatementList() override;

    bool addStatementList(vector<shared_ptr<StmtNode> > stmt_lst) override;

private:
    shared_ptr<CondExpressionNode> expr;
    vector<shared_ptr<StmtNode> > then_stmt_lst;
};

class ReadNode : public StmtNode {
public:
    ReadNode(const shared_ptr<TNode> &parent,
             shared_ptr<VariableNode> var);

    shared_ptr<TNode> getVariable() override;

private:
    shared_ptr<VariableNode> var;
};

class PrintNode : public StmtNode {
public:
    PrintNode(const shared_ptr<TNode> &parent,
              shared_ptr<VariableNode> var);

    shared_ptr<TNode> getVariable() override;

private:
    shared_ptr<VariableNode> var;
};

class AssignNode : public StmtNode {
public:
    AssignNode(const shared_ptr<TNode> &parent,
               shared_ptr<VariableNode> var, shared_ptr<ExpressionNode> expr);

    shared_ptr<TNode> getVariable() override;

    shared_ptr<TNode> getExpression() override;

private:
    shared_ptr<VariableNode> var;
    shared_ptr<ExpressionNode> expr;
};

class ExpressionNode: public TNode {
public:
    explicit ExpressionNode(DesignEntity type, string value);

};

class VariableNode : public ExpressionNode {
private:
    shared_ptr<WildcardNode> wildcard = nullptr;
public:
    explicit VariableNode(string value);
    shared_ptr<TNode> getLeftNode() override;
    bool addWildcard() override;
};

class ConstantNode : public ExpressionNode {
private:
    shared_ptr<WildcardNode> wildcard = nullptr;
public:
    explicit ConstantNode(string value);
    shared_ptr<TNode> getLeftNode() override;
    bool addWildcard() override;
};

class OpNode : public ExpressionNode {
public:
    // + , - , / , * , %
    OpNode(string value, shared_ptr<ExpressionNode> lhs, shared_ptr<ExpressionNode> rhs);

    shared_ptr<TNode> getLeftNode() override;

    shared_ptr<TNode> getRightNode() override;

private:
    shared_ptr<ExpressionNode> lhs = nullptr;
    shared_ptr<ExpressionNode> rhs = nullptr;
};

class CondExpressionNode : public TNode {
public:
    explicit CondExpressionNode(string value);
};

class NormCondExpressionNode : public CondExpressionNode {
public:
    // &&, ||
    NormCondExpressionNode(string value, shared_ptr<CondExpressionNode> lhs,
                           shared_ptr<CondExpressionNode> rhs);

    // !
    explicit NormCondExpressionNode(shared_ptr<CondExpressionNode> lhs);

    shared_ptr<TNode> getLeftNode() override;

    shared_ptr<TNode> getRightNode() override;

private:
    shared_ptr<CondExpressionNode> lhs = nullptr;
    shared_ptr<CondExpressionNode> rhs = nullptr;
};

class RelExpressionNode : public CondExpressionNode {
public:
    // <, > , <=, >= , == , !=
    RelExpressionNode(string value, shared_ptr<ExpressionNode> lhs, shared_ptr<ExpressionNode> rhs);

    shared_ptr<TNode> getLeftNode() override;

    shared_ptr<TNode> getRightNode() override;

private:
    shared_ptr<ExpressionNode> lhs = nullptr;
    shared_ptr<ExpressionNode> rhs = nullptr;
};

class WildcardNode : public ExpressionNode {
public:
    explicit WildcardNode();
};

class CallNode : public StmtNode {
public:
    CallNode(const shared_ptr<TNode> &parent,
             shared_ptr<VariableNode> var);

    shared_ptr<TNode> getVariable() override;

private:
    shared_ptr<VariableNode> var;
};

#endif //AUTOTESTER_TNODE_H

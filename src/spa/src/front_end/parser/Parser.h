#ifndef AUTOTESTER_PARSER_H
#define AUTOTESTER_PARSER_H

#include "TNode.h"
#include "front_end/tokenizer/Token.h"
#include "exceptions/SPAException.h"
#include <vector>
#include <memory>
#include <unordered_map>

using namespace std;

class Parser;

class Parser {
private:
    vector<Token> token_stream;

    unordered_map<string, int> expr_bp;

    int current = -1;

    static Parser* instance;

    Token *next();

    Token *peek();

    void expect(const string &str);

    bool match(Token::TokenType type);

    bool match(const string &str);

    bool matchNext(Token::TokenType type);

    bool matchNext(const string &str);

    shared_ptr<ProgramNode> createProgramNode();

    shared_ptr<ProcedureNode> createProcedureNode(const shared_ptr<ProgramNode> &parent);

    shared_ptr<StmtNode> createStatementNode(const shared_ptr<TNode> &parent);

    vector<shared_ptr<StmtNode> > createStatementList(const shared_ptr<TNode> &parent);

    shared_ptr<ReadNode> createReadNode(const shared_ptr<TNode> &parent);

    shared_ptr<CallNode> createCallNode(const shared_ptr<TNode> &parent);

    shared_ptr<PrintNode> createPrintNode(const shared_ptr<TNode> &parent);

    shared_ptr<IfNode> createIfNode(const shared_ptr<TNode> &parent);

    shared_ptr<WhileNode> createWhileNode(const shared_ptr<TNode> &parent);

    shared_ptr<AssignNode> createAssignNode(const shared_ptr<TNode> &parent);

    shared_ptr<CondExpressionNode> createCondExpressionNode();

    shared_ptr<RelExpressionNode> createRelExpressionNode();

    shared_ptr<VariableNode> createVariableNode();

    shared_ptr<ConstantNode> createConstantNode();

    //Expression Creator
    shared_ptr<ExpressionNode> createExpression(int bp);

    shared_ptr<ExpressionNode> nud();

    shared_ptr<ExpressionNode> led(shared_ptr<ExpressionNode> left, const string &op);

    string recExpressionToString(shared_ptr<TNode> root);

    int getBp(const string &op);

public:
    shared_ptr<TNode> buildExpression(const vector<Token>& token_stream);

    shared_ptr<TNode> buildCondExpression(const vector<Token>& token_stream);

    shared_ptr<TNode> buildAST(const vector<Token>& token_stream);

    explicit Parser();

    static Parser* getInstance();

    string expressionToString(shared_ptr<TNode> root);

};





#endif //AUTOTESTER_PARSER_H

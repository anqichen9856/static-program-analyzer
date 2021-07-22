#include "Parser.h"

Parser::Parser() {
    expr_bp.emplace("+", 1);
    expr_bp.emplace("-", 1);
    expr_bp.emplace("/", 2);
    expr_bp.emplace("*", 2);
    expr_bp.emplace("%", 2);
}

Parser *Parser::instance = nullptr;

Parser *Parser::getInstance() {
    if (instance == nullptr) {
        instance = new Parser();
    }
    return instance;
}

Token *Parser::next() {
    return &token_stream[++current];
}

Token *Parser::peek() {
    return &token_stream[current + 1];
}

void Parser::expect(const string &str) {
    if (next()->getTokenValue() != str) {
        throw InvalidTokenException("'" + str + "'");
    }
}

bool Parser::match(const string &str) {
    return token_stream[current].getTokenValue() == str;
}

bool Parser::match(Token::TokenType type) {
    return token_stream[current].getTokenType() == type;
}

bool Parser::matchNext(const string &str) {
    if (current + 1 >= token_stream.size()) return false;
    return token_stream[current + 1].getTokenValue() == str;
}

bool Parser::matchNext(Token::TokenType type) {
    return token_stream[current + 1].getTokenType() == type;
}


shared_ptr<TNode> Parser::buildAST(const vector<Token>& _token_stream) {
    //Initialize
    token_stream = _token_stream;
    current = -1;

    auto root = createProgramNode();
    auto proc_lst =vector<shared_ptr<ProcedureNode>>{};
    while (!matchNext(Token::TokenType::EndOfFile)) {
        auto proc = createProcedureNode(root);
        proc_lst.emplace_back(proc);
    }
    root->addProcedureList(
            move(proc_lst));
    return root;
}

shared_ptr<TNode> Parser::buildExpression(const vector<Token>& _token_stream) {
    //Initialize
    token_stream = _token_stream;
    current = -1;

    try {
        auto expr = createExpression(0);
        if (token_stream.size() > current+2) {
            throw InvalidParserException("Invalid Expression Statement: expected valid expression operator, got: '" +
                                         token_stream[current+1].getTokenValue() + "'.");
        }
        return expr;
    } catch(InvalidTokenException &e) {
        throw InvalidParserException("Invalid Expression Statement: expected "+ string(e.what()) +", got: '" +
                                     token_stream[current].getTokenValue() + "'.");
    }
}

shared_ptr<TNode> Parser::buildCondExpression(const vector<Token>& _token_stream) {
    //Initialize
    token_stream = _token_stream;
    current = -1;
    try {
        auto cond_expr = createCondExpressionNode();
        if (token_stream.size() > current+2) {
            throw InvalidParserException("Invalid Conditional Expression Statement: expected valid expression operator, got: '" +
                                         token_stream[current+1].getTokenValue() + "'.");
        }
        return cond_expr;
    } catch(InvalidTokenException &e) {
        throw InvalidParserException("Invalid Conditional Expression Statement: expected "+ string(e.what()) +", got: '" +
                                     token_stream[current].getTokenValue() + "'.");
    }

}

shared_ptr<ProgramNode> Parser::createProgramNode() {
    return make_shared<ProgramNode>(ProgramNode());
}


shared_ptr<ProcedureNode> Parser::createProcedureNode(const shared_ptr<ProgramNode> &parent) {
    try {
        expect("procedure");
        next();
        if (!(match(Token::TokenType::Keyword) || match(Token::TokenType::Identifier))) {
            throw InvalidTokenException("procedure name");
        }
        auto node = ProcedureNode(token_stream[current].getTokenValue());
        auto node_ptr =make_shared<ProcedureNode>(node);
        auto stmt_lst = createStatementList(node_ptr);
        node_ptr->addStatementList(move(stmt_lst));
        return node_ptr;
    } catch (InvalidTokenException &e) {
        throw InvalidParserException(
                "Invalid Procedure: expected " + string(e.what()) + ", got '" + token_stream[current].getTokenValue() +
                "'.");
    }
}


vector<shared_ptr<StmtNode>> Parser::createStatementList(const shared_ptr<TNode> &parent) {
    vector<shared_ptr<StmtNode>> stmt_lst;
    expect("{");
    while (!(matchNext("}"))) {
        auto statement = createStatementNode(parent);
        stmt_lst.push_back(move(statement));
    }
    expect("}");
    if(stmt_lst.size()==0){
        throw InvalidParserException("Invalid Statement List: expected 1 or more statements, got '}'.");
    }
    return stmt_lst;
}


shared_ptr<StmtNode> Parser::createStatementNode(const
shared_ptr<TNode> &parent) {
    next();
    if (matchNext("=")) { //Assign must be parsed first as keywords can be variable names
        return createAssignNode(parent);
    } else if (match("read")) {
        next();
        return createReadNode(parent);
    } else if (match("print")) {
        next();
        return createPrintNode(parent);
    } else if (match("while")) {
        return createWhileNode(parent);
    } else if (match("if")) {
        return createIfNode(parent);
    } else if (match("call")) {
        next();
        return createCallNode(parent);
    } else {
        //Incorrect Node: throw error
        throw InvalidParserException("Invalid statement starting with: '" + token_stream[current].getTokenValue() + "'.");
    }
}


shared_ptr<ReadNode> Parser::createReadNode(const shared_ptr<TNode> &parent) {
    try {
        auto var = createVariableNode();
        expect(";");
        return make_shared<ReadNode>(parent,move(var));
    } catch (InvalidTokenException &e) {
        throw InvalidParserException("Invalid Read Statement: expected " + string(e.what()) + ", got '" +
                                token_stream[current].getTokenValue() + "'.");
    }
}

shared_ptr<CallNode> Parser::createCallNode(const shared_ptr<TNode> &parent) {
    try {
        auto var = createVariableNode();
        expect(";");
        return make_shared<CallNode>(parent,move(var));
    } catch (InvalidTokenException &e) {
        throw InvalidParserException("Invalid Call Statement: expected " + string(e.what()) + ", got '" +
                                     token_stream[current].getTokenValue() + "'.");
    }
}


shared_ptr<PrintNode> Parser::createPrintNode(const shared_ptr<TNode> &parent) {
    try {
        auto var = createVariableNode();
        expect(";");
        return make_shared<PrintNode>(parent,move(var));
    } catch (InvalidTokenException &e) {
        throw InvalidParserException("Invalid Print Statement: expected " + string(e.what()) + ", got '" +
                                token_stream[current].getTokenValue() + "'.");
    }
}


shared_ptr<AssignNode> Parser::createAssignNode(const shared_ptr<TNode> &parent) {
    try {
        auto var = createVariableNode();
        expect("=");
        auto expression = createExpression(0);
        expect(";");
        auto node = make_shared<AssignNode>(parent,move(var),move(expression));
        return node;
    } catch (InvalidTokenException &e) {
        throw InvalidParserException("Invalid Assign Statement: expected " + string(e.what()) + ", got '" +
                                token_stream[current].getTokenValue() + "'.");
    }
}


shared_ptr<WhileNode> Parser::createWhileNode(const shared_ptr<TNode> &parent) {
    try {
        expect("(");
        auto cond = createCondExpressionNode();
        expect(")");
        auto while_node =
                make_shared<WhileNode>(parent,move(cond));
        auto then_stmt_lst = createStatementList(while_node);
        while_node->addStatementList(move(then_stmt_lst));
        return while_node;
    } catch (InvalidTokenException &e) {
        throw InvalidParserException("Invalid While Statement: expected " + string(e.what()) + ", got '" +
                                token_stream[current].getTokenValue() + "'.");

    }
}


shared_ptr<IfNode> Parser::createIfNode(const shared_ptr<TNode> &parent) {
    try {
        expect("(");
        auto cond = createCondExpressionNode();
        expect(")");
        expect("then");
        auto if_node =make_shared<IfNode>(parent,move(cond));
        auto then_stmt_lst = createStatementList(if_node);
        expect("else");
        auto else_stmt_lst = createStatementList(if_node);
        if_node->addStatementList(
                move(then_stmt_lst));
        if_node->addElseStatementList(
                move(else_stmt_lst));
        return if_node;
    } catch (InvalidTokenException &e) {
        throw InvalidParserException("Invalid If Statement: expected " + string(e.what()) + ", got '" +
                                token_stream[current].getTokenValue() + "'.");
    }
}


shared_ptr<VariableNode> Parser::createVariableNode() {
    if (!(match(Token::Token::Identifier) || match(Token::Token::Keyword))) {
        throw InvalidTokenException("variable");
    }
    auto node = VariableNode(token_stream[current].getTokenValue());
    return
    make_shared<VariableNode>(node);
}


shared_ptr<ConstantNode> Parser::createConstantNode() {
    if (!(match(Token::Token::Literal))) {
        throw InvalidParserException("constant");
    }
    auto node = ConstantNode(token_stream[current].getTokenValue());
    return
    make_shared<ConstantNode>(node);
}


shared_ptr<CondExpressionNode> Parser::createCondExpressionNode() {

    /* Case 1: ! (cond_expr) */
    if (matchNext("!")) {
        expect("!");
        expect("(");
        auto node = createCondExpressionNode();
        expect(")");
        return
        make_shared<NormCondExpressionNode>(
                move(node));
    }

    /* Case 2: rel_expr */
    if (!matchNext("(")) {
        auto expr = createRelExpressionNode();
        return expr;
    }

    /* Case 3: (cond_expr) || (cond_expr), (cond_expr) && (cond_expr) */
    int current_pos = current;
    try {
        auto expr = createRelExpressionNode();
        return expr;
    } catch (InvalidTokenException& e) {
        current = current_pos;
    }

    expect("(");
    auto lhs = createCondExpressionNode();
    expect(")");
    next();
    if (!(match("||") || match("&&"))) {
        throw InvalidTokenException("conditional operator");
    }

    string op = token_stream[current].getTokenValue();

    expect("(");
    auto rhs = createCondExpressionNode();
    expect(")");
    NormCondExpressionNode cond_expr(op,move(lhs),move(rhs));
    return make_shared<NormCondExpressionNode>(cond_expr);
}


shared_ptr<RelExpressionNode> Parser::createRelExpressionNode() {
    auto lhs = createExpression(0);
    next();
    if (!(match("<") || match(">") || match("==") ||
          match("!=") || match("<=") || match(">="))) {
        throw InvalidTokenException("relative expression comparator");
    }

    string op = token_stream[current].getTokenValue();
    auto rhs = createExpression(0);
    auto rel_node = RelExpressionNode(op, move(lhs), move(rhs));
    return make_shared<RelExpressionNode>(rel_node);
}

int Parser::getBp(const string &op) {
    return expr_bp[op];
}

shared_ptr<ExpressionNode> Parser::createExpression(int bp) {
    shared_ptr<ExpressionNode> left;
    if (matchNext("(")) {
        expect("(");
        left = createExpression(0);
        expect(")");
    } else {
        left = nud();
    }

    string op = peek()->getTokenValue();
    while (expr_bp.count(op) != 0 && getBp(op) > bp) {
        expect(op);
        left = led(move(left), op);
        op = peek()->getTokenValue();
    }
    return left;
}

shared_ptr<ExpressionNode> Parser::led(shared_ptr<ExpressionNode> left, const string &op) {
    int bp = getBp(op);
    auto right = createExpression(bp);

    return make_shared<OpNode>(op,move(left),move(right));
}

shared_ptr<ExpressionNode> Parser::nud() {
    try {
        next();
        if (match(Token::TokenType::Literal)) {
            return createConstantNode();
        } else {
            return createVariableNode();
        }
    } catch (InvalidTokenException &e) {
        throw InvalidTokenException("expression constant or variable");
    }
}

string Parser::expressionToString(shared_ptr<TNode> root) {
    string to_return = "(";
    if (root->getValue() == "!") {
        to_return += "!";
        to_return += expressionToString(root->getLeftNode());
        return to_return + ")";
    }
    else if (root->getType() == DesignEntity::Op) {
        to_return += expressionToString(root->getLeftNode());
        to_return += root->getValue();
        to_return += expressionToString(root->getRightNode());
        return to_return + ")";
    } else {
        return root->getValue();
    }
}

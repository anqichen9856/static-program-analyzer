#include "catch.hpp"
#include "front_end/parser/Parser.h"
#include "front_end/tokenizer/Tokenizer.h"

TEST_CASE("Parse Procedure Test") {
    vector<Token> token_stream;
    Parser *parser = Parser::getInstance();

    SECTION ("Valid") {
        token_stream.emplace_back(Token("procedure", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("main", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("print", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("x", Token::TokenType::Identifier));
        token_stream.emplace_back(Token(";", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token::createEndToken());
        shared_ptr<TNode> root = parser->buildAST(token_stream);

        REQUIRE(root->getValue() == "root");
        REQUIRE(root->getProcedures().size() == 1);
        REQUIRE(root->getProcedures()[0]->getValue() == "main");
    }

    SECTION ("2 procedures") {
        token_stream.emplace_back(Token("procedure", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("main", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("print", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("x", Token::TokenType::Identifier));
        token_stream.emplace_back(Token(";", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token("procedure", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("main2", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("print", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("x", Token::TokenType::Identifier));
        token_stream.emplace_back(Token(";", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token::createEndToken());
        shared_ptr<TNode> root = parser->buildAST(token_stream);

        REQUIRE(root->getValue() == "root");
        REQUIRE(root->getProcedures().size() == 2);
        REQUIRE(root->getProcedures()[0]->getValue() == "main");
        REQUIRE(root->getProcedures()[1]->getValue() == "main2");
    }

    SECTION("With 'keyword' procedure name") {
        token_stream.emplace_back(Token("procedure", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("if", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("print", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("x", Token::TokenType::Identifier));
        token_stream.emplace_back(Token(";", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token::createEndToken());
        shared_ptr<TNode> root = parser->buildAST(token_stream);
        REQUIRE_NOTHROW(parser->buildAST(token_stream), "Invalid Procedure Name: 'if'");
    }

    SECTION("Missing '{'") {
        token_stream.emplace_back(Token("procedure", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("main", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token::createEndToken());
        REQUIRE_THROWS_WITH(parser->buildAST(token_stream), "Invalid Procedure: expected '{', got '}'.");
    }


    SECTION("Missing '}'") {
        token_stream.emplace_back(Token("procedure", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("main", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token::createEndToken());
        REQUIRE_THROWS_WITH(parser->buildAST(token_stream), "Invalid statement starting with: 'EOF'.");
    }

    SECTION("Wrong 'procedure' identifier") {
        token_stream.emplace_back(Token("Procedure", Token::TokenType::Keyword));
        REQUIRE_THROWS_WITH(parser->buildAST(token_stream),
                            "Invalid Procedure: expected 'procedure', got 'Procedure'.");
    }

    SECTION("'constant' procedure name") {
        token_stream.emplace_back(Token("procedure", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token::createEndToken());
        REQUIRE_THROWS_WITH(parser->buildAST(token_stream), "Invalid Procedure: expected procedure name, got '1'.");
    }
}

TEST_CASE("Parse Call Statement") {
    vector<Token> token_stream;
    Parser *parser = Parser::getInstance();
    SECTION ("Valid Statement") {
        token_stream.emplace_back(Token("procedure", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("main", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("call", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("Lol", Token::TokenType::Identifier));
        token_stream.emplace_back(Token(";", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token::createEndToken());
        auto node = parser->buildAST(token_stream);
        REQUIRE(node->getProcedures()[0]->getStatementList()[0]->getType() == DesignEntity::Call);
        REQUIRE(node->getProcedures()[0]->getStatementList()[0]->getVariable()->getValue() == "Lol");
    }

    SECTION ("Missing ;") {
        token_stream.emplace_back(Token("procedure", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("main", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("call", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("Lol", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token::createEndToken());
        REQUIRE_THROWS_WITH(parser->buildAST(token_stream),"Invalid Call Statement: expected ';', got '}'.");
    }

}

TEST_CASE("Parse Statement Test") {
    vector<Token> token_stream;
    Parser *parser = Parser::getInstance();
    SECTION ("Invalid Statement") {
        token_stream.emplace_back(Token("procedure", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("main", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("test", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token::createEndToken());
        REQUIRE_THROWS_WITH(parser->buildAST(token_stream), "Invalid statement starting with: 'test'.");
    }
}

TEST_CASE("Parse Read Statement Test") {
    vector<Token> token_stream;
    Parser *parser = Parser::getInstance();
    SECTION ("Valid") {
        token_stream.emplace_back(Token("procedure", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("main", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("read", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("x", Token::TokenType::Identifier));
        token_stream.emplace_back(Token(";", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token::createEndToken());
        shared_ptr<TNode> node = parser->buildAST(token_stream);
        node = node->getProcedures()[0];
        REQUIRE(node->getStatementList().size() == 1);
        REQUIRE(node->getStatementList()[0]->getParent() != nullptr);
        REQUIRE(node->getStatementList()[0]->getValue() == "read");
        REQUIRE(node->getStatementList()[0]->getVariable()->getValue() == "x");
    }

    SECTION ("Valid With Long Variable") {
        token_stream.emplace_back(Token("procedure", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("main", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("read", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("a1234567891000023123124314515125151", Token::TokenType::Identifier));
        token_stream.emplace_back(Token(";", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token::createEndToken());
        shared_ptr<TNode> node = parser->buildAST(token_stream);
        node = node->getProcedures()[0];
        REQUIRE(node->getStatementList().size() == 1);
        REQUIRE(node->getStatementList()[0]->getParent() != nullptr);
        REQUIRE(node->getStatementList()[0]->getValue() == "read");
        REQUIRE(node->getStatementList()[0]->getVariable()->getValue() == "a1234567891000023123124314515125151");
    }

    SECTION ("Missing ;") {
        token_stream.emplace_back(Token("procedure", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("main", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("read", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("x", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token::createEndToken());
        REQUIRE_THROWS_WITH(parser->buildAST(token_stream), "Invalid Read Statement: expected ';', got '}'.");
    }

    SECTION ("Invalid variable") {
        token_stream.emplace_back(Token("procedure", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("main", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("read", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token(";", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token::createEndToken());
        REQUIRE_THROWS_WITH(parser->buildAST(token_stream), "Invalid Read Statement: expected variable, got '1'.");
    }
}

TEST_CASE("Parse Print Statement Test") {
    vector<Token> token_stream;
    Parser *parser = Parser::getInstance();
    SECTION ("Valid") {
        token_stream.emplace_back(Token("procedure", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("main", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("print", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("x", Token::TokenType::Identifier));
        token_stream.emplace_back(Token(";", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token::createEndToken());
        shared_ptr<TNode> node = parser->buildAST(token_stream);
        node = node->getProcedures()[0];
        REQUIRE(node->getStatementList().size() == 1);
        REQUIRE(node->getStatementList()[0]->getParent() != nullptr);
        REQUIRE(node->getStatementList()[0]->getValue() == "print");
        REQUIRE(node->getStatementList()[0]->getVariable()->getValue() == "x");
    }

    SECTION ("Valid with Long variable") {
        token_stream.emplace_back(Token("procedure", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("main", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("print", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("a1234567891000023123124314515125151", Token::TokenType::Identifier));
        token_stream.emplace_back(Token(";", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token::createEndToken());
        shared_ptr<TNode> node = parser->buildAST(token_stream);
        node = node->getProcedures()[0];
        REQUIRE(node->getStatementList().size() == 1);
        REQUIRE(node->getStatementList()[0]->getParent() != nullptr);
        REQUIRE(node->getStatementList()[0]->getValue() == "print");
        REQUIRE(node->getStatementList()[0]->getVariable()->getValue() == "a1234567891000023123124314515125151");
    }

    SECTION ("Missing ';'") {
        token_stream.emplace_back(Token("procedure", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("main", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("print", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("x", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token::createEndToken());
        REQUIRE_THROWS_WITH(parser->buildAST(token_stream), "Invalid Print Statement: expected ';', got '}'.");
    }

    SECTION ("Invalid variable") {
        token_stream.emplace_back(Token("procedure", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("main", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("print", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token(";", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token::createEndToken());
        REQUIRE_THROWS_WITH(parser->buildAST(token_stream), "Invalid Print Statement: expected variable, got '1'.");
    }
}

TEST_CASE("Parse Assign Test") {
    vector<Token> token_stream;
    Parser *parser = Parser::getInstance();
    SECTION ("Valid") {
        token_stream.emplace_back(Token("procedure", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("main", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("x", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("=", Token::TokenType::Operator));
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token(";", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token::createEndToken());
        shared_ptr<TNode> node = parser->buildAST(token_stream);
        node = node->getProcedures()[0];
        REQUIRE(node->getStatementList()[0]->getParent() != nullptr);
        REQUIRE(node->getStatementList()[0]->getValue() == "assign");
        REQUIRE(node->getStatementList()[0]->getVariable()->getValue() == "x");
        REQUIRE(node->getStatementList()[0]->getExpression()->getValue() == "1");
    }

    SECTION ("Valid with long numbers") {
        token_stream.emplace_back(Token("procedure", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("main", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("x", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("=", Token::TokenType::Operator));
        token_stream.emplace_back(Token("1234567891000023123124314515125151", Token::TokenType::Literal));
        token_stream.emplace_back(Token(";", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token::createEndToken());
        shared_ptr<TNode> node = parser->buildAST(token_stream);
        node = node->getProcedures()[0];
        REQUIRE(node->getStatementList()[0]->getParent() != nullptr);
        REQUIRE(node->getStatementList()[0]->getValue() == "assign");
        REQUIRE(node->getStatementList()[0]->getVariable()->getValue() == "x");
        REQUIRE(node->getStatementList()[0]->getExpression()->getValue() == "1234567891000023123124314515125151");
    }

    SECTION ("Missing ';'") {
        token_stream.emplace_back(Token("procedure", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("main", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("x", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("=", Token::TokenType::Operator));
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token::createEndToken());
        REQUIRE_THROWS_WITH(parser->buildAST(token_stream), "Invalid Assign Statement: expected ';', got '}'.");
    }

    SECTION ("Incorrect Variable") {
        token_stream.emplace_back(Token("procedure", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("main", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token("=", Token::TokenType::Operator));
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token(";", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token::createEndToken());
        REQUIRE_THROWS_WITH(parser->buildAST(token_stream), "Invalid Assign Statement: expected variable, got '1'.");
    }
}

TEST_CASE("Parse Expression Test") {
    vector<Token> token_stream;
    Parser *parser = Parser::getInstance();
    SECTION ("i") {
        token_stream.emplace_back(Token("i", Token::TokenType::Literal));
        token_stream.emplace_back(Token::createEndToken());
        shared_ptr<TNode> node = parser->buildExpression(token_stream);
        REQUIRE(node->getValue() == "i");
    }

    SECTION ("1+1") {
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token("+", Token::TokenType::Operator));
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token::createEndToken());
        shared_ptr<TNode> node = parser->buildExpression(token_stream);

        REQUIRE(node->getValue() == "+");
        REQUIRE(node->getLeftNode()->getValue() == "1");
        REQUIRE(node->getRightNode()->getValue() == "1");
    }

    SECTION ("(1+1)") {
        token_stream.emplace_back(Token("(", Token::TokenType::Separator));
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token("+", Token::TokenType::Operator));
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token(")", Token::TokenType::Separator));
        token_stream.emplace_back(Token::createEndToken());
        shared_ptr<TNode> node = parser->buildExpression(token_stream);

        REQUIRE(node->getValue() == "+");
        REQUIRE(node->getLeftNode()->getValue() == "1");
        REQUIRE(node->getRightNode()->getValue() == "1");
    }

    SECTION ("1+x") {
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token("+", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("x", Token::TokenType::Literal));
        token_stream.emplace_back(Token::createEndToken());
        shared_ptr<TNode> node = parser->buildExpression(token_stream);

        REQUIRE(node->getValue() == "+");
        REQUIRE(node->getLeftNode()->getValue() == "1");
        REQUIRE(node->getRightNode()->getValue() == "x");
    }

    SECTION ("1+2*3") {
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token("+", Token::TokenType::Operator));
        token_stream.emplace_back(Token("2", Token::TokenType::Literal));
        token_stream.emplace_back(Token("*", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("3", Token::TokenType::Literal));
        token_stream.emplace_back(Token::createEndToken());
        shared_ptr<TNode> node = parser->buildExpression(token_stream);

        REQUIRE(node->getValue() == "+");
        REQUIRE(node->getLeftNode()->getValue() == "1");
        REQUIRE(node->getRightNode()->getValue() == "*");
        REQUIRE(node->getRightNode()->getLeftNode()->getValue() == "2");
        REQUIRE(node->getRightNode()->getRightNode()->getValue() == "3");
    }

    SECTION ("(1+2)*3") {
        token_stream.emplace_back(Token("(", Token::TokenType::Separator));
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token("+", Token::TokenType::Operator));
        token_stream.emplace_back(Token("2", Token::TokenType::Literal));
        token_stream.emplace_back(Token(")", Token::TokenType::Separator));
        token_stream.emplace_back(Token("*", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("3", Token::TokenType::Literal));
        token_stream.emplace_back(Token::createEndToken());
        shared_ptr<TNode> node = parser->buildExpression(token_stream);

        REQUIRE(node->getValue() == "*");
        REQUIRE(node->getLeftNode()->getValue() == "+");
        REQUIRE(node->getRightNode()->getValue() == "3");

        REQUIRE(node->getLeftNode()->getLeftNode()->getValue() == "1");
        REQUIRE(node->getLeftNode()->getRightNode()->getValue() == "2");
    }

    SECTION ("1*(2+3)") {
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token("*", Token::TokenType::Operator));
        token_stream.emplace_back(Token("(", Token::TokenType::Separator));
        token_stream.emplace_back(Token("2", Token::TokenType::Literal));
        token_stream.emplace_back(Token("+", Token::TokenType::Operator));
        token_stream.emplace_back(Token("3", Token::TokenType::Literal));
        token_stream.emplace_back(Token(")", Token::TokenType::Separator));
        token_stream.emplace_back(Token::createEndToken());
        shared_ptr<TNode> node = parser->buildExpression(token_stream);

        REQUIRE(node->getValue() == "*");
        REQUIRE(node->getLeftNode()->getValue() == "1");
        REQUIRE(node->getRightNode()->getValue() == "+");
        REQUIRE(node->getRightNode()->getLeftNode()->getValue() == "2");
        REQUIRE(node->getRightNode()->getRightNode()->getValue() == "3");
    }

    SECTION ("1*(2+3)*3") {
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token("*", Token::TokenType::Operator));
        token_stream.emplace_back(Token("(", Token::TokenType::Separator));
        token_stream.emplace_back(Token("2", Token::TokenType::Literal));
        token_stream.emplace_back(Token("+", Token::TokenType::Operator));
        token_stream.emplace_back(Token("3", Token::TokenType::Literal));
        token_stream.emplace_back(Token(")", Token::TokenType::Separator));
        token_stream.emplace_back(Token("*", Token::TokenType::Operator));
        token_stream.emplace_back(Token("3", Token::TokenType::Literal));
        token_stream.emplace_back(Token::createEndToken());
        shared_ptr<TNode> node = parser->buildExpression(token_stream);
        REQUIRE(node->getValue() == "*");
        REQUIRE(node->getLeftNode()->getValue() == "*");
        REQUIRE(node->getRightNode()->getValue() == "3");
        REQUIRE(node->getLeftNode()->getLeftNode()->getValue() == "1");
        REQUIRE(node->getLeftNode()->getRightNode()->getValue() == "+");
        REQUIRE(node->getLeftNode()->getRightNode()->getLeftNode()->getValue() == "2");
        REQUIRE(node->getLeftNode()->getRightNode()->getRightNode()->getValue() == "3");
    }

    SECTION ("1<x") {
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token("<", Token::TokenType::Operator));
        token_stream.emplace_back(Token("x", Token::TokenType::Identifier));
        token_stream.emplace_back(Token::createEndToken());
        REQUIRE_THROWS_WITH(parser->buildExpression(token_stream), "Invalid Expression Statement: expected valid expression operator, got: '<'.");
    }

    SECTION ("(1+x") {
        token_stream.emplace_back(Token("(", Token::TokenType::Separator));
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token("+", Token::TokenType::Operator));
        token_stream.emplace_back(Token("x", Token::TokenType::Identifier));
        token_stream.emplace_back(Token::createEndToken());
        REQUIRE_THROWS_WITH(parser->buildExpression(token_stream), "Invalid Expression Statement: expected ')', got: 'EOF'.");
    }

    SECTION ("(1+x))") {
        token_stream.emplace_back(Token("(", Token::TokenType::Separator));
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token("+", Token::TokenType::Operator));
        token_stream.emplace_back(Token("x", Token::TokenType::Identifier));
        token_stream.emplace_back(Token(")", Token::TokenType::Separator));
        token_stream.emplace_back(Token(")", Token::TokenType::Separator));
        token_stream.emplace_back(Token::createEndToken());
        REQUIRE_THROWS_WITH(parser->buildExpression(token_stream), "Invalid Expression Statement: expected valid expression operator, got: ')'.");
    }

    SECTION ("1++") {
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token("+", Token::TokenType::Operator));
        token_stream.emplace_back(Token("+", Token::TokenType::Operator));
        token_stream.emplace_back(Token::createEndToken());

        REQUIRE_THROWS_WITH(parser->buildExpression(token_stream),
                            "Invalid Expression Statement: expected expression constant or variable, got: '+'.");
    }

    SECTION ("1=2") {
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token("=", Token::TokenType::Operator));
        token_stream.emplace_back(Token("2", Token::TokenType::Operator));
        token_stream.emplace_back(Token::createEndToken());

        REQUIRE_THROWS_WITH(parser->buildExpression(token_stream),
                            "Invalid Expression Statement: expected valid expression operator, got: '='.");
    }

    SECTION ("1&&2") {
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token("&&", Token::TokenType::Operator));
        token_stream.emplace_back(Token("2", Token::TokenType::Operator));
        token_stream.emplace_back(Token::createEndToken());

        REQUIRE_THROWS_WITH(parser->buildExpression(token_stream),
                            "Invalid Expression Statement: expected valid expression operator, got: '&&'.");
    }
}


TEST_CASE("Parse If Statement") {
    vector<Token> token_stream;
    Parser *parser = Parser::getInstance();
    SECTION ("Empty") {
        token_stream.emplace_back(Token("procedure", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("main", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("if", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("(", Token::TokenType::Separator));
        token_stream.emplace_back(Token("i", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("==", Token::TokenType::Operator));
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token(")", Token::TokenType::Separator));
        token_stream.emplace_back(Token("then", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("print", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("x", Token::TokenType::Identifier));
        token_stream.emplace_back(Token(";", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token("else", Token::TokenType::Separator));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("print", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("x", Token::TokenType::Identifier));
        token_stream.emplace_back(Token(";", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token::createEndToken());
        shared_ptr<TNode> node = parser->buildAST(token_stream);
        REQUIRE(node->getProcedures()[0]->getStatementList()[0]->getParent() != nullptr);
        REQUIRE(node->getProcedures()[0]->getStatementList()[0]->getValue() == "if");
        REQUIRE(node->getProcedures()[0]->getStatementList()[0]->getExpression()->getValue() == "==");
        REQUIRE(node->getProcedures()[0]->getStatementList()[0]->getExpression()->getLeftNode()->getValue() == "i");
        REQUIRE(node->getProcedures()[0]->getStatementList()[0]->getExpression()->getRightNode()->getValue() == "1");
    }

    SECTION ("With Statements") {
        token_stream.emplace_back(Token("procedure", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("main", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("if", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("(", Token::TokenType::Separator));
        token_stream.emplace_back(Token("i", Token::TokenType::Identifier));
        token_stream.emplace_back(Token(">=", Token::TokenType::Operator));
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token(")", Token::TokenType::Separator));
        token_stream.emplace_back(Token("then", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("read", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("x", Token::TokenType::Identifier));
        token_stream.emplace_back(Token(";", Token::TokenType::Separator));
        token_stream.emplace_back(Token("print", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("x", Token::TokenType::Identifier));
        token_stream.emplace_back(Token(";", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token("else", Token::TokenType::Separator));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("read", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("y", Token::TokenType::Identifier));
        token_stream.emplace_back(Token(";", Token::TokenType::Separator));
        token_stream.emplace_back(Token("print", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("y", Token::TokenType::Identifier));
        token_stream.emplace_back(Token(";", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token::createEndToken());
        shared_ptr<TNode> node = parser->buildAST(token_stream);
        node = node->getProcedures()[0]->getStatementList()[0];
        REQUIRE(node->getStatementList()[0]->getParent()->getValue() == "if");
        REQUIRE(node->getStatementList()[0]->getValue() == "read");
        REQUIRE(node->getStatementList()[1]->getValue() == "print");
        REQUIRE(node->getStatementList()[1]->getParent()->getValue() == "if");
        REQUIRE(node->getElseStatementList()[0]->getParent()->getValue() == "if");
        REQUIRE(node->getElseStatementList()[1]->getParent()->getValue() == "if");
        REQUIRE(node->getElseStatementList()[0]->getValue() == "read");
        REQUIRE(node->getElseStatementList()[1]->getValue() == "print");
    }

    SECTION ("Missing '(' in conditional statement") {
        token_stream.emplace_back(Token("procedure", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("main", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("if", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("i", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("==", Token::TokenType::Operator));
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token(")", Token::TokenType::Separator));
        token_stream.emplace_back(Token("then", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token("else", Token::TokenType::Separator));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token::createEndToken());
        REQUIRE_THROWS_WITH(parser->buildAST(token_stream), "Invalid If Statement: expected '(', got 'i'.");
    }

    SECTION ("Missing ')' in conditional statement") {
        token_stream.emplace_back(Token("procedure", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("main", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("if", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("(", Token::TokenType::Separator));
        token_stream.emplace_back(Token("i", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("==", Token::TokenType::Operator));
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token("then", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token("else", Token::TokenType::Separator));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token::createEndToken());
        REQUIRE_THROWS_WITH(parser->buildAST(token_stream), "Invalid If Statement: expected ')', got 'then'.");
    }

    SECTION ("Missing Else: if(x==1){}") {
        token_stream.emplace_back(Token("procedure", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("main", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("if", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("(", Token::TokenType::Separator));
        token_stream.emplace_back(Token("i", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("==", Token::TokenType::Operator));
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token(")", Token::TokenType::Separator));
        token_stream.emplace_back(Token("then", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("print", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("x", Token::TokenType::Identifier));
        token_stream.emplace_back(Token(";", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token::createEndToken());
        REQUIRE_THROWS_WITH(parser->buildAST(token_stream), "Invalid If Statement: expected 'else', got '}'.");
    }

    SECTION ("Missing (: ifi==1)") {
        token_stream.emplace_back(Token("procedure", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("main", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("if", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("(", Token::TokenType::Separator));
        token_stream.emplace_back(Token("i", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("==", Token::TokenType::Operator));
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token(")", Token::TokenType::Separator));
        token_stream.emplace_back(Token(")", Token::TokenType::Separator));
        token_stream.emplace_back(Token("then", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token("else", Token::TokenType::Separator));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token::createEndToken());
        REQUIRE_THROWS_WITH(parser->buildAST(token_stream), "Invalid If Statement: expected 'then', got ')'.");
    }

    SECTION ("Missing ): if(i==1") {
        token_stream.emplace_back(Token("(", Token::TokenType::Separator));
        token_stream.emplace_back(Token("i", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("==", Token::TokenType::Operator));
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token::createEndToken());

        REQUIRE_THROWS_WITH(parser->buildCondExpression(token_stream),
                            "Invalid Conditional Expression Statement: expected ')', got: 'EOF'.");
    }
}

TEST_CASE("Parse While Statement") {
    vector<Token> token_stream;
    Parser *parser = Parser::getInstance();
    SECTION ("Empty") {
        token_stream.emplace_back(Token("procedure", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("main", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("while", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("(", Token::TokenType::Separator));
        token_stream.emplace_back(Token("i", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("==", Token::TokenType::Operator));
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token(")", Token::TokenType::Separator));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("print", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("x", Token::TokenType::Identifier));
        token_stream.emplace_back(Token(";", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token::createEndToken());
        shared_ptr<TNode> node = parser->buildAST(token_stream);
        node = node->getProcedures()[0];
        REQUIRE(node->getStatementList()[0]->getValue() == "while");
        REQUIRE(node->getStatementList()[0]->getExpression()->getValue() == "==");
        REQUIRE(node->getStatementList()[0]->getExpression()->getLeftNode()->getValue() == "i");
        REQUIRE(node->getStatementList()[0]->getExpression()->getRightNode()->getValue() == "1");
    }

    SECTION ("With Statements") {
        token_stream.emplace_back(Token("procedure", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("main", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("while", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("(", Token::TokenType::Separator));
        token_stream.emplace_back(Token("i", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("==", Token::TokenType::Operator));
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token(")", Token::TokenType::Separator));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("read", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("x", Token::TokenType::Identifier));
        token_stream.emplace_back(Token(";", Token::TokenType::Separator));
        token_stream.emplace_back(Token("print", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("x", Token::TokenType::Identifier));
        token_stream.emplace_back(Token(";", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token::createEndToken());
        shared_ptr<TNode> node = parser->buildAST(token_stream);
        node = node->getProcedures()[0]->getStatementList()[0];
        REQUIRE(node->getStatementList()[0]->getValue() == "read");
        REQUIRE(node->getStatementList()[1]->getValue() == "print");
        REQUIRE(node->getStatementList()[0]->getParent()->getValue() == "while");
        REQUIRE(node->getStatementList()[1]->getParent()->getValue() == "while");
    }

    SECTION ("Missing '(' in conditional statement") {
        token_stream.emplace_back(Token("procedure", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("main", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("while", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("i", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("==", Token::TokenType::Operator));
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token(")", Token::TokenType::Separator));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token::createEndToken());
        REQUIRE_THROWS_WITH(parser->buildAST(token_stream), "Invalid While Statement: expected '(', got 'i'.");
    }

    SECTION ("Missing ')' in conditional statement") {
        token_stream.emplace_back(Token("procedure", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("main", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("while", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("(", Token::TokenType::Separator));
        token_stream.emplace_back(Token("i", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("==", Token::TokenType::Operator));
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token::createEndToken());
        REQUIRE_THROWS_WITH(parser->buildAST(token_stream), "Invalid While Statement: expected ')', got '{'.");
    }
}

TEST_CASE("Parse Relative Statement Test") {
    vector<Token> token_stream;
    Parser *parser = Parser::getInstance();
    SECTION ("if(i==1)") {
        token_stream.emplace_back(Token("procedure", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("main", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("if", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("(", Token::TokenType::Separator));
        token_stream.emplace_back(Token("i", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("==", Token::TokenType::Operator));
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token(")", Token::TokenType::Separator));
        token_stream.emplace_back(Token("then", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("print", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("x", Token::TokenType::Identifier));
        token_stream.emplace_back(Token(";", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token("else", Token::TokenType::Separator));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("print", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("x", Token::TokenType::Identifier));
        token_stream.emplace_back(Token(";", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token::createEndToken());
        shared_ptr<TNode> node = parser->buildAST(token_stream);
        node = node->getProcedures()[0]->getStatementList()[0]->getExpression();
        REQUIRE(node->getValue() == "==");
        REQUIRE(node->getLeftNode()->getValue() == "i");
        REQUIRE(node->getRightNode()->getValue() == "1");
    }

    SECTION ("if(i<=1)") {
        token_stream.emplace_back(Token("procedure", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("main", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("if", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("(", Token::TokenType::Separator));
        token_stream.emplace_back(Token("i", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("<=", Token::TokenType::Operator));
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token(")", Token::TokenType::Separator));
        token_stream.emplace_back(Token("then", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("print", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("x", Token::TokenType::Identifier));
        token_stream.emplace_back(Token(";", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token("else", Token::TokenType::Separator));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("print", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("x", Token::TokenType::Identifier));
        token_stream.emplace_back(Token(";", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token::createEndToken());
        shared_ptr<TNode> node = parser->buildAST(token_stream);
        node = node->getProcedures()[0]->getStatementList()[0]->getExpression();
        REQUIRE(node->getValue() == "<=");
        REQUIRE(node->getLeftNode()->getValue() == "i");
        REQUIRE(node->getRightNode()->getValue() == "1");
    }

    SECTION ("if(i>=1)") {
        token_stream.emplace_back(Token("procedure", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("main", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("if", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("(", Token::TokenType::Separator));
        token_stream.emplace_back(Token("i", Token::TokenType::Identifier));
        token_stream.emplace_back(Token(">=", Token::TokenType::Operator));
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token(")", Token::TokenType::Separator));
        token_stream.emplace_back(Token("then", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("print", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("x", Token::TokenType::Identifier));
        token_stream.emplace_back(Token(";", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token("else", Token::TokenType::Separator));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("print", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("x", Token::TokenType::Identifier));
        token_stream.emplace_back(Token(";", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token::createEndToken());
        shared_ptr<TNode> node = parser->buildAST(token_stream);
        node = node->getProcedures()[0]->getStatementList()[0]->getExpression();
        REQUIRE(node->getValue() == ">=");
        REQUIRE(node->getLeftNode()->getValue() == "i");
        REQUIRE(node->getRightNode()->getValue() == "1");
    }

    SECTION ("if(i>1)") {
        token_stream.emplace_back(Token("procedure", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("main", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("if", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("(", Token::TokenType::Separator));
        token_stream.emplace_back(Token("i", Token::TokenType::Identifier));
        token_stream.emplace_back(Token(">", Token::TokenType::Operator));
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token(")", Token::TokenType::Separator));
        token_stream.emplace_back(Token("then", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("print", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("x", Token::TokenType::Identifier));
        token_stream.emplace_back(Token(";", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token("else", Token::TokenType::Separator));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("print", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("x", Token::TokenType::Identifier));
        token_stream.emplace_back(Token(";", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token::createEndToken());
        shared_ptr<TNode> node = parser->buildAST(token_stream);
        node = node->getProcedures()[0]->getStatementList()[0]->getExpression();
        REQUIRE(node->getValue() == ">");
        REQUIRE(node->getLeftNode()->getValue() == "i");
        REQUIRE(node->getRightNode()->getValue() == "1");
    }

    SECTION ("if(i<1)") {
        token_stream.emplace_back(Token("procedure", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("main", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("if", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("(", Token::TokenType::Separator));
        token_stream.emplace_back(Token("i", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("<", Token::TokenType::Operator));
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token(")", Token::TokenType::Separator));
        token_stream.emplace_back(Token("then", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("print", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("x", Token::TokenType::Identifier));
        token_stream.emplace_back(Token(";", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token("else", Token::TokenType::Separator));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("print", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("x", Token::TokenType::Identifier));
        token_stream.emplace_back(Token(";", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token::createEndToken());
        shared_ptr<TNode> node = parser->buildAST(token_stream);
        node = node->getProcedures()[0]->getStatementList()[0]->getExpression();
        REQUIRE(node->getValue() == "<");
        REQUIRE(node->getLeftNode()->getValue() == "i");
        REQUIRE(node->getRightNode()->getValue() == "1");
    }

    SECTION ("if(i!=1)") {
        token_stream.emplace_back(Token("procedure", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("main", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("if", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("(", Token::TokenType::Separator));
        token_stream.emplace_back(Token("i", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("!=", Token::TokenType::Operator));
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token(")", Token::TokenType::Separator));
        token_stream.emplace_back(Token("then", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("print", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("x", Token::TokenType::Identifier));
        token_stream.emplace_back(Token(";", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token("else", Token::TokenType::Separator));
        token_stream.emplace_back(Token("{", Token::TokenType::Separator));
        token_stream.emplace_back(Token("print", Token::TokenType::Keyword));
        token_stream.emplace_back(Token("x", Token::TokenType::Identifier));
        token_stream.emplace_back(Token(";", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token("}", Token::TokenType::Separator));
        token_stream.emplace_back(Token::createEndToken());
        shared_ptr<TNode> node = parser->buildAST(token_stream);
        node = node->getProcedures()[0]->getStatementList()[0]->getExpression();
        REQUIRE(node->getValue() == "!=");
        REQUIRE(node->getLeftNode()->getValue() == "i");
        REQUIRE(node->getRightNode()->getValue() == "1");
    }

}

TEST_CASE("Parse Conditional Statement Test") {
    vector<Token> token_stream;
    Parser *parser = Parser::getInstance();
    SECTION ("i==1") {
        token_stream.emplace_back(Token("i", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("==", Token::TokenType::Operator));
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token::createEndToken());
        shared_ptr<TNode> node = parser->buildCondExpression(token_stream);
        REQUIRE(node->getValue() == "==");
        REQUIRE(node->getLeftNode()->getValue() == "i");
        REQUIRE(node->getRightNode()->getValue() == "1");
    }

    SECTION ("!(i==1)") {
        token_stream.emplace_back(Token("!", Token::TokenType::Operator));
        token_stream.emplace_back(Token("(", Token::TokenType::Separator));
        token_stream.emplace_back(Token("i", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("==", Token::TokenType::Operator));
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token(")", Token::TokenType::Separator));
        token_stream.emplace_back(Token::createEndToken());
        shared_ptr<TNode> node = parser->buildCondExpression(token_stream);
        REQUIRE(node->getValue() == "!");
        REQUIRE(node->getLeftNode()->getValue() == "==");
        REQUIRE(node->getLeftNode()->getLeftNode()->getValue() == "i");
        REQUIRE(node->getLeftNode()->getRightNode()->getValue() == "1");
    }

    SECTION ("if((i==1) && (x < 1))") {

        token_stream.emplace_back(Token("(", Token::TokenType::Separator));
        token_stream.emplace_back(Token("i", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("==", Token::TokenType::Operator));
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token(")", Token::TokenType::Separator));
        token_stream.emplace_back(Token("&&", Token::TokenType::Operator));
        token_stream.emplace_back(Token("(", Token::TokenType::Separator));
        token_stream.emplace_back(Token("x", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("<", Token::TokenType::Operator));
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token(")", Token::TokenType::Separator));
        token_stream.emplace_back(Token::createEndToken());
        shared_ptr<TNode> node = parser->buildCondExpression(token_stream);
        REQUIRE(node->getValue() == "&&");
        REQUIRE(node->getLeftNode()->getValue() == "==");
        REQUIRE(node->getLeftNode()->getLeftNode()->getValue() == "i");
        REQUIRE(node->getLeftNode()->getRightNode()->getValue() == "1");
        REQUIRE(node->getRightNode()->getValue() == "<");
        REQUIRE(node->getRightNode()->getLeftNode()->getValue() == "x");
        REQUIRE(node->getRightNode()->getRightNode()->getValue() == "1");
    }

    SECTION ("if((i+1==1/2) && (x-1 < 1+3))") {
        token_stream.emplace_back(Token("(", Token::TokenType::Separator));
        token_stream.emplace_back(Token("i", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("+", Token::TokenType::Operator));
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token("==", Token::TokenType::Operator));
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token("/", Token::TokenType::Operator));
        token_stream.emplace_back(Token("2", Token::TokenType::Literal));
        token_stream.emplace_back(Token(")", Token::TokenType::Separator));
        token_stream.emplace_back(Token("&&", Token::TokenType::Operator));
        token_stream.emplace_back(Token("(", Token::TokenType::Separator));
        token_stream.emplace_back(Token("x", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("-", Token::TokenType::Operator));
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token("<", Token::TokenType::Operator));
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token("+", Token::TokenType::Operator));
        token_stream.emplace_back(Token("3", Token::TokenType::Literal));
        token_stream.emplace_back(Token(")", Token::TokenType::Separator));
        token_stream.emplace_back(Token::createEndToken());
        shared_ptr<TNode> node = parser->buildCondExpression(token_stream);
        REQUIRE(node->getValue() == "&&");
        REQUIRE(node->getLeftNode()->getValue() == "==");
        REQUIRE(node->getLeftNode()->getLeftNode()->getValue() == "+");
        REQUIRE(node->getLeftNode()->getLeftNode()->getLeftNode()->getValue() == "i");
        REQUIRE(node->getLeftNode()->getLeftNode()->getRightNode()->getValue() == "1");
        REQUIRE(node->getLeftNode()->getRightNode()->getValue() == "/");
        REQUIRE(node->getLeftNode()->getRightNode()->getLeftNode()->getValue() == "1");
        REQUIRE(node->getLeftNode()->getRightNode()->getRightNode()->getValue() == "2");
        REQUIRE(node->getRightNode()->getValue() == "<");
        REQUIRE(node->getRightNode()->getLeftNode()->getValue() == "-");
        REQUIRE(node->getRightNode()->getLeftNode()->getLeftNode()->getValue() == "x");
        REQUIRE(node->getRightNode()->getLeftNode()->getRightNode()->getValue() == "1");
        REQUIRE(node->getRightNode()->getRightNode()->getValue() == "+");
        REQUIRE(node->getRightNode()->getRightNode()->getLeftNode()->getValue() == "1");
        REQUIRE(node->getRightNode()->getRightNode()->getRightNode()->getValue() == "3");
    }

    SECTION ("((i==1) && (x<2)) || (z>3)") {
        token_stream.emplace_back(Token("(", Token::TokenType::Separator));
        token_stream.emplace_back(Token("(", Token::TokenType::Separator));
        token_stream.emplace_back(Token("i", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("==", Token::TokenType::Operator));
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token(")", Token::TokenType::Separator));
        token_stream.emplace_back(Token("&&", Token::TokenType::Operator));
        token_stream.emplace_back(Token("(", Token::TokenType::Separator));
        token_stream.emplace_back(Token("x", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("<", Token::TokenType::Operator));
        token_stream.emplace_back(Token("2", Token::TokenType::Literal));
        token_stream.emplace_back(Token(")", Token::TokenType::Separator));
        token_stream.emplace_back(Token(")", Token::TokenType::Separator));
        token_stream.emplace_back(Token("||", Token::TokenType::Operator));
        token_stream.emplace_back(Token("(", Token::TokenType::Separator));
        token_stream.emplace_back(Token("z", Token::TokenType::Identifier));
        token_stream.emplace_back(Token(">", Token::TokenType::Operator));
        token_stream.emplace_back(Token("3", Token::TokenType::Literal));
        token_stream.emplace_back(Token(")", Token::TokenType::Separator));
        token_stream.emplace_back(Token::createEndToken());
        shared_ptr<TNode> node = parser->buildCondExpression(token_stream);
        REQUIRE(node->getValue() == "||");
        REQUIRE(node->getLeftNode()->getValue() == "&&");
        REQUIRE(node->getLeftNode()->getLeftNode()->getValue() == "==");
        REQUIRE(node->getLeftNode()->getRightNode()->getValue() == "<");
        REQUIRE(node->getRightNode()->getValue() == ">");
        REQUIRE(node->getRightNode()->getLeftNode()->getValue() == "z");
        REQUIRE(node->getRightNode()->getRightNode()->getValue() == "3");
    }

    SECTION ("(i)==(1)") {
        token_stream.emplace_back(Token("(", Token::TokenType::Separator));
        token_stream.emplace_back(Token("i", Token::TokenType::Identifier));
        token_stream.emplace_back(Token(")", Token::TokenType::Separator));
        token_stream.emplace_back(Token("==", Token::TokenType::Operator));
        token_stream.emplace_back(Token("(", Token::TokenType::Separator));
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token(")", Token::TokenType::Separator));
        token_stream.emplace_back(Token::createEndToken());
        shared_ptr<TNode> node = parser->buildCondExpression(token_stream);
        REQUIRE(node->getValue() == "==");
        REQUIRE(node->getLeftNode()->getValue() == "i");
        REQUIRE(node->getRightNode()->getValue() == "1");

    }

    SECTION ("(i+1)+1==(1)") {
        token_stream.emplace_back(Token("(", Token::TokenType::Separator));
        token_stream.emplace_back(Token("i", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("+", Token::TokenType::Operator));
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token(")", Token::TokenType::Separator));
        token_stream.emplace_back(Token("+", Token::TokenType::Operator));
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token("==", Token::TokenType::Operator));
        token_stream.emplace_back(Token("(", Token::TokenType::Separator));
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token(")", Token::TokenType::Separator));
        token_stream.emplace_back(Token::createEndToken());
        shared_ptr<TNode> node = parser->buildCondExpression(token_stream);

        REQUIRE(node->getValue() == "==");
        REQUIRE(node->getLeftNode()->getValue() == "+");
        REQUIRE(node->getLeftNode()->getLeftNode()->getValue() == "+");
        REQUIRE(node->getLeftNode()->getLeftNode()->getLeftNode()->getValue() == "i");
        REQUIRE(node->getLeftNode()->getLeftNode()->getRightNode()->getValue() == "1");
        REQUIRE(node->getLeftNode()->getRightNode()->getValue() == "1");
        REQUIRE(node->getRightNode()->getValue() == "1");
    }

    SECTION ("((i+1)+1==(1)) && (x<1)") {
        token_stream.emplace_back(Token("(", Token::TokenType::Separator));
        token_stream.emplace_back(Token("(", Token::TokenType::Separator));
        token_stream.emplace_back(Token("i", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("+", Token::TokenType::Operator));
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token(")", Token::TokenType::Separator));
        token_stream.emplace_back(Token("+", Token::TokenType::Operator));
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token("==", Token::TokenType::Operator));
        token_stream.emplace_back(Token("(", Token::TokenType::Separator));
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token(")", Token::TokenType::Separator));
        token_stream.emplace_back(Token(")", Token::TokenType::Separator));
        token_stream.emplace_back(Token("&&", Token::TokenType::Operator));
        token_stream.emplace_back(Token("(", Token::TokenType::Separator));
        token_stream.emplace_back(Token("x", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("<", Token::TokenType::Operator));
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token(")", Token::TokenType::Separator));
        token_stream.emplace_back(Token::createEndToken());
        shared_ptr<TNode> node = parser->buildCondExpression(token_stream);
        REQUIRE(node->getValue() == "&&");
        REQUIRE(node->getLeftNode()->getValue() == "==");
        REQUIRE(node->getLeftNode()->getLeftNode()->getValue() == "+");
        REQUIRE(node->getLeftNode()->getLeftNode()->getLeftNode()->getValue() == "+");
        REQUIRE(node->getLeftNode()->getLeftNode()->getLeftNode()->getLeftNode()->getValue() == "i");
        REQUIRE(node->getLeftNode()->getLeftNode()->getLeftNode()->getRightNode()->getValue() == "1");
        REQUIRE(node->getLeftNode()->getLeftNode()->getRightNode()->getValue() == "1");
        REQUIRE(node->getRightNode()->getValue() == "<");
        REQUIRE(node->getRightNode()->getLeftNode()->getValue() == "x");
        REQUIRE(node->getRightNode()->getRightNode()->getValue() == "1");
    }

    SECTION ("(!(i == 12)) && (j != 13)") {

        token_stream.emplace_back(Token("(", Token::TokenType::Separator));
        token_stream.emplace_back(Token("!", Token::TokenType::Operator));
        token_stream.emplace_back(Token("(", Token::TokenType::Separator));
        token_stream.emplace_back(Token("i", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("==", Token::TokenType::Operator));
        token_stream.emplace_back(Token("12", Token::TokenType::Literal));
        token_stream.emplace_back(Token(")", Token::TokenType::Separator));
        token_stream.emplace_back(Token(")", Token::TokenType::Separator));
        token_stream.emplace_back(Token("&&", Token::TokenType::Operator));
        token_stream.emplace_back(Token("(", Token::TokenType::Separator));
        token_stream.emplace_back(Token("j", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("!=", Token::TokenType::Operator));
        token_stream.emplace_back(Token("13", Token::TokenType::Literal));
        token_stream.emplace_back(Token(")", Token::TokenType::Separator));
        token_stream.emplace_back(Token::createEndToken());
        shared_ptr<TNode> node = parser->buildCondExpression(token_stream);
        REQUIRE(node->getValue() == "&&");
        REQUIRE(node->getLeftNode()->getValue() == "!");
        REQUIRE(node->getLeftNode()->getLeftNode()->getValue() == "==");
        REQUIRE(node->getLeftNode()->getRightNode() == nullptr);
        REQUIRE(node->getLeftNode()->getLeftNode()->getLeftNode()->getValue() == "i");
        REQUIRE(node->getLeftNode()->getLeftNode()->getRightNode()->getValue() == "12");
        REQUIRE(node->getRightNode()->getValue() == "!=");
        REQUIRE(node->getRightNode()->getLeftNode()->getValue() == "j");
        REQUIRE(node->getRightNode()->getRightNode()->getValue() == "13");
    }

    SECTION ("Long Conditionals") {
        token_stream.emplace_back(Token("1234567891000023123124314515125151", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("==", Token::TokenType::Operator));
        token_stream.emplace_back(Token("a1234567891000023123124314515125151", Token::TokenType::Literal));
        token_stream.emplace_back(Token::createEndToken());
        shared_ptr<TNode> node = parser->buildCondExpression(token_stream);
        REQUIRE(node->getValue() == "==");
        REQUIRE(node->getLeftNode()->getValue() == "1234567891000023123124314515125151");
        REQUIRE(node->getRightNode()->getValue() == "a1234567891000023123124314515125151");
    }

    SECTION ("Missing Relative Expression: x+1") {
        token_stream.emplace_back(Token("x", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("+", Token::TokenType::Operator));
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token::createEndToken());

        REQUIRE_THROWS_WITH(parser->buildCondExpression(token_stream),
                            "Invalid Conditional Expression Statement: expected relative expression comparator, got: 'EOF'.");
    }

    SECTION ("Missing conditional Expression:(i==1) or (x<1)") {
        token_stream.emplace_back(Token("(", Token::TokenType::Separator));
        token_stream.emplace_back(Token("i", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("==", Token::TokenType::Operator));
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token(")", Token::TokenType::Separator));
        token_stream.emplace_back(Token("or", Token::TokenType::Operator));
        token_stream.emplace_back(Token("(", Token::TokenType::Separator));
        token_stream.emplace_back(Token("x", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("<", Token::TokenType::Operator));
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token(")", Token::TokenType::Separator));
        token_stream.emplace_back(Token::createEndToken());

        REQUIRE_THROWS_WITH(parser->buildCondExpression(token_stream),
                            "Invalid Conditional Expression Statement: expected conditional operator, got: 'or'.");
    }

    SECTION ("Missing Brackets in presence of Conditional Operators: if(i==1 && x < 1)") {
        token_stream.emplace_back(Token("i", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("==", Token::TokenType::Operator));
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token("&&", Token::TokenType::Operator));
        token_stream.emplace_back(Token("x", Token::TokenType::Identifier));
        token_stream.emplace_back(Token("<", Token::TokenType::Operator));
        token_stream.emplace_back(Token("1", Token::TokenType::Literal));
        token_stream.emplace_back(Token::createEndToken());
        REQUIRE_THROWS_WITH(parser->buildCondExpression(token_stream), "Invalid Conditional Expression Statement: expected valid expression operator, got: '&&'.");
    }

}

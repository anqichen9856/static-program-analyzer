
#include "catch.hpp"
#include "front_end/tokenizer/Tokenizer.h"
#include "front_end/tokenizer/Token.h"
#include "exceptions/SPAException.h"

using namespace std;

void checkToken(vector<string> expectedTokenValue, vector<Token::TokenType> expectedTokenType, vector<Token> actualTokenStream) {

    if (expectedTokenValue.size() != actualTokenStream.size()) {
        REQUIRE_THROWS_WITH(expectedTokenValue.size() != actualTokenStream.size(), "Invalid token stream size.");
    } else {
        for(int i = 0; i < expectedTokenValue.size(); i++) {
            REQUIRE(expectedTokenValue[i] == actualTokenStream[i].getTokenValue());
            REQUIRE(expectedTokenType[i] == actualTokenStream[i].getTokenType());
        }

    }
}

TEST_CASE("Tokenize Source Test") {
    Tokenizer* tokenizer = Tokenizer::getInstance();

    SECTION("Valid Tokenization: Conditional, Rel expression and End Tokens") {
        string stmt1 = "x||y";
        string stmt2 = "(x<0)";
        string stmt3 = "(x<=0)";
        string stmt4 = "!(x>0)";

        vector<string> expected_stmt1_value = {"x","||","y","EOF"};
        vector<Token::TokenType> expected_stmt1_type = {Token::TokenType::Identifier, Token::TokenType::Operator,
                                                        Token::TokenType::Identifier, Token::TokenType::EndOfFile};


        vector<string> expected_stmt2_value = {"(","x","<","0",")", "EOF"};
        vector<Token::TokenType> expected_stmt2_type = {Token::TokenType::Separator, Token::TokenType::Identifier,
                                                        Token::TokenType::Operator, Token::TokenType::Literal,
                                                        Token::TokenType::Separator, Token::TokenType::EndOfFile};

        vector<string> expected_stmt3_value = {"(","x","<=","0",")", "EOF"};
        vector<Token::TokenType> expected_stmt3_type = {Token::TokenType::Separator, Token::TokenType::Identifier,
                                                        Token::TokenType::Operator, Token::TokenType::Literal,
                                                        Token::TokenType::Separator, Token::TokenType::EndOfFile};

        vector<string> expected_stmt4_value = {"!","(","x",">","0",")", "EOF"};
        vector<Token::TokenType> expected_stmt4_type = {Token::TokenType::Operator, Token::TokenType::Separator,
                                                        Token::TokenType::Identifier, Token::TokenType::Operator,
                                                        Token::TokenType::Literal, Token::TokenType::Separator,
                                                        Token::TokenType::EndOfFile};


        vector<Token> actualTokenStream1 = tokenizer->tokenizeSource(stmt1);
        vector<Token> actualTokenStream2 = tokenizer->tokenizeSource(stmt2);
        vector<Token> actualTokenStream3 = tokenizer->tokenizeSource(stmt3);
        vector<Token> actualTokenStream4 = tokenizer->tokenizeSource(stmt4);

        checkToken(expected_stmt1_value, expected_stmt1_type, actualTokenStream1);
        checkToken(expected_stmt2_value, expected_stmt2_type, actualTokenStream2);
        checkToken(expected_stmt3_value, expected_stmt3_type, actualTokenStream3);
        checkToken(expected_stmt4_value, expected_stmt4_type, actualTokenStream4);

    }

    SECTION("Valid Tokenization: Design entities")
    {
        string stmt1 = "if(){}else{}";
        string stmt2 = "while () {}";
        string stmt3 = "if(!(x==y)){}";
        string stmt4 = "procedure main{}";


        vector<string> expected_stmt1_value = {"if","(",")","{","}","else","{","}", "EOF"};
        vector<Token::TokenType> expected_stmt1_type = {Token::TokenType::Keyword, Token::TokenType::Separator,
                                                        Token::TokenType::Separator, Token::TokenType::Separator,
                                                        Token::TokenType::Separator, Token::TokenType::Keyword,
                                                        Token::TokenType::Separator, Token::TokenType::Separator,
                                                        Token::TokenType::EndOfFile};

        vector<string> expected_stmt2_value = {"while","(",")","{","}", "EOF"};
        vector<Token::TokenType> expected_stmt2_type = {Token::TokenType::Keyword, Token::TokenType::Separator,
                                                        Token::TokenType::Separator, Token::TokenType::Separator,
                                                        Token::TokenType::Separator,Token::TokenType::EndOfFile};

        vector<string> expected_stmt3_value = {"if","(","!","(","x","==","y",")",")","{","}", "EOF"};
        vector<Token::TokenType> expected_stmt3_type = {Token::TokenType::Keyword, Token::TokenType::Separator,
                                                        Token::TokenType::Operator, Token::TokenType::Separator,
                                                        Token::TokenType::Identifier, Token::TokenType::Operator,
                                                        Token::TokenType::Identifier, Token::TokenType::Separator,
                                                        Token::TokenType::Separator, Token::TokenType::Separator,
                                                        Token::TokenType::Separator, Token::TokenType::EndOfFile};

        vector<string> expected_stmt4_value = {"procedure", "main","{","}", "EOF"};
        vector<Token::TokenType> expected_stmt4_type = {Token::TokenType::Keyword, Token::TokenType::Identifier,
                                                        Token::TokenType::Separator, Token::TokenType::Separator,
                                                        Token::TokenType::EndOfFile};

        vector<Token> actualTokenStream1 = tokenizer->tokenizeSource(stmt1);
        vector<Token> actualTokenStream2 = tokenizer->tokenizeSource(stmt2);
        vector<Token> actualTokenStream3 = tokenizer->tokenizeSource(stmt3);
        vector<Token> actualTokenStream4 = tokenizer->tokenizeSource(stmt4);


        checkToken(expected_stmt1_value, expected_stmt1_type, actualTokenStream1);
        checkToken(expected_stmt2_value, expected_stmt2_type, actualTokenStream2);
        checkToken(expected_stmt3_value, expected_stmt3_type, actualTokenStream3);
        checkToken(expected_stmt4_value, expected_stmt4_type, actualTokenStream4);
    }

    SECTION ("Valid Tokenization: Math operation") {
        string stmt1 = "a + b - c / d * e % f";
        string stmt2 = "a+b-c/d*e%f";
        string stmt3 = "a +b- c/d* e% f";
        string stmt4 = "x=x+y;";

        vector<string> expected_stmt1_value = {"a","+", "b","-","c","/","d","*","e","%","f", "EOF"};
        vector<Token::TokenType> expected_stmt1_type = {Token::TokenType::Identifier, Token::TokenType::Operator,
                                                        Token::TokenType::Identifier, Token::TokenType::Operator,
                                                        Token::TokenType::Identifier, Token::TokenType::Operator,
                                                        Token::TokenType::Identifier, Token::TokenType::Operator,
                                                        Token::TokenType::Identifier, Token::TokenType::Operator,
                                                        Token::TokenType::Identifier, Token::TokenType::EndOfFile};

        vector<string> expected_stmt2_value = {"a","+", "b","-","c","/","d","*","e","%","f","EOF"};
        vector<Token::TokenType> expected_stmt2_type = {Token::TokenType::Identifier, Token::TokenType::Operator,
                                                        Token::TokenType::Identifier, Token::TokenType::Operator,
                                                        Token::TokenType::Identifier, Token::TokenType::Operator,
                                                        Token::TokenType::Identifier, Token::TokenType::Operator,
                                                        Token::TokenType::Identifier, Token::TokenType::Operator,
                                                        Token::TokenType::Identifier, Token::TokenType::EndOfFile};

        vector<string> expected_stmt3_value = {"a","+", "b","-","c","/","d","*","e","%","f","EOF"};
        vector<Token::TokenType> expected_stmt3_type = {Token::TokenType::Identifier, Token::TokenType::Operator,
                                                        Token::TokenType::Identifier, Token::TokenType::Operator,
                                                        Token::TokenType::Identifier, Token::TokenType::Operator,
                                                        Token::TokenType::Identifier, Token::TokenType::Operator,
                                                        Token::TokenType::Identifier, Token::TokenType::Operator,
                                                        Token::TokenType::Identifier, Token::TokenType::EndOfFile};

        vector<string> expected_stmt4_value = {"x","=","x","+","y",";","EOF"};
        vector<Token::TokenType> expected_stmt4_type = {Token::TokenType::Identifier, Token::TokenType::Operator,
                                                        Token::TokenType::Identifier, Token::TokenType::Operator,
                                                        Token::TokenType::Identifier, Token::TokenType::Separator,
                                                        Token::TokenType::EndOfFile};

        vector<Token> actualTokenStream1 = tokenizer->tokenizeSource(stmt1);
        vector<Token> actualTokenStream2 = tokenizer->tokenizeSource(stmt2);
        vector<Token> actualTokenStream3 = tokenizer->tokenizeSource(stmt3);
        vector<Token> actualTokenStream4 = tokenizer->tokenizeSource(stmt4);

        checkToken(expected_stmt1_value, expected_stmt1_type, actualTokenStream1);
        checkToken(expected_stmt2_value, expected_stmt2_type, actualTokenStream2);
        checkToken(expected_stmt3_value, expected_stmt3_type, actualTokenStream3);
        checkToken(expected_stmt4_value, expected_stmt4_type, actualTokenStream4);
    }

    SECTION("Invalid Tokenization: Empty File") {
        string emptySource;
        string exceptionMessage = "Invalid Tokenizer Exception! Nothing to tokenize. Expected: Non-empty or non-space source file, Received: " + emptySource;
        REQUIRE_THROWS_WITH(tokenizer->tokenizeSource(emptySource), exceptionMessage);

    }

    SECTION("Invalid Tokenization: File with spaces") {
        string emptySource = "              ";
        string exceptionMessage = "Invalid Tokenizer Exception! Nothing to tokenize. Expected: Non-empty or non-space source file, Received: " + emptySource;
        REQUIRE_THROWS_WITH(tokenizer->tokenizeSource(emptySource), exceptionMessage);
    }


}
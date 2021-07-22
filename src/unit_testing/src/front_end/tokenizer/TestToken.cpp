
#include "catch.hpp"
#include "front_end/tokenizer/Token.h"


TEST_CASE("Check Lexical Token") {
    SECTION("Valid Name") {
        string token1 = "variable";
        string token2 = "VARIABLE";
        string token3 = "VARIaBlE";
        string token4 = "var1";
        string token5 = "VAR2";

        Token actual_token1 = Token::createToken(token1, "");
        Token actual_token2 = Token::createToken(token2, "");
        Token actual_token3 = Token::createToken(token3, "");
        Token actual_token4 = Token::createToken(token4, "");
        Token actual_token5 = Token::createToken(token5, "");

        REQUIRE(token1 == actual_token1.getTokenValue());
        REQUIRE(token2 == actual_token2.getTokenValue());
        REQUIRE(token3 == actual_token3.getTokenValue());
        REQUIRE(token4 == actual_token4.getTokenValue());
        REQUIRE(token5 == actual_token5.getTokenValue());
    }

    SECTION("Valid Integer") {
        string token1 = "0";
        string token2 = "918";

        Token actual_token1 = Token::createToken(token1, "");
        Token actual_token2 = Token::createToken(token2, "");

        REQUIRE(token1 == actual_token1.getTokenValue());
        REQUIRE(token2 == actual_token2.getTokenValue());

    }

    SECTION("Invalid Name") {
        string token1 = "Var_1";
        string token2 = "1Var";

        string exception_message_1 = "Invalid Lexical Token Exception! Received Invalid Lexical Token: " + token1;
        string exception_message_2 = "Invalid Lexical Token Exception! Received Invalid Lexical Token: " + token2;

        REQUIRE_THROWS_WITH(Token::createToken(token1, ""), exception_message_1);
        REQUIRE_THROWS_WITH(Token::createToken(token2, ""), exception_message_2);
    }

    SECTION("Invalid Integer") {
        string token1 = "3.1412";

        string exception_message_1 = "Invalid Lexical Token Exception! Received Invalid Lexical Token: " + token1;

        REQUIRE_THROWS_WITH(Token::createToken(token1, ""), exception_message_1);
    }

}

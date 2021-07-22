#ifndef AUTOTESTER_TOKEN_H
#define AUTOTESTER_TOKEN_H

#pragma once

#include <vector>
#include <iostream>
#include <string>
#include <regex>

#include "utils/Constants.h"
#include "exceptions/SPAException.h"

using std::string;
using std::vector;
using std::regex_match;
using std::regex;

class Token {
public:
    enum TokenType {
        Identifier, Keyword, Separator, Operator, Literal, EndOfFile
    };

    Token(string value, TokenType type);

    Token();

    TokenType getTokenType();

    string getTokenValue();

    static Token createToken(string currToken, string prevToken);

    static Token createEndToken();

private:
    string tokenValue;
    TokenType tokenType;

    void setTokenType(string token, string prevToken);

    void setTokenValue(string token);

    static bool isValidLexicalToken(string currToken);

};

#endif //AUTOTESTER_TOKEN_H
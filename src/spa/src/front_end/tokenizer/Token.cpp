#include "Token.h"

Token::Token(string value, TokenType type) : tokenType(type), tokenValue(value) {}

Token::Token() {};

Token::TokenType Token::getTokenType() {
    return tokenType;
};

string Token::getTokenValue() {
    return tokenValue;
};

bool Token::isValidLexicalToken(string currToken) {
    regex regName(TokenNames::REGEX_NAME);
    regex regInteger(TokenNames::REGEX_INTEGER);

    if (regex_match(currToken, regName) || regex_match(currToken, regInteger)) {
        return true;
    }

    return false;
}

void Token::setTokenType(string currToken, string prevToken) {
    regex reg(TokenNames::REGEX_LITERAL);

    if (TokenNames::TOKEN_KEYWORD_SET.find(prevToken) != TokenNames::TOKEN_KEYWORD_SET.end()) {
        if (isValidLexicalToken(currToken)) {
            if (regex_match(currToken, reg)) {
                tokenType = Literal;
            } else {
                tokenType = Identifier;
            }
        } else if (TokenNames::TOKEN_OPERATOR_SET.find(currToken) != TokenNames::TOKEN_OPERATOR_SET.end()) {
            tokenType = Operator;
        } else if (TokenNames::TOKEN_SEPARATOR_SET.find(currToken) != TokenNames::TOKEN_SEPARATOR_SET.end()) {
            tokenType = Separator;
        } else {
            throw InvalidTokenizeException("Invalid Lexical Token Exception! Received Invalid Lexical Token: " + currToken);
        }
    } else {
        if (TokenNames::TOKEN_KEYWORD_SET.find(currToken) != TokenNames::TOKEN_KEYWORD_SET.end()) {
            tokenType = Keyword;
        } else if (TokenNames::TOKEN_SEPARATOR_SET.find(currToken) != TokenNames::TOKEN_SEPARATOR_SET.end()) {
            tokenType = Separator;
        } else if (TokenNames::TOKEN_OPERATOR_SET.find(currToken) != TokenNames::TOKEN_OPERATOR_SET.end()) {
            tokenType = Operator;
        } else if (regex_match(currToken, reg)) {
            tokenType = Literal;
        } else {
            if (isValidLexicalToken(currToken)) {
                tokenType = Identifier;
            } else {
                throw InvalidTokenizeException("Invalid Lexical Token Exception! Received Invalid Lexical Token: " + currToken);
            }
        }
    }
};

Token Token::createEndToken() {
    Token endToken;
    endToken.tokenType = EndOfFile;
    endToken.tokenValue = "EOF";

    return endToken;
}

void Token::setTokenValue(string token) {
    tokenValue = token;
};

Token Token::createToken(string currToken, string prevToken) {
    Token newToken;
    newToken.setTokenType(currToken, prevToken);
    newToken.setTokenValue(currToken);
    return newToken;
};


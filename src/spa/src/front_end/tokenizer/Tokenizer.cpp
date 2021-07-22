#include "Tokenizer.h"

Tokenizer::Tokenizer() {}

Tokenizer *Tokenizer::instance = nullptr;

Tokenizer *Tokenizer::getInstance() {
    if (instance == nullptr) {
        instance = new Tokenizer();
    }
    return instance;
}

vector<Token> Tokenizer::tokenizeSource(string sourceBuffer) {
    if (Tokenizer::isSpaceString(sourceBuffer)) {
        throw SPAException("Invalid Tokenizer Exception! Nothing to tokenize. "
                        "Expected: Non-empty or non-space source file, Received: " + sourceBuffer);
    }

    vector<Token> tokenStream = {};
    Token newToken;
    Token endToken;

    string currToken;
    string tempToken;
    string prevToken;

    int i = 0;

    while (i < sourceBuffer.size() - 1) {
        char currChar = sourceBuffer[i];
        char nextChar = sourceBuffer[i + 1];
        string currString(1, currChar);
        string nextString(1, nextChar);

        tempToken.push_back(currChar);
        tempToken.push_back(nextChar);

        if (tokenStream.size() > 0) {
            prevToken = tokenStream.back().getTokenValue();
        } else {
            prevToken = "";
        }

        if (TokenNames::TOKENS_SET_2.find(tempToken) != TokenNames::TOKENS_SET_2.end()) {
            // Token is an expression token
            if (currToken != "") {
                newToken = Token::createToken(currToken, prevToken);
                tokenStream.push_back(newToken);
            }
            newToken = Token::createToken(tempToken, prevToken);
            tokenStream.push_back(newToken);
            currToken = "";
            i += 2;
        } else if (TokenNames::TOKENS_SET_1.find(currString) != TokenNames::TOKENS_SET_1.end()
                   || TokenNames::TOKEN_SEPARATOR_SET.find(currString) != TokenNames::TOKEN_SEPARATOR_SET.end()) {
            // nextChar is an operation or end token
            newToken = Token::createToken(currString, prevToken);
            tokenStream.push_back(newToken);
            currToken = "";
            i += 1;
        } else if ((TokenNames::TOKENS_SET_1.find(nextString) != TokenNames::TOKENS_SET_1.end()
                    || TokenNames::TOKEN_SEPARATOR_SET.find(nextString) != TokenNames::TOKEN_SEPARATOR_SET.end())
                   && !isspace(currChar)) {
            // nextChar is an operation or end token
            currToken += currChar;
            newToken = Token::createToken(currToken, prevToken);
            tokenStream.push_back(newToken);
            currToken = "";
            i += 1;
        } else if (isspace(currChar) && currToken != "") {
            // currToken is a complete name
            newToken = Token::createToken(currToken, prevToken);
            tokenStream.push_back(newToken);
            currToken = "";
            i += 1;
        } else if (!isspace(currChar)) {
            currToken += currChar;
            i += 1;
        } else {
            i += 1;
        }
        tempToken = "";
    }

    if (!isspace(sourceBuffer[i])) {
        string currString(1, sourceBuffer[i]);
        currToken += currString;
        newToken = Token::createToken(currToken, prevToken);
        tokenStream.push_back(newToken);
    }

    endToken = Token::createEndToken();
    tokenStream.push_back(endToken);

    return tokenStream;
}

bool Tokenizer::isSpaceString(string token) {
    for (int i = 0; i < token.size(); i++) {
        if (!isspace(token[i])) {
            return false;
        }
    }

    return true;
}


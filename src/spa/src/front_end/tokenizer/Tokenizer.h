#ifndef AUTOTESTER_TOKENIZER_H
#define AUTOTESTER_TOKENIZER_H

#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_set>

#include "Token.h"
#include "utils/Constants.h"

class Tokenizer {
public:
    static Tokenizer *instance;

    vector<Token> tokenizeSource(string sourceBuffer);

    static Tokenizer *getInstance();

    explicit Tokenizer();

private:
    static bool isSpaceString(string token);
};

#endif //AUTOTESTER_TOKENIZER_H
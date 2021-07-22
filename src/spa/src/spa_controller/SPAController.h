#ifndef AUTOTESTER_SPACONTROLLER_H
#define AUTOTESTER_SPACONTROLLER_H

#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <regex>
#include <list>

#include "front_end/tokenizer/Tokenizer.h"
#include "front_end/tokenizer/Token.h"
#include "front_end/parser/Parser.h"
#include "query_processor/query_preprocessor/QueryPreProcessor.h"
#include "query_processor/query_evaluator/QueryEvaluator.h"
#include "query_processor/query_evaluator/QueryResult.h"
#include "query_processor/query_result_projector/QueryResultProjector.h"
#include "query_processor/query_optimizer/QueryOptimizer.h"


class SPAController {
public:
    static void runSPAController(string filename);

    static void evaluateQuery(string query_string, list<string> &results);

private:
    static string readProgram(string filename);
};

#endif //AUTOTESTER_SPACONTROLLER_H

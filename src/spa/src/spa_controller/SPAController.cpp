#include "SPAController.h"

void SPAController::runSPAController(string filename) {
    string sourceContent = readProgram(filename);
    Tokenizer* tokenizer = Tokenizer::getInstance();
    vector<Token> tokenStream = tokenizer->tokenizeSource(sourceContent);
    Parser *parser = Parser::getInstance();
    AST ast = parser->buildAST(tokenStream);
    PKB *pkb = PKB::getInstance();
    pkb->initAST(ast);
};

string SPAController::readProgram(std::string filename) {
    // Open filename and read the source content
    ifstream ifs(filename);
    if (ifs.fail()) {
        throw SPAException("Invalid File Exception! " + filename + " cannot be found.");
    }

    string sourceContent;

    ifs.seekg(0, std::ios::end);
    sourceContent.reserve(ifs.tellg());
    ifs.seekg(0, std::ios::beg);

    sourceContent.assign((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

    if (!ifs) {
        throw SPAException("Invalid File Exception! File could not be read");
    }

    return sourceContent;
};

void SPAController::evaluateQuery(string query_string, list<string> &results) {
    try {
        Query query = QueryPreProcessor::getInstance()->constructQuery(query_string);
        QueryResult* query_res = QueryResult::init();
        auto* qo = new QueryOptimizer(&query);
        qo->optimizeQuery();
        auto res = QueryEvaluator::getInstance()->evaluateWithOptimization(query, query_res);
        //auto res = QueryEvaluator::getInstance()->evaluateQuery(query, query_res);
        QueryResultProjector::getInstance()->formatResults(results, query_res, query, res);
    } catch (InvalidQueryException &e) {
        results = {};
        cout<<e.what();
    } catch (InvalidSemanticException &e) {
        results = {"FALSE"};
    } catch (InvalidParserException &e) {
        results = {};
        cout<<e.what();
    } catch (InvalidTokenizeException &e) {
        results = {};
        cout<<e.what();
    } catch (InvalidTokenException &e) {
        results = {};
        cout<<e.what();
    } catch (SPAException &e) {
        results = {};
        cout<<e.what();
    }
};


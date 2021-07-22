#ifndef AUTOTESTER_QUERYSYNTAXCHECKER_H
#define AUTOTESTER_QUERYSYNTAXCHECKER_H

#include <string>
#include <vector>
#include "query_processor/query_preprocessor/QueryParser.h"
#include "utils/Constants.h"

using namespace std;

class QuerySyntaxChecker {
public:

    static bool checkQueryStructure(string query_string);

    static string getValidDeclaration(string query_string);

    static string getValidSelected(string query_string);

    static vector<string> getValidRelClauseGroups(string query_string);

    static vector<string> getValidPatternClauseGroups(string query_string);

    static vector<string> getValidWithClauseGroups(string query_string);

    static bool hasRemaining(string query_string);

    static bool hasInvalidSuchThat(string query_string);
};


#endif //AUTOTESTER_QUERYSYNTAXCHECKER_H

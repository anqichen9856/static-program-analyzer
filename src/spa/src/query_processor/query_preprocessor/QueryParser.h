#ifndef AUTOTESTER_QUERYPARSER_H
#define AUTOTESTER_QUERYPARSER_H

#include <vector>
#include <string>
#include <regex>
#include "utils/Constants.h"

using namespace std;

class QueryParser {
public:
    static bool isMatched(string query_string, string regex_string);

    static string getMatched(string query_string, string regex_string);

    static vector<string> getMatchedMulti(string query_string, string regex_string);

    static vector<string> tokenizeBy(string str, string delimiter);

    static bool isSubString(string s, string sub);

    static string removeSpaces(string str);

    static string removeMatched(string str, string rgx);

    static bool isSynonym(string s);

    static bool isSynAttrPair(string s);

    static string getSyn(string s);

    static string getAttr(string s);

    static string removeFirstNWord(string s, int n);
};


#endif //AUTOTESTER_QUERYPARSER_H

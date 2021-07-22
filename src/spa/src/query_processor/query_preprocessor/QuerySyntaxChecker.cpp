#include "QuerySyntaxChecker.h"
#include <iostream>

bool QuerySyntaxChecker::checkQueryStructure(string query_string) {
    return QueryParser::isMatched(query_string, REGEX_QUERY);
}

string QuerySyntaxChecker::getValidDeclaration(string query_string) {
    return QueryParser::getMatched(query_string, REGEX_DECL);
}

string QuerySyntaxChecker::getValidSelected(string query_string) {
    string select = QueryParser::getMatched(query_string, REGEX_SELECT);
    return QueryParser::removeFirstNWord(select, 1);
}

vector<string> QuerySyntaxChecker::getValidRelClauseGroups(string query_string) {
    return QueryParser::getMatchedMulti(query_string, REGEX_REL_CLAUSE);
}

vector<string> QuerySyntaxChecker::getValidPatternClauseGroups(string query_string) {
    vector<string> res = QueryParser::getMatchedMulti(query_string, REGEX_PATTERN);
    for (int i = 0; i < res.size(); i++) {
        res[i] = QueryParser::removeMatched(res[i], REGEX_REL_CLAUSE);
        res[i] = QueryParser::removeMatched(res[i], REGEX_WITH);
    }
    return res;
}

vector<string> QuerySyntaxChecker::getValidWithClauseGroups(string query_string) {
    return QueryParser::getMatchedMulti(query_string, REGEX_WITH);
}

bool QuerySyntaxChecker::hasRemaining(string query_string) {
    string remove_decl = QueryParser::removeMatched(query_string, REGEX_DECL);
    string remove_select = QueryParser::removeMatched(remove_decl, REGEX_SELECT);
    string remove_rel = QueryParser::removeMatched(remove_select, REGEX_REL_CLAUSE);
    string remove_pttn = QueryParser::removeMatched(remove_rel, REGEX_PATTERN);
    string remove_with = QueryParser::removeMatched(remove_pttn, REGEX_WITH);
    string remaining = QueryParser::removeSpaces(remove_with);
    return !remaining.empty();
}

bool QuerySyntaxChecker::hasInvalidSuchThat(string query_string) {
    int valid_occurrences = QueryParser::getMatchedMulti(query_string, REGEX_VALID_SUCH_THAT).size();
    int general_occurrences = QueryParser::getMatchedMulti(query_string, REGEX_GENERAL_SUCH_THAT).size();
    return general_occurrences > valid_occurrences;
}
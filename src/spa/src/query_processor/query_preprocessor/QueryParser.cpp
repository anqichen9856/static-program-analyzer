#include "QueryParser.h"

bool QueryParser::isMatched(string query_string, string regex_string) {
    regex rgx(regex_string);
    return regex_match(query_string, rgx);
}

string QueryParser::getMatched(string query_string, string regex_string) {
    regex rgx(regex_string);
    auto begin = sregex_iterator(query_string.begin(), query_string.end(), rgx);
    auto end = sregex_iterator();
    string res;

    for (sregex_iterator k = begin; k != end; ++k) {
        smatch match = *k;
        res += match.str();
    }
    return res;
}

vector<string> QueryParser::getMatchedMulti(string query_string, string regex_string) {
    vector<string> results;
    string subject(query_string);
    smatch match;
    regex rgx(regex_string);
    int i = 1;
    while (regex_search(subject, match, rgx)) {
        results.push_back(match.str(0));
        i++;
        subject = match.suffix().str();
    }
    return results;
}

vector<string> QueryParser::tokenizeBy(string str, string delimiter) {

    int curr;
    int delim_len = delimiter.length();
    string token;
    vector<string> res;
    regex r(R"(\s*)");

    while ((curr = str.find(delimiter)) != string::npos) {
        token = str.substr(0, curr);
        if (!regex_match(token, r)) {
            res.push_back(token);
        }
        str.erase(0, curr + delim_len);
    }
    if (!regex_match(str, r)) {
        res.push_back(str);
    }
    return res;
}

bool QueryParser::isSubString(string s, string sub) {
    return s.find(sub) != string::npos;
}

string QueryParser::removeSpaces(string str) {
    str.erase(remove(str.begin(), str.end(), ' '), str.end());
    return str;
}


string QueryParser::removeMatched(string query_string, string regex_string) {
    regex rgx(regex_string);
    return regex_replace(query_string, rgx, "");
}

bool QueryParser::isSynonym(string s) {
    return isMatched(s, REGEX_SYNONYM);
}

bool QueryParser::isSynAttrPair(string s) {
    return isMatched(s, REGEX_SYN_ATTR);
}

string QueryParser::getSyn(string s) {
    return tokenizeBy(s, ".")[0];
}

string QueryParser::getAttr(string s) {
    return tokenizeBy(s, ".")[1];
}

string QueryParser::removeFirstNWord(string s, int n) {
    vector<string> tokens = QueryParser::tokenizeBy(s, " ");
    string rest = "";
    for (int i = n; i < tokens.size(); i++) {
        if (i != tokens.size() - 1) {
            rest += tokens[i] + " ";
        } else {
            rest += tokens[i];
        }
    }
    return rest;
}
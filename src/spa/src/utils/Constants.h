#ifndef AUTOTESTER_CONSTANTS_H
#define AUTOTESTER_CONSTANTS_H

#include <string>
#include <unordered_set>

using namespace std;

namespace {

    // for Query syntax checking
    const string REGEX_QUERY = R"((\s*(stmt|read|print|call|while|if|assign|variable|constant|procedure|prog_line)\s+[A-Za-z][A-Za-z0-9]*\s*(\s*,\s*[A-Za-z][A-Za-z0-9]*)*\s*;\s*)*\s*Select\s+(BOOLEAN|([A-Za-z][A-Za-z0-9]*.(procName|varName|value|stmt#)|[A-Za-z][A-Za-z0-9]*)|<\s*([A-Za-z][A-Za-z0-9]*.(procName|varName|value|stmt#)|[A-Za-z][A-Za-z0-9]*)(\s*,\s*([A-Za-z][A-Za-z0-9]*.(procName|varName|value|stmt#)|[A-Za-z][A-Za-z0-9]*))*>)\s*.*)";
    const string REGEX_DECL = R"((\s*(stmt|read|print|call|while|if|assign|variable|constant|procedure|prog_line)\s+[A-Za-z][A-Za-z0-9]*\s*(\s*,\s*[A-Za-z][A-Za-z0-9]*)*\s*;\s*)+)";
    const string REGEX_SELECT = R"(Select\s+(BOOLEAN|([A-Za-z][A-Za-z0-9]*.(procName|varName|value|stmt#)|[A-Za-z][A-Za-z0-9]*)|<\s*([A-Za-z][A-Za-z0-9]*.(procName|varName|value|stmt#)|[A-Za-z][A-Za-z0-9]*)(\s*,\s*([A-Za-z][A-Za-z0-9]*.(procName|varName|value|stmt#)|[A-Za-z][A-Za-z0-9]*))*>))";
    const string REGEX_REL_CLAUSE = R"(such that\s+(Follows|Follows\*|Parent|Parent\*|Modifies|Uses|Calls|Calls\*|Next|Next\*|Affects|Affects\*)\s*\(\s*([A-Za-z][A-Za-z0-9]*|_|\"\s*[A-Za-z][A-Za-z0-9]*\s*\"|[0-9]+)*\s*,\s*([A-Za-z][A-Za-z0-9]*|_|\"\s*[A-Za-z][A-Za-z0-9]*\s*\"|[0-9]+)*\s*\)(\s+and\s+(Follows|Follows\*|Parent|Parent\*|Modifies|Uses|Calls|Calls\*|Next|Next\*|Affects|Affects\*)\s*\(\s*([A-Za-z][A-Za-z0-9]*|_|\"\s*[A-Za-z][A-Za-z0-9]*\s*\"|[0-9]+)*\s*,\s*([A-Za-z][A-Za-z0-9]*|_|\"\s*[A-Za-z][A-Za-z0-9]*\s*\"|[0-9]+)*\s*\))*)";
    const string REGEX_PATTERN = R"(pattern\s+([A-Za-z][A-Za-z0-9]*\s*\(\s*(\s*[A-Za-z][A-Za-z0-9]*\s*|_|\"\s*[A-Za-z][A-Za-z0-9]*\s*\")\s*,\s*(_\s*\".+\"\s*_|_|\".+\")\s*\)|[A-Za-z][A-Za-z0-9]*\s*\(\s*(\s*[A-Za-z][A-Za-z0-9]*\s*|_|\"\s*[A-Za-z][A-Za-z0-9]*\s*\")\s*,\s*_\s*,\s*_\s*\))(\s+and\s+([A-Za-z][A-Za-z0-9]*\s*\(\s*(\s*[A-Za-z][A-Za-z0-9]*\s*|_|\"\s*[A-Za-z][A-Za-z0-9]*\s*\")\s*,\s*(_\s*\".+\"\s*_|_|\".+\")\s*\)|[A-Za-z][A-Za-z0-9]*\s*\(\s*(\s*[A-Za-z][A-Za-z0-9]*\s*|_|\"\s*[A-Za-z][A-Za-z0-9]*\s*\")\s*,\s*_\s*,\s*_\s*\)))*)";
    const string REGEX_PATTERN_ASSIGN_WHILE = R"([A-Za-z][A-Za-z0-9]*\s*\(\s*(\s*[A-Za-z][A-Za-z0-9]*\s*|_|\"\s*[A-Za-z][A-Za-z0-9]*\s*\")\s*,\s*(_\s*\".+\"\s*_|_|\".+\")\s*\))";
    const string REGEX_PATTERN_IF = R"([A-Za-z][A-Za-z0-9]*\s*\(\s*(\s*[A-Za-z][A-Za-z0-9]*\s*|_|\"\s*[A-Za-z][A-Za-z0-9]*\s*\")\s*,\s*_\s*,\s*_\s*\))";
    const string REGEX_WITH = R"(with\s+(\"[A-Za-z][A-Za-z0-9]*\"|[0-9]+|[A-Za-z][A-Za-z0-9]*.(procName|varName|value|stmt#)|[A-Za-z][A-Za-z0-9]*)\s*=\s*(\"[A-Za-z][A-Za-z0-9]*\"|[0-9]+|[A-Za-z][A-Za-z0-9]*.(procName|varName|value|stmt#)|[A-Za-z][A-Za-z0-9]*)(\s+and\s+(\"[A-Za-z][A-Za-z0-9]*\"|[0-9]+|[A-Za-z][A-Za-z0-9]*.(procName|varName|value|stmt#)|[A-Za-z][A-Za-z0-9]*)\s*=\s*(\"[A-Za-z][A-Za-z0-9]*\"|[0-9]+|[A-Za-z][A-Za-z0-9]*.(procName|varName|value|stmt#)|[A-Za-z][A-Za-z0-9]*))*)";
    const string REGEX_WITH_ATTR_COMPARE = R"((\"[A-Za-z][A-Za-z0-9]*\"|[0-9]+|[A-Za-z][A-Za-z0-9]*.(procName|varName|value|stmt#)|[A-Za-z][A-Za-z0-9]*)\s*=\s*(\"[A-Za-z][A-Za-z0-9]*\"|[0-9]+|[A-Za-z][A-Za-z0-9]*.(procName|varName|value|stmt#)|[A-Za-z][A-Za-z0-9]*))";
    const string REGEX_SYN_ATTR = R"([A-Za-z][A-Za-z0-9]*.(procName|varName|value|stmt#))";
    const string REGEX_SYNONYM = R"([A-Za-z][A-Za-z0-9]*)";
    const string REGEX_NAME = R"(\"\s*[A-Za-z][A-Za-z0-9]*\s*\")";
    const string REGEX_WILDCARD = R"(_)";
    const string REGEX_INTEGER = R"([0-9]+)";
    const string REGEX_PARENTHESES = R"(\(.+\))";
    const string REGEX_VALID_SUCH_THAT = R"(such that)";
    const string REGEX_GENERAL_SUCH_THAT = R"(such\s+that)";

    const string REGEX_FOLLOWS = R"(\s*Follows\s*\(\s*(\s*[A-Za-z][A-Za-z0-9]*|_|[0-9]+)\s*,\s*([A-Za-z][A-Za-z0-9]*|_|[0-9]+)\s*\))";
    const string REGEX_FOLLOWS_STAR = R"(\s*Follows\*\s*\(\s*([A-Za-z][A-Za-z0-9]*|_|[0-9]+)\s*,\s*([A-Za-z][A-Za-z0-9]*|_|[0-9]+)\s*\))";
    const string REGEX_PARENT = R"(\s*Parent\s*\(\s*([A-Za-z][A-Za-z0-9]*|_|[0-9]+)\s*,\s*([A-Za-z][A-Za-z0-9]*|_|[0-9]+)\s*\))";
    const string REGEX_PARENT_STAR = R"(\s*Parent\*\s*\(\s*([A-Za-z][A-Za-z0-9]*|_|[0-9]+)\s*,\s*([A-Za-z][A-Za-z0-9]*|_|[0-9]+)\s*\))";
    const string REGEX_MODIFIES_S = R"(\s*Modifies\s*\(\s*([A-Za-z][A-Za-z0-9]*|_|[0-9]+)\s*,\s*([A-Za-z][A-Za-z0-9]*|_|\"\s*[A-Za-z][A-Za-z0-9]*\s*\")\s*\))";
    const string REGEX_MODIFIES_P = R"(\s*Modifies\s*\(\s*([A-Za-z][A-Za-z0-9]*|_|\"\s*[A-Za-z][A-Za-z0-9]*\s*\")\s*,\s*([A-Za-z][A-Za-z0-9]*|_|\"\s*[A-Za-z][A-Za-z0-9]*\s*\")\s*\))";
    const string REGEX_USES_S = R"(\s*Uses\s*\(\s*([A-Za-z][A-Za-z0-9]*|_|[0-9]+)\s*,\s*([A-Za-z][A-Za-z0-9]*|_|\"\s*[A-Za-z][A-Za-z0-9]*\s*\")\s*\))";
    const string REGEX_USES_P = R"(\s*Uses\s*\(\s*([A-Za-z][A-Za-z0-9]*|_|\"\s*[A-Za-z][A-Za-z0-9]*\s*\")\s*,\s*([A-Za-z][A-Za-z0-9]*|_|\"\s*[A-Za-z][A-Za-z0-9]*\s*\")\s*\))";
    const string REGEX_NEXT = R"(\s*Next\s*\(\s*(\s*[A-Za-z][A-Za-z0-9]*|_|[0-9]+)\s*,\s*([A-Za-z][A-Za-z0-9]*|_|[0-9]+)\s*\))";
    const string REGEX_NEXT_STAR = R"(\s*Next\*\s*\(\s*([A-Za-z][A-Za-z0-9]*|_|[0-9]+)\s*,\s*([A-Za-z][A-Za-z0-9]*|_|[0-9]+)\s*\))";
    const string REGEX_CALLS = R"(\s*Calls\s*\(\s*([A-Za-z][A-Za-z0-9]*|_|\"\s*[A-Za-z][A-Za-z0-9]*\s*\")\s*,\s*([A-Za-z][A-Za-z0-9]*|_|\"\s*[A-Za-z][A-Za-z0-9]*\s*\")\s*\))";
    const string REGEX_CALLS_STAR = R"(\s*Calls\*\s*\(\s*([A-Za-z][A-Za-z0-9]*|_|\"\s*[A-Za-z][A-Za-z0-9]*\s*\")\s*,\s*([A-Za-z][A-Za-z0-9]*|_|\"\s*[A-Za-z][A-Za-z0-9]*\s*\")\s*\))";
    const string REGEX_AFFECTS = R"(\s*Affects\s*\(\s*(\s*[A-Za-z][A-Za-z0-9]*|_|[0-9]+)\s*,\s*([A-Za-z][A-Za-z0-9]*|_|[0-9]+)\s*\))";
    const string REGEX_AFFECTS_STAR = R"(\s*Affects\*\s*\(\s*([A-Za-z][A-Za-z0-9]*|_|[0-9]+)\s*,\s*([A-Za-z][A-Za-z0-9]*|_|[0-9]+)\s*\))";
};

namespace TokenNames {
    // Design entities in SIMPLE
    const string PROCEDURE = "procedure";
    const string STMTLST = "stmtLst";
    const string STMT = "stmt";
    const string READ = "read";
    const string PRINT = "print";
    const string ASSIGN = "assign";
    const string CALL = "call";
    const string WHILE = "while";
    const string IF = "if";
    const string ELSE = "else";
    const string VARIABLE = "variable";
    const string CONSTANT = "constant";
    const string THEN = "then";

    // End Tokens
    const string END_SEMICOLON = ";";
    const string END_OPEN_CURLY = "{";
    const string END_CLOSE_CURLY = "}";
    const string END_OPEN_ROUND = "(";
    const string END_CLOSE_ROUND = ")";

    // Operations in SIMPLE
    const string OP_PLUS = "+";
    const string OP_MINUS = "-";
    const string OP_TIMES = "*";
    const string OP_DIVIDE = "/";
    const string OP_MOD = "%";
    const string OP_ASSIGN = "=";

    // Conditional expression
    const string OP_NOT = "!";
    const string OP_AND = "&&";
    const string OP_OR = "||";

    // Rel expression
    const string OP_LESS_THAN = "<";
    const string OP_LESS_THAN_EQUAL = "<=";
    const string OP_GREATER_THAN = ">";
    const string OP_GREATER_THAN_EQUAL = ">=";
    const string OP_EQUAL = "==";
    const string OP_NOT_EQUAL = "!=";

    const unordered_set<string> TOKENS_SET_1 = {OP_PLUS, OP_MINUS, OP_TIMES, OP_DIVIDE, OP_MOD, OP_ASSIGN, OP_LESS_THAN,
                                                OP_GREATER_THAN, OP_NOT};
    const unordered_set<string> TOKENS_SET_2 = {OP_AND, OP_LESS_THAN_EQUAL, OP_GREATER_THAN_EQUAL, OP_EQUAL,
                                                OP_NOT_EQUAL, OP_OR};

    //Token Types
    const string TOKEN_IDENTIFIER = "identifier";
    const string TOKEN_KEYWORD = "keyword";
    const string TOKEN_SEPARATOR = "separator";
    const string TOKEN_OPERATOR = "operator";
    const string TOKEN_LITERAL = "literal";
    const string TOKEN_COMMENT = "comment";
    const string TOKEN_EOF = "end of file";

    const unordered_set<string> TOKEN_KEYWORD_SET = {IF, ELSE, THEN, WHILE, PROCEDURE, READ, CALL, PRINT};
    const unordered_set<string> TOKEN_SEPARATOR_SET = {END_SEMICOLON, END_CLOSE_CURLY, END_OPEN_CURLY, END_OPEN_ROUND,
                                                       END_CLOSE_ROUND};
    const unordered_set<string> TOKEN_OPERATOR_SET = {OP_PLUS, OP_MINUS, OP_TIMES, OP_DIVIDE, OP_MOD, OP_ASSIGN, OP_OR,
                                                      OP_LESS_THAN, OP_GREATER_THAN, OP_NOT, OP_AND, OP_LESS_THAN_EQUAL,
                                                      OP_GREATER_THAN_EQUAL, OP_EQUAL, OP_NOT_EQUAL};
    const string REGEX_LITERAL = R"([0-9]+)";

    // Lexical Token rules
    const string REGEX_NAME = R"([A-Za-z][A-Za-z0-9]*)";
    const string REGEX_INTEGER = R"([0-9]+)";

}
#endif //AUTOTESTER_CONSTANTS_H

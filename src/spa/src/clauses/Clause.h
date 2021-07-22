#ifndef TEAM21_CP_SPA_20S1_CLAUSE_H
#define TEAM21_CP_SPA_20S1_CLAUSE_H

#include <vector>
#include <set>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <regex>

#include "program_knowledge_base/pkb_storage/PKB.h"
#include "utils/SPAEnums.h"
#include "exceptions/SPAException.h"
#include "query_processor/query_preprocessor/QueryParser.h"
#include "query_processor/query_evaluator/QueryResult.h"

using namespace std;

struct Query;

class Clause {
public:
    Clause(string left, string right);

    virtual ~Clause() = default;

    string getLeftParam();

    string getRightParam();

    virtual void validate(unordered_map<string, DesignEntity> map);

    virtual bool evaluate(Query* query, QueryResult* query_res);

    bool isSynonym(string s);

    bool isWildcard(string s);

    bool isInteger(string s);

    bool isName(string s);

    bool isPartialExpr(string s);

    bool isConstant(string s);

    bool isStmt(DesignEntity e);

    bool isProcedure(DesignEntity e);

    bool isSelectedSyn(string to_be_compared, string selected);

    string strip(string str); //remove 1st and last character from the string.

    vector<string> getResultsAccordingToBool(bool res, DesignEntity type);

    static vector<string> findIntersect(vector<string> &res, vector<string> &temp);

    virtual bool equals(Clause *c);

    virtual string getClauseName();

    static string constructPairString(string left, string right);

    static pair<string, string> splitPairString(string pairString);

    virtual int getNumOfSyn();

    virtual set<string> getSynonyms();

    virtual int getRank();

protected:
    string left, right;

    vector<pair<string, string>> findPairIntersect(vector<pair<string, string>> v1, vector<pair<string, string>> v2);
};

#endif //TEAM21_CP_SPA_20S1_CLAUSE_H


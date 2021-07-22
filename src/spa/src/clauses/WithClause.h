#ifndef AUTOTESTER_WITHCLAUSE_H
#define AUTOTESTER_WITHCLAUSE_H

#include "clauses/Clause.h"
#include "query_processor/Query.h"
#include "query_processor/query_evaluator/QueryResult.h"
#include <functional>

class WithClause : public Clause {
    struct attr_hash {
        std::size_t operator()(Attribute a) const noexcept {
            return std::size_t(a);
        }
    };

    enum ReturnValue {
        Int, Val
    };

public:
    WithClause(const string &lhs, const string &rhs);

    void validate(unordered_map<string, DesignEntity> map) override;

    bool evaluate(Query *query, QueryResult *query_res) override;

//    bool equals(WithClause *other);
   int getNumOfSyn() override;

   set<string> getSynonyms() override;

    string getClauseName() override;

    int getRank() override;

private:
    unordered_map<Attribute, ReturnValue, attr_hash> valid_attribute_mappings;

    Attribute validateAttributeType(const string &s, unordered_map<string, DesignEntity> map);

    bool isAttrRef(const string &attr_ref);

    string getSynFromAttrRef(const string &attr_ref);
    string getAttribute(const string &attr_ref);

    bool isCompatibleReturnValue(Attribute lhs_a, Attribute rhs_a);

    bool handleNonStmtNameAttributeAndIdent(QueryResult *query_res,
                                            function<bool(string)> func);

    bool handleStmtNameAttributeAndIdent(QueryResult *query_res, function<vector<int>(string)> func);

    bool handleNonStmtNameAttributeAndStmtNameAttribute(DesignEntity stmt_de, DesignEntity non_stmt_de, QueryResult *query_res,
                                                        function<vector<int>(string)> non_stmt_in_table_func,
                                                        function<vector<string>(int)> stmt_in_table_func);

    bool handleBothNonStmtNameAttribute(DesignEntity l_de, DesignEntity r_de, QueryResult *query_res, function<bool(string)> l_func,
                                        function<bool(string)> r_func);

    bool handleBothStmtNameAttribute(DesignEntity l_de, DesignEntity r_de, QueryResult *query_res, function<vector<int>(string)> l_func,
                                     function<vector<int>(string)> r_func);

    bool handleBothIntAttribute(DesignEntity l_de, DesignEntity r_de, QueryResult *query_res);

    bool handleIntAttributeAndInteger(DesignEntity de, QueryResult *query_res);

    bool handleBothIdentOrBothInteger(QueryResult *query_res);

    bool handleBothSyn(DesignEntity de, QueryResult *query_res);

    bool handleSynAndInteger(DesignEntity de, QueryResult *query_res);

    vector<string> findIntersect(vector<string> &res, vector<string> &temp);

    void reorderParams();

    string left;
    string right;
    Attribute l_attribute;
    Attribute r_attribute;
};


#endif //AUTOTESTER_WITHCLAUSE_H

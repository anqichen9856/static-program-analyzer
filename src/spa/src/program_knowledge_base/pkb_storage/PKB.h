#pragma once

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "utils/SPAEnums.h"

using namespace std;
typedef short PROC;

class TNode;

class VarTable;  // no need to #include "VarTable.h" as all I need is pointer

class PKB {

public:
    static PKB *instance;

    static PKB *getInstance();

    void reset();

//Follows
    unordered_map<int, int> follows_list;
    unordered_map<int, int> inverted_follows_list;

    void insertFollows(int s1, int s2);

//FollowsStar
    unordered_map<int, vector<int> > follows_star_list;
    unordered_map<int, vector<int> > inverted_follows_star_list;

    void insertFollowsStar(int s1, int s2);

//Parent
    unordered_map<int, vector<int> > parent_list;
    unordered_map<int, int> inverted_parent_list;

    void insertParent(int s1, int s2);

//ParentStar
    unordered_map<int, vector<int> > parent_star_list;
    unordered_map<int, vector<int> > inverted_parent_star_list;

    void insertParentStar(int s1, int s2);

//Uses
    unordered_map<int, vector<string> > uses_stmt_list;
    unordered_map<string, vector<int> > inverted_uses_stmt_list;
    unordered_map<string, vector<string> > uses_proc_list;
    unordered_map<string, vector<string> > inverted_uses_proc_list;
    unordered_map<int, vector<string> > uses_stmt_const_list;
    unordered_map<string, vector<int> > inverted_uses_stmt_const_list;
    unordered_map<string, vector<string> > uses_proc_const_list;
    unordered_map<string, vector<string> > inverted_uses_proc_const_list;

    void insertUses(int s, string v);

    void insertUses(string p, string v);

//Modifies
    unordered_map<int, vector<string> > modifies_stmt_list;
    unordered_map<string, vector<int> > inverted_modifies_stmt_list;
    unordered_map<string, vector<string> > modifies_proc_list;
    unordered_map<string, vector<string> > inverted_modifies_proc_list;

    void insertModifies(int s, string v);

    void insertModifies(string p, string v);

    static VarTable *varTable;

    static shared_ptr<TNode> getRootAST(PROC p);
//Calls
    unordered_map<string, vector<string> > calls_list;
    unordered_map<string, vector<string> > inverted_calls_list;
    unordered_map<string, vector<string> > calls_star_list;
    unordered_map<string, vector<string> > inverted_calls_star_list;

    void insertCallsStar(string caller, string called);
//Next
    unordered_map<int, vector<int> > next_list;
    unordered_map<int, vector<int> > inverted_next_list;
    unordered_map<int, vector<int> > next_star_list;
    unordered_map<int, vector<int> > inverted_next_star_list;

    void insertNext(int prior, int latter);
    void insertNextStar(int prior, int latter);
    void updateNext();
    pair<vector<int>, int> updateNextHelper(int statement, pair<vector<int>, int> prior_statements);
    void recGetNextStar(int statement, vector<int> past, vector<int> &answer);
    void recGetPrevStar(int statement, vector<int> past, vector<int> &answer);

//Used by Parser
    bool initAST(const shared_ptr<TNode> &ast);

//Used by QE

//StatementMasterLists
    vector<int> getAllWhile();

    vector<int> getAllIf();

    vector<int> getAllRead();

    vector<int> getAllPrint();

    vector<int> getAllCall();

    vector<int> getAllAssign();

    vector<int> getAllStmt();

    vector<int> while_list;
    vector<int> if_list;
    vector<int> read_list;
    vector<int> print_list;
    vector<int> call_list;
    vector<int> assign_list;
    vector<int> all_statements;
    vector<string> all_procedures;
    vector<string> all_variables;
    vector<string> all_constants;

    unordered_map<int, DesignEntity> stmt_type_list;

//getElements
    vector<string> getAllVar();

    vector<string> getAllConstants();

    static vector<string> getAllProcedure();

    vector<int> getStmtLst(int s);

    DesignEntity getStmtType(int s);

    //vector<int> getAllMatchedStmt(DesignEntity type);

    //vector<string> getAllMatchedVarOrConst(DesignEntity type);

    vector<string> getAllMatchedEntity(DesignEntity type);

    vector<string> convertIntToString(vector<int> v);

//getRelationships
//follows
    bool isFollows(int s1, int s2);

    int getFollowedBy(int s);

    int getFollows(int s);

    //vector<string> getLeading(DesignEntity type, DesignEntity selected_type);

    //vector<string> getNonLeading(DesignEntity type, DesignEntity selected_type);

    bool isFollowsStar(int s1, int s2);

    vector<int> getFollowedByStar(int s);

    vector<int> getFollowsStar(int s);

    //vector<string> getLeadingStar(DesignEntity type, DesignEntity selected_type);

    //vector<string> getNonLeadingStar(DesignEntity type, DesignEntity selected_type);

//parent
    bool isParent(int s1, int s2);

    int getParent(int s);

    vector<int> getChildren(int s);

    bool isParentStar(int s1, int s2);

    vector<int> getParentStar(int s);

    vector<int> getChildrenStar(int s);

    bool isTopLevelWhile(int s);

    bool isTopLevelIf(int s);

//uses
    bool isUses(int s, string var);

    bool isUses(string procedure, string var);

    vector<string> getAllVariablesUsedBy(int s);

    vector<string> getAllVariablesUsedBy(string p);

    vector<string> getAllConstantsUsedBy(int s);

    vector<string> getAllConstantsUsedBy(const string& p);

    bool usesVar(int s);

    bool usesVar(string p);

    bool usesConst(int s);

    bool usesConst(string p);

    bool isUsed(string var);

    vector<string> getAllUsedVar();

    vector<string> getAllUsedConst();

    vector<string> getAllVarUses(string v);

//modifies
    bool isModifies(int s, string var);

    bool isModifies(string procedure, string var);

    vector<string> getAllVariablesModifiedBy(int s);

    vector<string> getAllVariablesModifiedBy(string procedure);

    bool modifiesVar(int s);

    bool modifiesVar(string procedure);

    bool isModified(string var);

    vector<string> getAllModified();

    vector<string> getAllVarModifies(string v);
//Next

    bool isNextStatement(int prior, int latter);
    bool isNextStar(int prior, int latter);

    vector<int> getNextStatements(int statement);
    vector<int> getPreviousStatements(int statement);
    vector<int> getNextStar(int statement);
    vector<int> getPreviousStar(int statement);

//Calls
    void addCalls(int statement, string variable);
    void updateCalls();
    void updateCallsHelper(string procedure, string root, vector<string> past);
    bool isCalls(string caller, string called);
    bool isCallsStar(string caller, string called);
    vector<string> getCalls(string procedure);
    vector<string> getCallsStar(string procedure);
    vector<string> getCalledBy(string procedure);
    vector<string> getCalledByStar(string procedure);
    //Matching Expressions
    vector<int> getMatch(string lhs, string rhs);
//With clause API
    vector<int> getAllStatementsThatRead(string variable);
    vector<int> getAllStatementsThatPrint(string variable);
    vector<int> getAllStatementsThatCall(string procedure);
    string getProcedureCalledBy(int statement);
    string getVariableReadBy(int statement);
    string getVariablePrintedBy(int statement);
    bool isProcedure(string procedure);
    bool isVariable(string variable);

    class VarTable;  // no need to #include "VarTable.h" as all I need is pointer

    bool addStatements(vector<shared_ptr<TNode> > vector, int parent);

    bool addElseStatements(vector<shared_ptr<TNode> > statements, int parent);

    bool addStatement(const shared_ptr<TNode> &statement, int index, int parent, int prev);

    bool updateStar();

    void updateFollowsStar();

    void updateParentStar();

    void addStatementVariableRelationship(shared_ptr<TNode> statement, int index);

    void updateStatementVariableRelationShip();

    void addModifiesRelationship(shared_ptr<TNode> statement, int index);

    void addUsesRelationship(shared_ptr<TNode> statement, int index);

    void updateModifiesRelationship();

    void recUpdateModifies(int i);

    void insertVariable(shared_ptr<TNode> sharedPtr);

    shared_ptr<TNode> getVar(string variable);

    void recAddUses(shared_ptr<TNode> expression, int index);

    void updateUsesRelationship();

    void recUpdateUses(int index);

    void insertConstant(shared_ptr<TNode> constant);



    vector<int> getMatchedStmt(string LHS, string RHS);
    vector<string> getVarMatch(string RHS);
    vector<int> getPartialMatchHelper(string LHS, string RHS, vector<int> list_of_statments);
    bool matchStatement(shared_ptr<TNode> statement, string variable, shared_ptr<TNode> RHS);
    bool matchExpression(shared_ptr<TNode> left, shared_ptr<TNode> right);
    bool matchPartialStatement(shared_ptr<TNode> statement, string left, shared_ptr<TNode> right);
    bool recMatch(shared_ptr<TNode> expression, shared_ptr<TNode> right);
    bool isMatch(int s, string LHS, string RHS);

    vector<pair<string, string> > getMatchedAssignPair(string RHS);
    bool isNextStarHelper(int prior, int latter, vector<int> past);
    vector<int> getMatchHelper(string LHS, string RHS, vector<int> list_of_statements);

    //while matching
    bool isControlVar(int s, string expr);
    vector<pair<string, string> > getMatchedWhilePair();
    vector<int> getMatchedWhile(string expr);

    vector<pair<string, string> > getMatchedIfPair();
    vector<int> getMatchedIf(string expr);

    void addIfWhileUses(shared_ptr<TNode> sharedPtr, int index);
    bool isInList(vector<string> list, string s);

    string getAttribute(DesignEntity type, string entity, string attribute);

    vector<int> getAffects(int s);
    void getAffectsHelper(int statement, string variable, vector<int> past, vector<int> &answer);
    bool isAffects(int affecting, int affected);
    bool isAffectsStar(int affecting, int affected);
    void recGetAffected(int statement, string variable, vector<int> past, vector<int> &answer);
    vector<int> getAffected(int statement);

    vector<int> getAffectsStar(int s);
    void getAffectsStarHelper(int statement, string variable, vector<pair<int, int> > past, vector<int> &answer, int root);

    void getAffectedStarHelper(int statement, string variable, vector<pair<int, int> > past, vector<int> &answer, int root);
    vector<int> getAffectedStar(int statement);

    //EXTENSIONS

    //NextBIP
    unordered_map<int, vector<int> > next_bip_list;
    unordered_map<int, vector<int> > inverted_next_bip_list;
    vector<int> getNextBIP(int statement, vector<vector<int> > &stack);
    bool isNextBIP(int prev, int next, vector<vector<int> > &stack);

    void addProcEnds();

    vector<int> addProcEndsHelper(int, vector<int> past);

    vector<int> getPrevBIP(int statement, vector<int> &stack);

    vector<string> getMatchedIfVar(int s);

    vector<string> getMatchedWhileVar(int s);

    vector<string> getMatchedAssignVar(int s, string right);

    void insertAffected(vector<int> affecting, int affected);

    void insertAffects(int affecting, vector<int> affected);

    void insertAffectedStar(vector<int> affecting, int affected);

    void insertAffectsStar(int affecting, vector<int> affected);

    void sortLists();
};
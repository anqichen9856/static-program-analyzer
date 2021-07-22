#include <iostream>
#include <string>
#include <vector>

#include "PKB.h"
#include "front_end/parser/TNode.h"
#include "front_end/tokenizer/Tokenizer.h"
#include "front_end/parser/Parser.h"
#include <unordered_map>
#include <algorithm>
#include <exception>

PKB *PKB::instance = 0;

PKB *PKB::getInstance() {
    if (instance == 0) {
        instance = new PKB();
    }
    return instance;
}

//Overall element lists
unordered_map<int, shared_ptr<TNode>> statement_master_list;
unordered_map<string, shared_ptr<TNode>> procedure_list;
unordered_map<string, shared_ptr<TNode>> variable_master_list;
unordered_map<string, shared_ptr<TNode>> constants_master_list;

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
unordered_map<int, vector<string>> if_while_pairs;
unordered_map<string, int> proc_start;
unordered_map<string, vector<int>> proc_end;
unordered_map<string, vector<int>> proc_called_by_calls;
unordered_map<int, string> in_procedure_list;
unordered_map<int, vector<int>> affects_cache;
unordered_map<int, vector<int>> affected_cache;
unordered_map<int, vector<int>> affects_star_cache;
unordered_map<int, vector<int>> affected_star_cache;


void PKB::reset() {
    follows_list.clear();
    inverted_follows_list.clear();
    follows_star_list.clear();
    inverted_follows_star_list.clear();
    parent_list.clear();
    inverted_parent_list.clear();
    parent_star_list.clear();
    inverted_parent_star_list.clear();
    uses_stmt_list.clear();
    inverted_uses_stmt_list.clear();
    uses_proc_list.clear();
    inverted_uses_proc_list.clear();
    uses_stmt_const_list.clear();
    inverted_uses_stmt_const_list.clear();
    uses_proc_const_list.clear();
    inverted_uses_proc_const_list.clear();
    modifies_stmt_list.clear();
    inverted_modifies_stmt_list.clear();
    modifies_proc_list.clear();
    inverted_modifies_proc_list.clear();
    statement_master_list.clear();
    procedure_list.clear();
    variable_master_list.clear();
    constants_master_list.clear();
    while_list.clear();
    if_list.clear();
    read_list.clear();
    print_list.clear();
    call_list.clear();
    assign_list.clear();
    all_statements.clear();
    all_procedures.clear();
    all_variables.clear();
    all_constants.clear();
    stmt_type_list.clear();
    next_list.clear();
    inverted_next_list.clear();
    next_star_list.clear();
    inverted_next_star_list.clear();
    calls_list.clear();
    inverted_calls_list.clear();
    calls_star_list.clear();
    inverted_calls_star_list.clear();
    if_while_pairs.clear();
    proc_start.clear();
    proc_end.clear();
    proc_called_by_calls.clear();
    in_procedure_list.clear();
    next_bip_list.clear();
    inverted_next_bip_list.clear();
    affects_cache.clear();
    affected_cache.clear();
    affects_star_cache.clear();
    affected_star_cache.clear();
}

shared_ptr<TNode> root_ast;

shared_ptr<TNode> PKB::getRootAST(PROC p) {
    return root_ast;
}

bool PKB::initAST(const shared_ptr<TNode> &ast) {
    root_ast = ast;
    //offset indices for statements
    statement_master_list.insert({0, ast});
    vector<shared_ptr<TNode>> procList = ast->getProcedures();
    for (int i = 0; i < int(procList.size()); i++) {
        //insert into procedure_list
        shared_ptr<TNode> proc = procList[i];
        if (find(all_procedures.begin(), all_procedures.end(), proc->getValue()) != all_procedures.end()) {
            string message = "Repeated procedure name: " + proc->getValue();
            throw(InvalidProcedureException(message));
        }
        procedure_list.insert({proc->getValue(), proc});
        all_procedures.push_back(proc->getValue());
        calls_list.insert({proc->getValue(), vector<string>()});
        proc_start.insert({proc->getValue(), statement_master_list.size()});
        int before_stmt_number = statement_master_list.size();
        try {
            addStatements(proc->getStatementList(), -i - 1);
        }
        catch (exception e) {
            std::cerr << "Failed to add " << proc->getValue() << " to Procedure List" << std::endl;
            throw (e);
            return false;
        }
        int after_stmt_number = statement_master_list.size();
        for (int statement = before_stmt_number; statement < after_stmt_number; statement++) {
            in_procedure_list.insert({statement, proc->getValue()});
        }
    }
    updateNext();
    updateStar();
    updateStatementVariableRelationShip();
    addProcEnds();
    sortLists();
    return true;
}

void PKB::sortLists() {
    sort(while_list.begin(), while_list.end(), greater<int>());
    sort(if_list.begin(), if_list.end(), greater<int>());
    sort(assign_list.begin(), assign_list.end(), greater<int>());
    sort(read_list.begin(), read_list.end(), greater<int>());
    sort(all_statements.begin(), all_statements.end(), greater<int>());
    sort(print_list.begin(), print_list.end(), greater<int>());

}


bool PKB::addStatements(vector<shared_ptr<TNode>> statements, int parent) {
    try {
        int prev = -1;
        for (unsigned int i = 0; i < statements.size(); i++) {
            int index = statement_master_list.size();
            addStatement(statements[i], index, parent, prev);
            prev = index;
        }
    }
    catch (exception e) {
        throw e;
    }
    return true;
}

bool PKB::addElseStatements(vector<shared_ptr<TNode>> statements, int parent) {
    try {
        int prev = -1;
        for (unsigned int i = 0; i < statements.size(); i++) {
            int index = statement_master_list.size();
            addStatement(statements[i], index, parent, prev);
            prev = index;
        }
    }
    catch (exception e) {
        throw e;
    }
    return true;
}

bool PKB::addStatement(const shared_ptr<TNode> &statement, int index, int parent, int prev) {
    DesignEntity type = statement->getType();
    statement_master_list.insert({index, statement});
    stmt_type_list.insert({index, type});
    all_statements.push_back(index);
    //addParentRelationship
    insertParent(parent, index);
    //add followsRelationsShip
    insertFollows(prev, index);

    switch (type) {
        case DesignEntity::Assign  :
            assign_list.push_back(index);
            break;
        case DesignEntity::While  :
            while_list.push_back(index);
            addStatements(statement->getStatementList(), index);
            break;
        case DesignEntity::Print  :
            print_list.push_back(index);
            break;
        case DesignEntity::If  :
            if_list.push_back(index);
            addStatements(statement->getStatementList(), index);
            addElseStatements(statement->getElseStatementList(), index);
            break;
        case DesignEntity::Read  :
            read_list.push_back(index);
            break;
        case DesignEntity::Call  :
            call_list.push_back(index);
            addCalls(index, statement->getVariable()->getValue());
            break;
        default:
            break;
    }
    addStatementVariableRelationship(statement, index);

    return true;
}

bool PKB::updateStar() {
    updateFollowsStar();
    updateParentStar();
    return false;
}



//StatementList API

vector<int> PKB::getAllWhile() {
    return while_list;
}

vector<int> PKB::getAllIf() {
    return if_list;
}

vector<int> PKB::getAllRead() {
    return read_list;
}

vector<int> PKB::getAllPrint() {
    return print_list;
}

vector<int> PKB::getAllCall() {
    return call_list;
}

vector<int> PKB::getAllAssign() {
    return assign_list;
}

vector<int> PKB::getAllStmt() {
    /*vector<int> lst;
    lst.reserve(all_statements.size());
    for (int i = 0; i < all_statements.size(); i++) {
        lst.push_back(i);
    }
    ::std::sort(lst.begin(), lst.end());
    return lst;*/
    return all_statements;
}

DesignEntity PKB::getStmtType(int s) {
    return stmt_type_list[s];
    //return statement_master_list[s]->getType();
}
//ProcedureList API


vector<string> PKB::getAllProcedure() {
    vector<string> lst;
    lst.reserve(procedure_list.size());
    for (std::pair<std::string, shared_ptr<TNode>> element : procedure_list) {
        lst.push_back(element.first);
    }
    ::std::sort(lst.begin(), lst.end());
    return lst;
    //return all_procedures;
}

//VariableList API

vector<string> PKB::getAllMatchedEntity(DesignEntity type) {
    switch (type) {
    case DesignEntity::Assign:
        return convertIntToString(assign_list);
    case DesignEntity::While:
        return convertIntToString(while_list);
    case DesignEntity::Print:
        return convertIntToString(print_list);
    case DesignEntity::If:
        return convertIntToString(if_list);
    case DesignEntity::Read:
        return convertIntToString(read_list);
    case DesignEntity::Call:
        return convertIntToString(call_list);
    case DesignEntity::Stmt: case DesignEntity::ProgLine:
        return convertIntToString(all_statements);
    case DesignEntity::Variable:
        return all_variables;
        //return getAllUsedVar();
    case DesignEntity::Constant:
        return all_constants;
        //return convertIntToString(getAllUsedConst());
    case DesignEntity::Procedure:
        return all_procedures;
    default:
        break;
    }
    return vector<string>();
}

vector<string> PKB::convertIntToString(vector<int> v) {
    vector<string> converted;
    for (int i : v) {
        converted.push_back(to_string(i));
    }
    return converted;
}

//Follows API
void PKB::insertFollows(int s1, int s2) {
    if (!follows_list.count(s1)) {
        follows_list.insert({s1, s2});
        inverted_follows_list.insert({s2, s1});
    }
}

bool PKB::isFollows(int s1, int s2) {
    return follows_list.find(s1) != follows_list.end() && follows_list[s1] == s2;
}

int PKB::getFollowedBy(int s) {
    if (inverted_follows_list.find(s) != inverted_follows_list.end()) {
        return inverted_follows_list[s];
    }
    return 0;
}

int PKB::getFollows(int s) {
    if (follows_list.find(s) != follows_list.end()) {
        return follows_list[s];
    }
    return 0;
}

//FollowsStar API
void PKB::insertFollowsStar(int s1, int s2) {
    if (find(follows_star_list[s1].begin(), follows_star_list[s1].end(), s2) == follows_star_list[s1].end()) {
        follows_star_list[s1].push_back(s2);
        inverted_follows_star_list[s2].push_back(s1);
    }
}

void PKB::updateFollowsStar() {
    for (std::pair<int, int> element : inverted_follows_list) {
        if (element.first < 0) {
            continue;
        }
        follows_star_list.insert({element.first, vector<int>()});
        inverted_follows_star_list.insert({element.first, vector<int>()});
        int current = inverted_follows_list.at(element.first);
        while (current > 0) {
            insertFollowsStar(current, element.first);
            current = inverted_follows_list[current];
        }
    }
}

bool PKB::isFollowsStar(int s1, int s2) {
    return follows_star_list.find(s1) != follows_star_list.end() &&
           find(follows_star_list[s1].begin(), follows_star_list[s1].end(), s2)
           != follows_star_list[s1].end();
}

vector<int> PKB::getFollowedByStar(int s) {
    if (inverted_follows_star_list.find(s) != inverted_follows_star_list.end()) {
        return inverted_follows_star_list[s];
    }
    return vector<int>();
}

vector<int> PKB::getFollowsStar(int s) {
    if (follows_star_list.find(s) != follows_star_list.end()) {
        return follows_star_list[s];
    }
    return vector<int>();
}

//Parent API
void PKB::insertParent(int s1, int s2) {
    if (parent_list.find(s1) == parent_list.end()) {
        parent_list[s1].push_back(s2);
        inverted_parent_list.insert({s2, s1});
    } else {
        if (find(parent_list[s1].begin(), parent_list[s1].end(), s2) == parent_list[s1].end()) {
            parent_list[s1].push_back(s2);
            inverted_parent_list.insert({s2, s1});
        }
    }
}

bool PKB::isParent(int s1, int s2) {
    return inverted_parent_list.find(s2) != inverted_parent_list.end() && inverted_parent_list[s2] == s1;
}

int PKB::getParent(int s) {
    return inverted_parent_list.find(s) != inverted_parent_list.end() ? inverted_parent_list[s] : int();
}

vector<int> PKB::getChildren(int s) {
    return parent_list.find(s) != parent_list.end() ? parent_list[s] : vector<int>();
}

//ParentStar API
void PKB::insertParentStar(int s1, int s2) {
    if (find(parent_star_list[s1].begin(), parent_star_list[s1].end(), s2) == parent_star_list[s1].end()) {
        parent_star_list[s1].push_back(s2);
        inverted_parent_star_list[s2].push_back(s1);
    }
}

void PKB::updateParentStar() {
    for (unsigned int i = inverted_parent_list.size(); i > 0; i--) {
        parent_star_list.insert({i, vector<int>()});
        inverted_parent_star_list.insert({i, vector<int>()});
        int current = inverted_parent_list.at(i);
        while (current > -1) {
            insertParentStar(current, i);
            current = inverted_parent_list[current];
        }
    }
}

bool PKB::isParentStar(int s1, int s2) {
    return inverted_parent_star_list.find(s2) != inverted_parent_star_list.end() &&
           find(inverted_parent_star_list[s2].begin(), inverted_parent_star_list[s2].end(), s1) !=
           inverted_parent_star_list[s2].end();
}

vector<int> PKB::getParentStar(int s) {
    return inverted_parent_star_list.find(s) != inverted_parent_star_list.end() ? inverted_parent_star_list[s] : vector<int>();
}

vector<int> PKB::getChildrenStar(int s) {
    return parent_star_list.find(s) != parent_star_list.end() ? parent_star_list[s] : vector<int>();
}

bool PKB::isTopLevelWhile(int s) {
    if (s > int(all_statements.size())) {
        return false;
    }
    int parent = inverted_parent_list[s];
    return (getStmtType(parent) == DesignEntity::While && parent > 0);
}

bool PKB::isTopLevelIf(int s) {
    if (s > int(all_statements.size())) {
        return false;
    }
    int parent = inverted_parent_list[s];
    return (getStmtType(parent) == DesignEntity::If && parent > 0);
}


void PKB::addStatementVariableRelationship(shared_ptr<TNode> statement, int index) {
    addModifiesRelationship(statement, index);
    addUsesRelationship(statement, index);
}

void PKB::updateStatementVariableRelationShip() {
    updateUsesRelationship();
    updateModifiesRelationship();
    updateCalls();
    updateUsesRelationship();
    updateModifiesRelationship();
}

void PKB::addModifiesRelationship(shared_ptr<TNode> statement, int index) {
    DesignEntity type = statement->getType();
    switch (type) {
        case DesignEntity::Read:
        case DesignEntity::Assign:
            insertModifies(index, statement->getVariable()->getValue());
            insertVariable(statement->getVariable());
            break;
        case DesignEntity::If:
            break;
        case DesignEntity::While:
            break;
        case DesignEntity::Print:
            break;
        case DesignEntity::Stmt:
            break;
        case DesignEntity::Procedure:
            break;
        case DesignEntity::Program:
            break;
        case DesignEntity::Constant:
            break;
        case DesignEntity::Variable:
            break;
        case DesignEntity::Op:
            break;
        case DesignEntity::Call:
            break;
        case DesignEntity::Undefined:
            break;
        default:
            break;
    }
}

void PKB::updateModifiesRelationship() {
    for (int i = all_statements.size(); i > 0; i--) {
        recUpdateModifies(i);
    }
}

void PKB::recUpdateModifies(int i) {
    vector<string> toAdd = getAllVariablesModifiedBy(i);
    int parent = inverted_parent_list.at(i);
    if (parent < 0) {
        for (auto variable : toAdd) {
            insertModifies(all_procedures[-parent - 1], variable);
        }
    } else {
        for (auto variable : toAdd) {
            insertModifies(parent, variable);
        }
    }
}


void PKB::addUsesRelationship(shared_ptr<TNode> statement, int index) {
    DesignEntity type = statement->getType();
    switch (type) {
        case DesignEntity::Assign:
            recAddUses(statement->getExpression(), index);
            break;
        case DesignEntity::If:
        case DesignEntity::While:
            recAddUses(statement->getExpression(), index);
            addIfWhileUses(statement->getExpression(), index);
            break;
        case DesignEntity::Print:
            recAddUses(statement->getVariable(), index);
            break;
        case DesignEntity::Call:
            //TODO IF "call proc" uses "proc"
        default:
            break;
    }
}

void PKB::recAddUses(shared_ptr<TNode> expression, int index) {
    DesignEntity type = expression->getType();
    switch (type) {
        case DesignEntity::Variable:
            insertUses(index, expression->getValue());
            insertVariable(expression);
            break;
        case DesignEntity::Constant:
            insertUses(index, expression->getValue());
            insertConstant(expression);
            break;
        case DesignEntity::Op:
            if (expression->getValue() == "!") {
                recAddUses(expression->getLeftNode(), index);
                return;
            }
            recAddUses(expression->getLeftNode(), index);
            recAddUses(expression->getRightNode(), index);
        default:
            return;
    }
}


void PKB::updateUsesRelationship() {
    for (int i = all_statements.size(); i > 0; i--) {
        recUpdateUses(i);
    }
}

void PKB::recUpdateUses(int index) {
    vector<string> toAdd = getAllVariablesUsedBy(index);
    vector<string> toAddConst = getAllConstantsUsedBy(index);

    int parent = inverted_parent_list.at(index);
    if (parent < 0) {
        for (const auto &variable : toAdd) {
            insertUses(all_procedures[-parent - 1], variable);
        }
        for (auto constant : toAddConst) {
            insertUses(all_procedures[-parent - 1], constant);
        }
    } else {
        for (const auto &variable : toAdd) {
            insertUses(parent, variable);
        }
        for (auto constant : toAddConst) {
            insertUses(parent, constant);
        }
    }
}


//Uses API
void PKB::insertUses(int s, string v) {
    if (isdigit(v.at(0))) {
        if (find(uses_stmt_const_list[s].begin(), uses_stmt_const_list[s].end(), v) == uses_stmt_const_list[s].end()) {
            uses_stmt_const_list[s].push_back(v);
            inverted_uses_stmt_const_list[v].push_back(s);
        }
    } else {
        if (find(uses_stmt_list[s].begin(), uses_stmt_list[s].end(), v) == uses_stmt_list[s].end()) {
            uses_stmt_list[s].push_back(v);
            inverted_uses_stmt_list[v].push_back(s);
        }
    }
}

void PKB::insertUses(string p, string v) {
    if (isdigit(v.at(0))) {
        if (find(uses_proc_const_list[p].begin(), uses_proc_const_list[p].end(), v) == uses_proc_const_list[p].end()) {
            uses_proc_const_list[p].push_back(v);
            inverted_uses_proc_const_list[v].push_back(p);
        }
    } else {
        if (find(uses_proc_list[p].begin(), uses_proc_list[p].end(), v) == uses_proc_list[p].end()) {
            uses_proc_list[p].push_back(v);
            inverted_uses_proc_list[v].push_back(p);
        }
    }
}

//uses_stmt_list is more efficient since it is unlikely for one statement to use too many var
bool PKB::isUses(int s, string v) {
    if (isdigit(v.at(0))) {
        return uses_stmt_const_list.find(s) != uses_stmt_const_list.end() &&
               find(uses_stmt_const_list[s].begin(), uses_stmt_const_list[s].end(), v) !=
               uses_stmt_const_list[s].end();
    } else {
        bool res = uses_stmt_list.find(s) != uses_stmt_list.end() &&
                   find(uses_stmt_list[s].begin(), uses_stmt_list[s].end(), v) !=
                   uses_stmt_list[s].end();
        return uses_stmt_list.find(s) != uses_stmt_list.end() &&
               find(uses_stmt_list[s].begin(), uses_stmt_list[s].end(), v) !=
               uses_stmt_list[s].end();
    }
}

//to optimise, can change to search inverted_uses_proc_list
bool PKB::isUses(string p, string v) {
    if (isdigit(v.at(0))) {
        return uses_proc_const_list.find(p) != uses_proc_const_list.end() &&
               find(uses_proc_const_list[p].begin(), uses_proc_const_list[p].end(), v) !=
               uses_proc_const_list[p].end();
    } else {
        return uses_proc_list.find(p) != uses_proc_list.end() &&
               find(uses_proc_list[p].begin(), uses_proc_list[p].end(), v) !=
               uses_proc_list[p].end();
    }
}

vector<string> PKB::getAllVariablesUsedBy(int s) {
    return uses_stmt_list.find(s) == uses_stmt_list.end() ?
           vector<string>() :
           uses_stmt_list[s];
}

vector<string> PKB::getAllVariablesUsedBy(string p) {
    return uses_proc_list.find(p) == uses_proc_list.end() ?
           vector<string>() :
           uses_proc_list[p];
}

vector<string> PKB::getAllConstantsUsedBy(int s) {
    return uses_stmt_const_list.find(s) == uses_stmt_const_list.end() ?
           vector<string>() :
           uses_stmt_const_list[s];
}

vector<string> PKB::getAllConstantsUsedBy(const string& p) {
    return uses_proc_const_list.find(p) == uses_proc_const_list.end() ?
           vector<string>() :
           uses_proc_const_list[p];
}

bool PKB::usesVar(int s) {
    return uses_stmt_list.find(s) != uses_stmt_list.end();
}

bool PKB::usesVar(string p) {
    return uses_proc_list.find(p) != uses_proc_list.end();
}

bool PKB::usesConst(int s) {
    return uses_stmt_const_list.find(s) != uses_stmt_const_list.end();
}

bool PKB::usesConst(string p) {
    return uses_proc_const_list.find(p) != uses_proc_const_list.end();
}

bool PKB::isUsed(string v) {
    if (isdigit(v.at(0))) {
        return inverted_uses_stmt_const_list.find(v) != inverted_uses_stmt_const_list.end();
    } else {
        return inverted_uses_stmt_list.find(v) != inverted_uses_stmt_list.end();
    }
}

vector<string> PKB::getAllUsedVar() {
    vector<string> temp;
    for (auto row : inverted_uses_proc_list) {
        temp.push_back(row.first);
    }
    return temp;
}

vector<string> PKB::getAllUsedConst() {
    vector<string> temp;
    for (auto row : inverted_uses_proc_const_list) {
        temp.push_back(row.first);
    }
    return temp;
}

//Modifies API
void PKB::insertModifies(int s, string v) {
    if (find(modifies_stmt_list[s].begin(), modifies_stmt_list[s].end(), v) == modifies_stmt_list[s].end()) {
        modifies_stmt_list[s].push_back(v);
        inverted_modifies_stmt_list[v].push_back(s);
    }
}

void PKB::insertModifies(string p, string v) {
    if (find(modifies_proc_list[p].begin(), modifies_proc_list[p].end(), v) == modifies_proc_list[p].end()) {
        modifies_proc_list[p].push_back(v);
        inverted_modifies_proc_list[v].push_back(p);
    }
}

bool PKB::isModifies(int s, string v) {
    return modifies_stmt_list.find(s) != modifies_stmt_list.end() &&
           find(modifies_stmt_list[s].begin(), modifies_stmt_list[s].end(), v) !=
           modifies_stmt_list[s].end();
}

bool PKB::isModifies(string p, string v) {
    return modifies_proc_list.find(p) != modifies_proc_list.end() &&
           find(modifies_proc_list[p].begin(), modifies_proc_list[p].end(), v) !=
           modifies_proc_list[p].end();
}

vector<string> PKB::getAllVariablesModifiedBy(int s) {
    return modifies_stmt_list.find(s) == modifies_stmt_list.end() ?
           vector<string>() :
           modifies_stmt_list[s];
}

vector<string> PKB::getAllVariablesModifiedBy(string p) {
    return modifies_proc_list.find(p) == modifies_proc_list.end() ?
           vector<string>() :
           modifies_proc_list[p];
}

bool PKB::modifiesVar(int s) {
    return modifies_stmt_list.find(s) != modifies_stmt_list.end();
}

bool PKB::modifiesVar(string p) {
    return modifies_proc_list.find(p) != modifies_proc_list.end();
}

bool PKB::isModified(string v) {
    return inverted_modifies_stmt_list.find(v) != inverted_modifies_stmt_list.end();
}

vector<string> PKB::getAllModified() {
    vector<string> temp;
    for (auto row : inverted_modifies_proc_list) {
        temp.push_back(row.first);
    }
    return temp;
}

void PKB::insertVariable(shared_ptr<TNode> variable) {
    variable_master_list.insert({variable->getValue(), variable});
    if (find(all_variables.begin(), all_variables.end(), variable->getValue()) == all_variables.end()) {
        all_variables.push_back(variable->getValue());
    }
}

shared_ptr<TNode> PKB::getVar(string variableName) {
    if (variable_master_list.find(variableName) == variable_master_list.end()) {
        return NULL;
    }
    return variable_master_list.at(variableName);
}

void PKB::insertConstant(shared_ptr<TNode> constant) {
    constants_master_list.insert({constant->getValue(), constant});
    if (find(all_constants.begin(), all_constants.end(), constant->getValue()) == all_constants.end()) {
        all_constants.push_back(constant->getValue());
    }
}

vector<string> PKB::getAllConstants() {
    return all_constants;
}

vector<string> PKB::getAllVar() {
    return all_variables;
}

vector<int> PKB::getStmtLst(int s) {
    if (s > int(all_statements.size())) {
        //throw exception("Statement Index Out of Bounds"); //TODO error in build
    } else {
        return parent_list.at(s);
    }
    return vector<int>();
}

shared_ptr<TNode> tokenizeParse(string expr) {
    Tokenizer tokenizer = Tokenizer();
    vector<Token> tokens = tokenizer.tokenizeSource(expr);
    return Parser().buildExpression(tokens);
}
vector<int> PKB::getMatchedStmt(string LHS, string RHS) {
    if (RHS[0] == '_') {
        return getPartialMatchHelper(LHS, RHS, assign_list);
    } else {
        return getMatchHelper(LHS, RHS, assign_list);
    }
}

shared_ptr<TNode> tokenizeParseCond(string expr) {
    Tokenizer tokenizer = Tokenizer();
    vector<Token> tokens = tokenizer.tokenizeSource(expr);
    return Parser().buildCondExpression(tokens);
}

vector<int> PKB::getMatchHelper(string LHS, string RHS, vector<int> list_of_statements) {
    shared_ptr<TNode> expression = tokenizeParse(RHS);
    vector<int> ret = vector<int>();
    for (int index : list_of_statements) {
        if (matchStatement(statement_master_list[index], LHS, expression)) {
            ret.push_back(index);
        }
    }
    return ret;
}

bool PKB::matchStatement(shared_ptr<TNode> statement, string left, shared_ptr<TNode> right) {
    if (left != "_") {
        try {
            if (statement->getVariable()->getValue() != left) {
                return false;
            }
        } catch (exception e) {}
    }
    return matchExpression(statement->getExpression(), right);
}

bool PKB::matchExpression(shared_ptr<TNode> left, shared_ptr<TNode> right) {
    if (right->getType() == DesignEntity::Wildcard) {
        return true;
    }
    DesignEntity leftType = left->getType();
    DesignEntity rightType = right->getType();
    if (leftType != rightType) {
        return false;
    }
    switch (leftType) {
        case DesignEntity::Variable:case DesignEntity::Constant:
            return left->getValue() == right->getValue();
        case DesignEntity::Op:
            if (left->getValue() == "!") {
                return matchExpression(left->getLeftNode(), right->getLeftNode());
            }
            if (left->getValue() != right->getValue()) {
                return false;
            }
            return matchExpression(left->getLeftNode(), right->getLeftNode()) && matchExpression(left->getRightNode(), right->getRightNode());
        default:
            return false;
    }
}

vector<string> PKB::getVarMatch(string RHS) {
    vector<int> stmts = getMatchedStmt("_", RHS);
    vector<string> ret = vector<string>();
    for (int index : stmts) {
        string var = statement_master_list[index]->getVariable()->getValue();
        if (std::find(ret.begin(), ret.end(), var) == ret.end()) {
            ret.push_back(var);
        }
    }
    return ret;
}

bool PKB::matchPartialStatement(shared_ptr<TNode> statement, string left, shared_ptr<TNode> right) {
    if (left != "_") {
        try {
            if (statement->getVariable()->getValue() != left) {
                return false;
            }
        } catch (exception e) {}
    }
    return recMatch(statement->getExpression(), right);
}


vector<int> PKB::getPartialMatchHelper(string LHS, string RHS, vector<int> list_of_statements) {
    shared_ptr<TNode> expression;
    if (RHS == "_") {
        expression = make_shared<WildcardNode>(WildcardNode());
    } else {
        RHS.erase(remove(RHS.begin(), RHS.end(), '_'), RHS.end());
        if (list_of_statements == while_list || list_of_statements == if_list) {
            //TODO?
            expression = tokenizeParse(RHS);
        } else {
            expression = tokenizeParse(RHS);
        }
        shared_ptr<TNode> temp = expression;


        shared_ptr<TNode> l_node = expression;
        while(!(l_node->getType() == DesignEntity::Constant || l_node->getType() == DesignEntity::Variable)) {
            l_node = l_node->getLeftNode();
        }
        l_node->addWildcard();

        //Add wildcard to rightmost node
        shared_ptr<TNode> r_node = expression;
        while(!(r_node->getType() == DesignEntity::Constant || r_node->getType() == DesignEntity::Variable) || r_node->getValue() == "!") {
            r_node = r_node->getRightNode();
        }
        r_node->addWildcard();
    }
    vector<int> ret = vector<int>();
    for (int index : list_of_statements) {
        if (matchPartialStatement(statement_master_list[index], LHS, expression)) {
            ret.push_back(index);
        }
    }
    return ret;
}

bool PKB::recMatch(shared_ptr<TNode> expression, shared_ptr<TNode> right) {
    if (expression == NULL) {
        return false;
    }
    if (matchExpression(expression, right)) {
        return true;
    }
    if (DesignEntity::Op == expression->getType()) {
        if (recMatch(expression->getLeftNode(), right)) {
            return true;
        }
        if (expression->getValue() != "!") {
            if (recMatch(expression->getRightNode(), right)) {
                return true;
            }
        }
    }
    return false;
}

vector<pair<string, string>> PKB::getMatchedAssignPair(string RHS) {
    vector<int> statements = getMatchedStmt("_", RHS);
    vector<pair<string, string>> ret;
    ret.reserve(statements.size());
    for (auto i : statements) {
        ret.emplace_back(to_string(i), statement_master_list[i]->getVariable()->getValue());
    }
    return ret;
}
bool PKB::isMatch(int s, string LHS, string RHS) {
    //todo: change hardcode
    if (LHS == "_" && RHS == "_") {
        return true;
    } else if (RHS == "_") {
        return isModifies(s, LHS);
    } else if (RHS[0] == '_') {
        RHS.erase(remove(RHS.begin(), RHS.end(), '_'), RHS.end());
        return matchPartialStatement(statement_master_list[s], LHS, tokenizeParse(RHS));
    }
    return matchStatement(statement_master_list[s], LHS, tokenizeParse(RHS));
}
bool PKB::isControlVar(int s, string expr) {
    if (expr == "_") {
        return !if_while_pairs[s].empty();
    }
    pair<int, string> target = pair<int, string>(s, expr);
    vector<string> to_find = if_while_pairs[s];
    return (find(to_find.begin(), to_find.end(), expr) != to_find.end());
}

vector<pair<string, string>> PKB::getMatchedWhilePair() {
    vector<pair<string, string>> to_return;
    for (int statement : while_list) {
        for (string variable : if_while_pairs[statement]) {
            to_return.push_back(pair<string, string>(to_string(statement), variable));
        }
    }
    return to_return;
}

vector<pair<string, string>> PKB::getMatchedIfPair() {
    vector<pair<string, string>> to_return;
    for (int statement : if_list) {
        for (string variable : if_while_pairs[statement]) {
            to_return.push_back(pair<string, string>(to_string(statement), variable));
        }
    }
    return to_return;
}

vector<int> PKB::getMatchedWhile(string expr) {
    if (expr == "_") {
        vector<int> to_return;
        for (int statement : while_list) {
            if (!if_while_pairs[statement].empty()) {
                to_return.push_back(statement);
            }
        }
        return to_return;
    }
    return getPartialMatchHelper("_", expr, while_list);
}

vector<int> PKB::getMatchedIf(string expr) {
    if (expr == "_") {
        vector<int> to_return;
        for (int statement : if_list) {
            if (!if_while_pairs[statement].empty()) {
                to_return.push_back(statement);
            }
        }
        return to_return;
    }
    return getPartialMatchHelper("_", expr, if_list);

}


void PKB::insertNext(int prior, int latter) {
    if (find(next_list[prior].begin(), next_list[prior].end(), latter) == next_list[prior].end()) {
        next_list[prior].push_back(latter);
        inverted_next_list[latter].push_back(prior);
    }
}

void PKB::insertNextStar(int prior, int latter) {
    if (find(next_star_list[prior].begin(), next_star_list[prior].end(), latter) == next_star_list[prior].end()) {
        next_star_list[prior].push_back(latter);
    }
}


bool PKB::isNextStatement(int prior, int latter) {
    if (next_list.find(prior) != next_list.end()) {
        return find(next_list[prior].begin(), next_list[prior].end(), latter) != next_list[prior].end();
    } else {
        return false;
    }
}

vector<int> PKB::getNextStatements(int statement) {
    if (next_list.find(statement) != next_list.end()) {
        return next_list[statement];
    } else {
        return vector<int>();
    }
}

vector<int> PKB::getPreviousStatements(int statement) {
    if (inverted_next_list.find(statement) != inverted_next_list.end()) {
        return inverted_next_list[statement];
    } else {
        return vector<int>();
    }
}


bool PKB::isNextStar(int prior, int latter) {
    vector<int> next_star = getNextStar(prior);
    return (find(next_star.begin(), next_star.end(), latter) != next_star.end());
}

void PKB::recGetPrevStar(int statement, vector<int> past, vector<int> &answer) {
    for (int prev_statement : PKB::getPreviousStatements(statement)) {
        if (find(past.begin(), past.end(), prev_statement) == past.end()) {
            past.push_back(prev_statement);
            recGetPrevStar(prev_statement, past, answer);
        }
        if (find(answer.begin(), answer.end(), prev_statement) == answer.end()) {
            answer.push_back(prev_statement);
        }
    }

}

vector<int> PKB::getPreviousStar(int statement) {
    if (inverted_next_star_list.find(statement) != inverted_next_star_list.end()) {
        return inverted_next_star_list[statement];
    }
    vector<int> answer;
    recGetPrevStar(statement, vector<int>({statement}), answer);
    sort(answer.begin(), answer.end());
    answer.erase(unique(answer.begin(), answer.end()), answer.end());
    inverted_next_star_list[statement] = answer;
    return answer;
}


void PKB::updateNext() {
    int prior_statement = 0;
    int total_statements = statement_master_list.size();
    for (int prior_statement = 0; prior_statement < all_statements.size(); prior_statement++) {
        updateNextHelper(prior_statement + 1, pair<vector<int>, int>()).first[0];
    }
    for (pair<int, vector<int>> val : next_list) {
        int target = val.first;
        sort(next_list[target].begin(), next_list[target].end() );
        next_list[target].erase( unique( next_list[target].begin(), next_list[target].end()), next_list[target].end());
    }

}

pair<vector<int>, int> PKB::updateNextHelper(int statement, pair<vector<int>, int> prior_statements) {
    DesignEntity type = getStmtType(statement);
    for (int prior_statement : prior_statements.first) {
        insertNext(prior_statement, statement);
    }
    vector<int> possible_next_statements;
    possible_next_statements.push_back(statement);
    int temp_max = prior_statements.second;
    if (statement > temp_max) {
        temp_max = statement;
    }
    switch (type) {
        case DesignEntity::While : {
            pair<vector<int>, int> back_to_while = updateNextHelper(statement + 1, pair<vector<int>, int>({statement}, temp_max));
            for (int prior_statement : back_to_while.first) {
                insertNext(prior_statement, statement);
            }
            temp_max = back_to_while.second;
            break;
        }
        case DesignEntity::If : {
        //statement lists have to have statements?
            possible_next_statements.pop_back();
            pair<vector<int>, int> true_if = updateNextHelper(statement + 1, pair<vector<int>, int>({statement}, temp_max));
            temp_max = true_if.second;
            int start_false_if = true_if.second;
            pair<vector<int>, int> false_if = updateNextHelper(start_false_if + 1, pair<vector<int>, int>({statement}, temp_max));
            for (int prior_statement : true_if.first) {
                possible_next_statements.push_back(prior_statement);
            }
            for (int prior_statement : false_if.first) {
                possible_next_statements.push_back(prior_statement);
            }
            temp_max = false_if.second;
            break;
        }
        default: {
            break;
        }
    }
    if (getFollows(statement) != 0) {
        auto update = updateNextHelper(getFollows(statement), pair<vector<int>, int>({possible_next_statements}, temp_max));
        possible_next_statements = update.first;
        temp_max = update.second;
    }
    return pair<vector<int>, int>(possible_next_statements, temp_max);
}

void PKB::recGetNextStar(int statement, vector<int> past, vector<int> &answer) {
    for (int next_statement : PKB::getNextStatements(statement)) {
        if (find(past.begin(), past.end(), next_statement) == past.end()) {
            past.push_back(next_statement);
            if (next_star_list.find(next_statement) != next_star_list.end()) {
                vector<int> to_insert = getNextStar(next_statement);
                answer.insert(answer.end(), to_insert.begin(), to_insert.end());
            } else {
                recGetNextStar(next_statement, past, answer);
            }
        }
        if (find(answer.begin(), answer.end(), next_statement) == answer.end()) {
            answer.push_back(next_statement);
        }
    }

}

vector<int> PKB::getNextStar(int statement) {
    //Possible to reduce the update calls
    if (next_star_list.find(statement) != next_star_list.end()) {
        return next_star_list[statement];
    }
    vector<int> answer;
    recGetNextStar(statement, vector<int>({statement}), answer);
    sort(answer.begin(), answer.end());
    answer.erase(unique(answer.begin(), answer.end()), answer.end());
    next_star_list[statement] = answer;
    return answer;

}

void PKB::addCalls(int statement, string variable) {
    int proc = statement;
    while (proc > 0) {
        proc = getParent(proc);
    }
    if (find(calls_list[all_procedures[-proc - 1]].begin(),
             calls_list[all_procedures[-proc - 1]].end(), variable) == calls_list[all_procedures[-proc - 1]].end()) {
        calls_list[all_procedures[-proc - 1]].push_back(variable);
        inverted_calls_list[variable].push_back(all_procedures[-proc - 1]);
    }
}

void PKB::insertCallsStar(string caller, string called) {
    if (find(calls_star_list[caller].begin(), calls_star_list[caller].end(), called) == calls_star_list[caller].end()) {
        calls_star_list[caller].push_back(called);
        inverted_calls_star_list[called].push_back(caller);
    }
}

void yeetRecursion(string root, string called, vector<string> past) {
    if (root == called || find(past.begin(), past.end(), called) != past.end()) {
        throw PKBException("Recursive procedure calls in SIMPLE source");
    }
}
void PKB::updateCallsHelper(string procedure, string root, vector<string> past) {
    for (string procedure_called : getCalls(procedure)) {
        vector<string> new_past = vector<string>(past);
        yeetRecursion(root, procedure_called, new_past);
        new_past.push_back(procedure_called);
        updateCallsHelper(procedure_called, root, new_past);
        insertCallsStar(root, procedure_called);
    }
    for (string procedure_called : getCalls(procedure)) {
        for (string var : getAllVariablesUsedBy(procedure_called)) {
            insertUses(procedure, var);
        }
        for (string var : getAllVariablesModifiedBy(procedure_called)) {
            insertModifies(procedure, var);
        }
        for (string constant : getAllConstantsUsedBy(procedure_called)) {
            insertUses(procedure, constant);
        }
    }
}

void PKB::updateCalls() {
    for (string procedure : all_procedures) {
        updateCallsHelper(procedure, procedure, vector<string>());
    }

    for (int call_statement : call_list) {
        string procedure_called = statement_master_list[call_statement]->getVariable()->getValue();
        if (find(all_procedures.begin(), all_procedures.end(), procedure_called) == all_procedures.end()) {
            throw PKBException("Call Statement: " + to_string(call_statement) +
                               " calling non-existent procedure: " + procedure_called);
        }
        proc_called_by_calls[procedure_called].push_back(call_statement);
        for (string var : getAllVariablesUsedBy(procedure_called)) {
            insertUses(call_statement, var);
        }
        for (string var : getAllVariablesModifiedBy(procedure_called)) {
            insertModifies(call_statement, var);
        }
        for (string constant : getAllConstantsUsedBy(procedure_called)) {
            insertUses(call_statement, constant);
        }
    }
}

vector<string> PKB::getCalls(string procedure) {
    if (calls_list.find(procedure) != calls_list.end()) {
        return calls_list[procedure];
    } else {
        return vector<string>();
    }
}

vector<string> PKB::getCallsStar(string procedure) {
    if (calls_star_list.find(procedure) != calls_star_list.end()) {
        return calls_star_list[procedure];
    } else {
        return vector<string>();
    }
}

vector<string> PKB::getCalledBy(string procedure) {
    if (inverted_calls_list.find(procedure) != inverted_calls_list.end()) {
        return inverted_calls_list[procedure];
    } else {
        return vector<string>();
    }
}

vector<string> PKB::getCalledByStar(string procedure) {
    if (inverted_calls_star_list.find(procedure) != inverted_calls_star_list.end()) {
        return inverted_calls_star_list[procedure];
    } else {
        return vector<string>();
    }
}

bool PKB::isCalls(string caller, string called) {
    if (calls_list.find(caller) != calls_list.end()) {
        return find(calls_list[caller].begin(), calls_list[caller].end(), called) != calls_list[caller].end();
    } else {
        return false;
    }
}

bool PKB::isCallsStar(string caller, string called) {
    if (calls_star_list.find(caller) != calls_star_list.end()) {
        return find(calls_star_list[caller].begin(), calls_star_list[caller].end(), called) != calls_star_list[caller].end();
    } else {
        return false;
    }
}

vector<int> getStatementFromListUsingVar(const vector<int>& statement_list, const string& variable) {
    vector<int> result;
    for (int index : statement_list) {
        if (statement_master_list[index]->getVariable()->getValue() == variable) {
            result.push_back(index);
        }
    }
    return result;
}
vector<int> PKB::getAllStatementsThatRead(string variable) {
    return getStatementFromListUsingVar(read_list, variable);
}

vector<int> PKB::getAllStatementsThatPrint(string variable) {
    return getStatementFromListUsingVar(print_list, variable);
}

vector<int> PKB::getAllStatementsThatCall(string procedure) {
    return getStatementFromListUsingVar(call_list, procedure);
}

string getVariableUsedBy(vector<int> statement_list, int statement, string type) {
    if (find(statement_list.begin(), statement_list.end(), statement) != statement_list.end()) {
        return statement_master_list[statement]->getVariable()->getValue();
    } else {
        throw PKBException("Statement " + to_string(statement) + " not of type: " + type);
    }
}
string PKB::getProcedureCalledBy(int statement) {
    return getVariableUsedBy(call_list, statement, "Calls");
}

string PKB::getVariableReadBy(int statement) {
    return getVariableUsedBy(read_list, statement, "Read");
}

string PKB::getVariablePrintedBy(int statement) {
    return getVariableUsedBy(print_list, statement, "Print");
}

bool PKB::isProcedure(string procedure) {
    return find(all_procedures.begin(), all_procedures.end(), procedure) != all_procedures.end();
}

bool PKB::isVariable(string variable) {
    return find(all_variables.begin(), all_variables.end(), variable) != all_variables.end();
}

bool PKB::isInList(vector<string> list, string s) {
    return find(list.begin(), list.end(), s) != list.end();
}

void PKB::addIfWhileUses(shared_ptr<TNode> expression, int index) {
    DesignEntity type = expression->getType();
    switch (type) {
        case DesignEntity::Op:
            addIfWhileUses(expression->getLeftNode(), index);
            if (expression->getValue() == "!") {
                break;
            }
            addIfWhileUses(expression->getRightNode(), index);
            break;
        case DesignEntity::Variable:
            if (if_while_pairs.find(index) == if_while_pairs.end()) {
                if_while_pairs.insert({index, vector<string>({expression->getValue()})});
            } else {
                if (find(if_while_pairs[index].begin(), if_while_pairs[index].end(), expression->getValue()) == if_while_pairs[index].end()) {
                    if_while_pairs[index].push_back(expression->getValue());
                }
            }
        default:
            break;
    }
}

string PKB::getAttribute(DesignEntity type, string entity, string attribute) {
    try {
        switch (type) {
            case DesignEntity::Procedure:
            case DesignEntity::Variable:
            case DesignEntity::Constant:
                return entity;
            default:
                if (attribute == "stmt#") {
                    return entity;
                }
                return statement_master_list[stoi(entity)]->getVariable()->getValue();
        }
    }
    catch (exception e) {
        throw PKBException(entity + "." + attribute + " does not exist");
    }
}

void PKB::getAffectsHelper(int statement, string variable, vector<int> past, vector<int> &answer) {
    for (int next_statement : PKB::getNextStatements(statement)) {
        if (find(past.begin(), past.end(), next_statement) == past.end()) {
            past.push_back(next_statement);
            if (!(getStmtType(next_statement) == DesignEntity::Assign && isModifies(next_statement, variable))) {
                if (!isModifies(next_statement, variable)
                || getStmtType(next_statement) == DesignEntity::While
                || getStmtType(next_statement) == DesignEntity::If) {
                    getAffectsHelper(next_statement, variable, past, answer);
                }
            }
        }
        if (find(answer.begin(), answer.end(), next_statement) == answer.end()
        && getStmtType(next_statement) == DesignEntity::Assign
        && isUses(next_statement, variable)) {
            answer.push_back(next_statement);
        }
    }

}

vector<int> PKB::getAffects(int statement) {
    vector<int> answer;
    if (affects_cache.find(statement) != affects_cache.end()) {
        return affects_cache[statement];
    }
    if (getStmtType(statement) == DesignEntity::Assign) {
        string variable = statement_master_list[statement]->getVariable()->getValue();
        getAffectsHelper(statement, variable, vector<int>({statement}), answer);
    }
    insertAffects(statement, answer);
    return answer;
}

void PKB::recGetAffected(int statement, string variable, vector<int> past, vector<int> &answer) {
    for (int prev_statement : PKB::getPreviousStatements(statement)) {
        if (find(past.begin(), past.end(), prev_statement) == past.end()) {
            past.push_back(prev_statement);
            if (!(getStmtType(prev_statement) == DesignEntity::Assign && isModifies(prev_statement, variable))) {
                if (!isModifies(prev_statement, variable)
                || getStmtType(prev_statement) == DesignEntity::While
                || getStmtType(prev_statement) == DesignEntity::If) {
                    recGetAffected(prev_statement, variable, past, answer);
                }
            }
        }
        if (find(answer.begin(), answer.end(), prev_statement) == answer.end()
            && getStmtType(prev_statement) == DesignEntity::Assign
            && isModifies(prev_statement, variable)) {
            answer.push_back(prev_statement);
        }
    }
}

vector<int> PKB::getAffected(int statement) {
    if (affected_cache.find(statement) != affected_cache.end()) {
        return affected_cache[statement];
    }
    vector<int> answer;
    if (getStmtType(statement) == DesignEntity::Assign) {
        for (string used : getAllVariablesUsedBy(statement)) {
            recGetAffected(statement, used, vector<int>({statement}), answer);
        }
    }
    sort(answer.begin(), answer.end());
    answer.erase(unique(answer.begin(), answer.end()), answer.end());
    insertAffected(answer, statement);
    return answer;
}

bool PKB::isAffects(int affecting, int affected) {
    vector<int> affecting_list = getAffects(affecting);
    return find(affecting_list.begin(), affecting_list.end(), affected) != affecting_list.end();
}


void PKB::insertAffected(vector<int> affecting, int affected) {
    affected_cache[affected] = affecting;
}

void PKB::insertAffects(int affecting, vector<int> affected) {
    affects_cache[affecting] = affected;
}

void PKB::insertAffectedStar(vector<int> affecting, int affected) {
    affected_star_cache[affected] = affecting;
}

void PKB::insertAffectsStar(int affecting, vector<int> affected) {
    affects_star_cache[affecting] = affected;
}


void PKB::getAffectsStarHelper(int statement, string variable, vector<pair<int, int>> past, vector<int> &answer, int root) {
    for (int next_statement : PKB::getNextStatements(statement)) {
        if (find(past.begin(), past.end(), pair<int, int>(root, next_statement)) == past.end()) {
            past.push_back(pair<int, int>(root, next_statement));
            if (!(getStmtType(next_statement) == DesignEntity::Assign && isModifies(next_statement, variable))) {
                if (!(getStmtType(next_statement) == DesignEntity::Assign && isModifies(next_statement, variable))) {
                    if (!isModifies(next_statement, variable)
                        || getStmtType(next_statement) == DesignEntity::While
                        || getStmtType(next_statement) == DesignEntity::If) {
                        getAffectsStarHelper(next_statement, variable, past, answer, root);
                    }
                }
            }
        }
        if (find(answer.begin(), answer.end(), next_statement) == answer.end()
            && getStmtType(next_statement) == DesignEntity::Assign
            && isUses(next_statement, variable)) {
            answer.push_back(next_statement);
            if (affects_star_cache.find(next_statement) != affects_star_cache.end()) {
                vector<int> to_insert = getAffectsStar(next_statement);
                answer.insert(answer.end(), to_insert.begin(), to_insert.end());
            } else {
                getAffectsStarHelper(next_statement,
                                     statement_master_list[next_statement]->getVariable()->getValue(),
                                     past,
                                     answer,
                                     next_statement);
            }
        }
    }

}

vector<int> PKB::getAffectsStar(int statement) {
    vector<int> answer;
    if (affects_star_cache.find(statement) != affects_star_cache.end()) {
        return affects_star_cache[statement];
    }
    if (getStmtType(statement) == DesignEntity::Assign) {
        string variable = statement_master_list[statement]->getVariable()->getValue();
        pair<int, int> past_statements = pair<int, int>(statement, statement);
        getAffectsStarHelper(statement, variable, vector<pair<int, int>>({past_statements}), answer, statement);
    }
    sort(answer.begin(), answer.end());
    answer.erase(unique(answer.begin(), answer.end()), answer.end());
    insertAffectsStar(statement, answer);
    return answer;
}

bool PKB::isAffectsStar(int affecting, int affected) {
    vector<int> affecting_list = getAffectsStar(affecting);
    return find(affecting_list.begin(), affecting_list.end(), affected) != affecting_list.end();
}

void PKB::getAffectedStarHelper(int statement, string variable, vector<pair<int, int>> past, vector<int> &answer, int root) {
    for (int prev_statement : PKB::getPreviousStatements(statement)) {
        if (find(past.begin(), past.end(), pair<int, int>(root, prev_statement)) == past.end()) {
            past.push_back(pair<int, int>(root, prev_statement));
            if (!(getStmtType(prev_statement) == DesignEntity::Assign && isModifies(prev_statement, variable))) {
                if (!isModifies(prev_statement, variable)
                    || getStmtType(prev_statement) == DesignEntity::While
                    || getStmtType(prev_statement) == DesignEntity::If) {
                    getAffectedStarHelper(prev_statement, variable, past, answer, root);
                }
            }
        }
        if (find(answer.begin(), answer.end(), prev_statement) == answer.end()
            && getStmtType(prev_statement) == DesignEntity::Assign
            && isModifies(prev_statement, variable)) {
            answer.push_back(prev_statement);
            for (string var : getAllVariablesUsedBy(prev_statement)) {
                getAffectedStarHelper(prev_statement,
                                      var,
                                      past,
                                      answer,
                                      prev_statement);
            }
        }
    }
}

vector<int> PKB::getAffectedStar(int statement) {
    vector<int> answer;
    if (affected_star_cache.find(statement) != affected_star_cache.end()) {
        return affected_star_cache[statement];
    }
    if (getStmtType(statement) == DesignEntity::Assign) {
        for (string variable : getAllVariablesUsedBy(statement)) {
            pair<int, int> past_statements = pair<int, int>(statement, statement);
            getAffectedStarHelper(statement, variable, vector<pair<int, int>>({past_statements}), answer, statement);
        }
    }
    insertAffected(answer, statement);
    return answer;
}

//EXTENSIONS
vector<int> PKB::addProcEndsHelper(int statement, vector<int> past) {
    vector<int> result;
    if (getNextStatements(statement).empty()) {
        return vector<int>({statement});
    } else {
        for (int next_statement : getNextStatements(statement)) {
            if (find(past.begin(), past.end(), statement) == past.end()) {
                past.push_back(next_statement);
                vector<int> to_add = addProcEndsHelper(next_statement, past);
                result.insert(result.end(), to_add.begin(), to_add.end());
            }
        }
        return result;
    }
}

void PKB::addProcEnds() {
    for (pair<string, int> proc : proc_start) {
        proc_end.insert({proc.first, addProcEndsHelper(proc.second, vector<int>())});
    }
}
vector<int> PKB::getNextBIP(int statement, vector<vector<int>> &stack) {
    vector<int> result = getNextStatements(statement);
    if (getStmtType(statement) == DesignEntity::Call) {
        return vector<int>({proc_start[getProcedureCalledBy(statement)]});
    } else if (result.empty() && stack.empty()) {
        result.clear();
        vector<int> possible_return = proc_called_by_calls[in_procedure_list[statement]];
        for (int return_point : possible_return) {
            vector<int> return_next = getNextStatements(return_point);
            result.insert(result.end(), return_next.begin(), return_next.end());
        }
        return result;
    } else if (result.empty()) {
        result = vector<int>(stack.back());
        stack.pop_back();
        return result;
    }
    return result;
}

vector<int> PKB::getPrevBIP(int statement, vector<int> &stack) {
    vector<int> result = getPreviousStatements(statement);
    if (result.empty()) {
        if (stack.empty()) {
            return proc_called_by_calls[in_procedure_list[statement]];
        } else if (!stack.empty()) {
            vector<int> prev = vector<int>(stack.back());
            stack.pop_back();
            return prev;
        }
    }
    return result;

}

bool PKB::isNextBIP(int prev, int next, vector<vector<int>> &stack) {
    vector<int> prev_next = getNextBIP(prev, stack);
    return find(prev_next.begin(), prev_next.end(), next) != prev_next.end();
}

vector<string> PKB::getMatchedAssignVar(int s, string right) {
    if (isMatch(s, "_", right)) {
        return getAllVariablesModifiedBy(s);
    }
    return vector<string>();
}
vector<string> PKB::getMatchedWhileVar(int s) {
    return if_while_pairs[s];
}
vector<string> PKB::getMatchedIfVar(int s) {
    return if_while_pairs[s];
}



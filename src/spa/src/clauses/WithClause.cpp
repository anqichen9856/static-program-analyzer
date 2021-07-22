#include "WithClause.h"

WithClause::WithClause(const string &lhs, const string &rhs) : Clause(lhs, rhs) {
    this->left = lhs;
    this->right = rhs;

    valid_attribute_mappings.emplace(Attribute::ProcProcName,
                                     ReturnValue::Val);
    valid_attribute_mappings.emplace(Attribute::CallProcName,
                                     ReturnValue::Val);
    valid_attribute_mappings.emplace(Attribute::ReadVarName,
                                     ReturnValue::Val);
    valid_attribute_mappings.emplace(Attribute::VarVarName,
                                     ReturnValue::Val);
    valid_attribute_mappings.emplace(Attribute::PrintVarName,
                                     ReturnValue::Val);
    valid_attribute_mappings.emplace(Attribute::Ident,
                                     ReturnValue::Val);
    valid_attribute_mappings.emplace(Attribute::Value,
                                     ReturnValue::Int);
    valid_attribute_mappings.emplace(Attribute::StmtNo,
                                     ReturnValue::Int);
    valid_attribute_mappings.emplace(Attribute::Integer,
                                     ReturnValue::Int);

}

void WithClause::validate(unordered_map<string, DesignEntity> map) {
    l_attribute = validateAttributeType(left, map);
    r_attribute = validateAttributeType(right, map);
    if (!isCompatibleReturnValue(l_attribute, r_attribute)) {
        throw InvalidSemanticException("LHS and RHS cannot be compared.");
    }
}

bool WithClause::evaluate(Query *query, QueryResult *query_res) {
/*
 * evaluate logic:
 * ALWAYS CHECK ON SELECTED-SYN
 *  //lhs attr and rhs attr - done
 *  //lhs attr and rhs int - done /lhs int and rhs attr
 *  //lhs attr and rhs name - done /lhs name and rhs attr
 *  //lhs attr and rhs syn - done /lhs syn and rhs attr
 *  //lhs name and rhs int: INVALID
 *  //lhs name and rhs name
 *  //lhs name and rhs syn: INVALID
 *  //lhs int and rhs int
 *  //lhs int and rhs name:INVALID
 *  //lhs syn and rhs int/lhs int and rhs syn
 *  //lhs syn and rhs name: INVALID
 *  //lhs syn and rhs syn
 * }
 */
    unordered_map<string, DesignEntity> syns = query->synonyms;
    bool res = false;
    reorderParams();

    if (l_attribute == Attribute::ProcProcName && r_attribute == Attribute::Ident) {
        //p.procName = IDENT | IDENT = p.procName
        res = handleNonStmtNameAttributeAndIdent(query_res, [&](string s) {
            return PKB::getInstance()->isProcedure(s);
        });
    } else if (l_attribute == Attribute::VarVarName && r_attribute == Attribute::Ident) {
        //v.varName = IDENT | IDENT = v.varName
        res = handleNonStmtNameAttributeAndIdent(query_res, [&](string s) {
            return PKB::getInstance()->isVariable(s);
        });
    } else if (l_attribute == Attribute::CallProcName && r_attribute == Attribute::Ident) {
        //cl.procName = IDENT | IDENT = cl.procName
        res = handleStmtNameAttributeAndIdent(query_res, [&](string s) {
            return PKB::getInstance()->getAllStatementsThatCall(s);
        });
    } else if (l_attribute == Attribute::ReadVarName && r_attribute == Attribute::Ident) {
        //r.varName = IDENT | IDENT = r.varName
        res = handleStmtNameAttributeAndIdent(query_res, [&](string s) {
            return PKB::getInstance()->getAllStatementsThatRead(s);
        });
    } else if (l_attribute == Attribute::PrintVarName && r_attribute == Attribute::Ident) {
        //pn.varName = IDENT | IDENT = pn.varName
        res = handleStmtNameAttributeAndIdent(query_res, [&](string s) {
            return PKB::getInstance()->getAllStatementsThatPrint(s);
        });
    } else if ((l_attribute == Attribute::VarVarName || l_attribute == Attribute::ProcProcName) &&
               r_attribute == Attribute::CallProcName) {
        //v.varName = cl.procName | p.procName = cl.procName  | cl.procName = v.varName | cl.procName = p.procName
        auto l_de = syns[getSynFromAttrRef(left)];
        auto r_de = syns[getSynFromAttrRef(right)];
        res = handleNonStmtNameAttributeAndStmtNameAttribute(r_de, l_de, query_res, [&](string s) {
            return PKB::getInstance()->getAllStatementsThatCall(s);
        }, [&](int i) {
            return vector<string>{PKB::getInstance()->getProcedureCalledBy(i)};
        });
    } else if ((l_attribute == Attribute::VarVarName || l_attribute == Attribute::ProcProcName) &&
               r_attribute == Attribute::ReadVarName) {
        //v.varName = r.varName | p.procName = r.varName  | r.varName = v.varName | r.varName = p.procName
        auto l_de = syns[getSynFromAttrRef(left)];
        auto r_de = syns[getSynFromAttrRef(right)];
        res = handleNonStmtNameAttributeAndStmtNameAttribute(r_de, l_de, query_res, [&](string s) {
            return PKB::getInstance()->getAllStatementsThatRead(s);
        }, [&](int i) {
            return vector<string>{PKB::getInstance()->getVariableReadBy(i)};
        });
    } else if ((l_attribute == Attribute::VarVarName || l_attribute == Attribute::ProcProcName) &&
               r_attribute == Attribute::PrintVarName) {
        //v.varName = pn.varName | p.procName = pn.varName  | pn.varName = v.varName | pn.varName = p.procName
        auto l_de = syns[getSynFromAttrRef(left)];
        auto r_de = syns[getSynFromAttrRef(right)];
        res = handleNonStmtNameAttributeAndStmtNameAttribute(r_de, l_de, query_res, [&](string s) {
            return PKB::getInstance()->getAllStatementsThatPrint(s);
        }, [&](int i) {
            return vector<string>{PKB::getInstance()->getVariablePrintedBy(i)};
        });
    } else if ((l_attribute == Attribute::VarVarName && r_attribute == Attribute::VarVarName)) {
        //v.varName = v.varName | p.procName = v.varName  | v.varName = p.procName | p.procName = p.procName
        DesignEntity de_1 = syns[getSynFromAttrRef(left)];
        DesignEntity de_2 = syns[getSynFromAttrRef(right)];
        res = handleBothNonStmtNameAttribute(de_1, de_2, query_res, [&](string s) {
            return PKB::getInstance()->isVariable(s);
        }, [&](string s) {
            return PKB::getInstance()->isVariable(s);
        });
    }else if (l_attribute == Attribute::VarVarName &&
              r_attribute == Attribute::ProcProcName ) {
        //v.varName = v.varName | p.procName = v.varName  | v.varName = p.procName | p.procName = p.procName
        DesignEntity de_1 = syns[getSynFromAttrRef(left)];
        DesignEntity de_2 = syns[getSynFromAttrRef(right)];
        res = handleBothNonStmtNameAttribute(de_1, de_2, query_res, [&](string s) {
            return PKB::getInstance()->isVariable(s);
        }, [&](string s) {
            return PKB::getInstance()->isProcedure(s);
        });
    }else if ( l_attribute == Attribute::ProcProcName &&
              r_attribute == Attribute::ProcProcName ) {
        //v.varName = v.varName | p.procName = v.varName  | v.varName = p.procName | p.procName = p.procName
        DesignEntity de_1 = syns[getSynFromAttrRef(left)];
        DesignEntity de_2 = syns[getSynFromAttrRef(right)];
        res = handleBothNonStmtNameAttribute(de_1, de_2, query_res, [&](string s) {
            return PKB::getInstance()->isProcedure(s);
        }, [&](string s) {
            return PKB::getInstance()->isProcedure(s);
        });
    } else if (l_attribute == Attribute::ProcProcName && r_attribute == Attribute::VarVarName) {
        //v.varName = v.varName | p.procName = v.varName  | v.varName = p.procName | p.procName = p.procName
        DesignEntity de_1 = syns[getSynFromAttrRef(left)];
        DesignEntity de_2 = syns[getSynFromAttrRef(right)];
        res = handleBothNonStmtNameAttribute(de_1, de_2, query_res, [&](string s) {
            return PKB::getInstance()->isProcedure(s);
        }, [&](string s) {
            return PKB::getInstance()->isVariable(s);
        });
    }
    else if (l_attribute == Attribute::ReadVarName && r_attribute == Attribute::ReadVarName) {
        //r.varName = r.varName
        DesignEntity l_de = syns[getSynFromAttrRef(left)];
        DesignEntity r_de = syns[getSynFromAttrRef(right)];
        res = handleBothStmtNameAttribute(l_de, r_de, query_res, [&](string s) {
            return PKB::getInstance()->getAllStatementsThatRead(s);
        }, [&](string s) {
            return PKB::getInstance()->getAllStatementsThatRead(s);
        });
    } else if (l_attribute == Attribute::PrintVarName && r_attribute == Attribute::ReadVarName) {
        //pn.varName = r.varName
        DesignEntity l_de = syns[getSynFromAttrRef(left)];
        DesignEntity r_de = syns[getSynFromAttrRef(right)];
        res = handleBothStmtNameAttribute(l_de, r_de, query_res, [&](string s) {
            return PKB::getInstance()->getAllStatementsThatPrint(s);
        }, [&](string s) {
            return PKB::getInstance()->getAllStatementsThatRead(s);
        });
    } else if (l_attribute == Attribute::CallProcName && r_attribute == Attribute::ReadVarName) {
        // cl.procName = r.varName
        DesignEntity l_de = syns[getSynFromAttrRef(left)];
        DesignEntity r_de = syns[getSynFromAttrRef(right)];
        res = handleBothStmtNameAttribute(l_de, r_de,  query_res, [&](string s) {
            return PKB::getInstance()->getAllStatementsThatCall(s);
        }, [&](string s) {
            return PKB::getInstance()->getAllStatementsThatRead(s);
        });
    } else if (l_attribute == Attribute::ReadVarName && r_attribute == Attribute::PrintVarName) {
        // r.varName = pn.varName
        DesignEntity l_de = syns[getSynFromAttrRef(left)];
        DesignEntity r_de = syns[getSynFromAttrRef(right)];
        res = handleBothStmtNameAttribute(l_de, r_de, query_res, [&](string s) {
            return PKB::getInstance()->getAllStatementsThatRead(s);
        }, [&](string s) {
            return PKB::getInstance()->getAllStatementsThatPrint(s);
        });
    } else if (l_attribute == Attribute::PrintVarName && r_attribute == Attribute::PrintVarName) {
        // pn.varName = pn.varName
        DesignEntity l_de = syns[getSynFromAttrRef(left)];
        DesignEntity r_de = syns[getSynFromAttrRef(right)];
        res = handleBothStmtNameAttribute(l_de, r_de,  query_res, [&](string s) {
            return PKB::getInstance()->getAllStatementsThatPrint(s);
        }, [&](string s) {
            return PKB::getInstance()->getAllStatementsThatPrint(s);
        });
    } else if (l_attribute == Attribute::CallProcName && r_attribute == Attribute::PrintVarName) {
        // cl.procName = pn.varName
        DesignEntity l_de = syns[getSynFromAttrRef(left)];
        DesignEntity r_de = syns[getSynFromAttrRef(right)];
        res = handleBothStmtNameAttribute(l_de, r_de,  query_res, [&](string s) {
            return PKB::getInstance()->getAllStatementsThatCall(s);
        }, [&](string s) {
            return PKB::getInstance()->getAllStatementsThatPrint(s);
        });
    } else if (l_attribute == Attribute::ReadVarName && r_attribute == Attribute::CallProcName) {
        // r.varName = cl.procName
        DesignEntity l_de = syns[getSynFromAttrRef(left)];
        DesignEntity r_de = syns[getSynFromAttrRef(right)];
        res = handleBothStmtNameAttribute(l_de, r_de, query_res, [&](string s) {
            return PKB::getInstance()->getAllStatementsThatRead(s);
        }, [&](string s) {
            return PKB::getInstance()->getAllStatementsThatCall(s);
        });
    } else if (l_attribute == Attribute::PrintVarName && r_attribute == Attribute::CallProcName) {
        // pn.varName = cl.procName
        DesignEntity l_de = syns[getSynFromAttrRef(left)];
        DesignEntity r_de = syns[getSynFromAttrRef(right)];
        res = handleBothStmtNameAttribute(l_de, r_de,  query_res, [&](string s) {
            return PKB::getInstance()->getAllStatementsThatPrint(s);
        }, [&](string s) {
            return PKB::getInstance()->getAllStatementsThatCall(s);
        });
    } else if (l_attribute == Attribute::CallProcName && r_attribute == Attribute::CallProcName) {
        // cl.procName = cl.procName
        DesignEntity l_de = syns[getSynFromAttrRef(left)];
        DesignEntity r_de = syns[getSynFromAttrRef(right)];
        res = handleBothStmtNameAttribute(l_de, r_de,  query_res, [&](string s) {
            return PKB::getInstance()->getAllStatementsThatCall(s);
        }, [&](string s) {
            return PKB::getInstance()->getAllStatementsThatCall(s);
        });
    } else if (
            (l_attribute == Attribute::StmtNo || l_attribute == Attribute::Value || l_attribute == Attribute::Synonym)
            && (r_attribute == Attribute::StmtNo || r_attribute == Attribute::Value ||
                r_attribute == Attribute::Synonym)) {
        //s.stmtNo = c.value | c.value = s.stmtNo | s.stmtNo = s.stmtNo | c.value = c.value |
        //s.stmtNo = synonym | synonym = s.stmtNo" | c.value = synonym | synonym = c.value
        DesignEntity l_de = syns[getSynFromAttrRef(left)];
        DesignEntity r_de = syns[getSynFromAttrRef(right)];
        res = handleBothIntAttribute(l_de, r_de, query_res);
    } else if ((l_attribute == Attribute::StmtNo || l_attribute == Attribute::Value) &&
               r_attribute == Attribute::Integer) {
        //s.stmtNo = INTEGER | c.value = INTEGER | INTEGER = c.value | INTEGER = s.stmtNo
        DesignEntity l_de = syns[getSynFromAttrRef(left)];
        res=handleIntAttributeAndInteger(l_de, query_res);
    } else if ((isName(left) && isName(right))
               || (isInteger(left) && isInteger(right))) {
        //"IDENT" = "IDENT" | INTEGER = INTEGER
        res = handleBothIdentOrBothInteger(query_res);
    } else if (isSynonym(left) && isSynonym(right)) {
        // synonym = synonym
        DesignEntity de = syns[getSynFromAttrRef(left)];
        res = handleBothSyn(de, query_res);
    } else if (isSynonym(left) && isInteger(right)) {
        //INTEGER = synonym | synonym = INTEGER
        DesignEntity de = syns[getSynFromAttrRef(left)];
        res = handleSynAndInteger(de, query_res);
    }
    return res;
}

Attribute WithClause::validateAttributeType(const string &s, unordered_map<string, DesignEntity> map) {
    if (isName(s)) {
        return Attribute::Ident;
    } else if (isInteger(s)) {
        return Attribute::Integer;
    } else if (isSynonym(s)) {
        DesignEntity e = map[s];
        if (map.find(s) == map.end()) {
            throw InvalidSemanticException("Design Entity is not declared.");
        }
        if (e != DesignEntity::ProgLine) {
            throw InvalidSemanticException("Argument " + s + "  is a synonym that is not prog_line.");
        }
        return Attribute::Synonym;
    } else if (isAttrRef(s)) {
        string attribute = s.substr(s.find(".") + 1);
        string syn = getSynFromAttrRef(s);

        if (map.find(syn) == map.end()) {
            throw InvalidSemanticException("Design Entity is not declared.");
        }

        DesignEntity e = map[syn];

        if (attribute == "procName") {
            switch (e) {
                case DesignEntity::Procedure:
                    return Attribute::ProcProcName;
                case DesignEntity::Call:
                    return Attribute::CallProcName;
                default:
                    throw InvalidSemanticException("Invalid Attribute and Design Entity mapping for: " + s);
            }
        } else if (attribute == "varName") {
            switch (e) {
                case DesignEntity::Print:
                    return Attribute::PrintVarName;
                case DesignEntity::Read:
                    return Attribute::ReadVarName;
                case DesignEntity::Variable:
                    return Attribute::VarVarName;
                default:
                    throw InvalidSemanticException("Invalid Attribute and Design Entity mapping for: " + s);
            }
        } else if (attribute == "value" && e == DesignEntity::Constant) {
            return Attribute::Value;
        } else if (attribute == "stmt#" && isStmt(e) && e != DesignEntity::ProgLine) {
            return Attribute::StmtNo;
        }
    }
    throw InvalidSemanticException("Invalid Query Argument: " + s);
}

bool WithClause::isAttrRef(const string &attr_ref) {
    string r = R"((.*)(.)(procName|varName|value|stmt#))";
    regex reg(r);
    return regex_match(attr_ref, reg);
}

string WithClause::getSynFromAttrRef(const string &attr_ref) {
    return attr_ref.substr(0, attr_ref.find("."));
}

bool WithClause::isCompatibleReturnValue(Attribute a_1, Attribute a_2) {
    return valid_attribute_mappings[a_1] == valid_attribute_mappings[a_2];
}

string WithClause::getClauseName() {
    return "With";
}

bool WithClause::handleNonStmtNameAttributeAndIdent(QueryResult *query_res,
                                                    function<bool(string)> func) {
    vector<string> res;
    auto syn_name = getSynFromAttrRef(left);

    bool isValid = func(strip(right));
    if (isValid) {
        res.push_back(strip(right));
    }

    if(res.empty()) {
        return false;
    }

    if (query_res->isInTable(syn_name)) {
        return query_res->filterOneSyn(syn_name, [&](string s) {
            return PKB::getInstance()->isInList(res, s);
        });
    } else {
        return query_res->addOneSyn(syn_name, res);;
    }
}

bool WithClause::handleStmtNameAttributeAndIdent(QueryResult *query_res,
                                                 function<vector<int>(string)> func) {
    vector<string> res;
    auto syn_name = getSynFromAttrRef(left);

    vector<int> stmts = func(strip(right));
    for (auto stmt: stmts) {
        res.push_back(to_string(stmt));
    }

    if(res.empty()) {
        return false;
    }

    if (query_res->isInTable(syn_name)) {
        return query_res->filterOneSyn(syn_name, [&](string s) {
            return PKB::getInstance()->isInList(res, s);
        });
    } else {
        return query_res->addOneSyn(syn_name, res);;
    }
}

bool WithClause::handleNonStmtNameAttributeAndStmtNameAttribute(DesignEntity stmt_de,DesignEntity non_stmt_de, QueryResult *query_res,
                                                                function<vector<int>(string)> non_stmt_in_table_func, function<vector<string>(int)> stmt_in_table_func) {

    string non_stmt_syn = getSynFromAttrRef(left);
    string stmt_syn = getSynFromAttrRef(right);
    if (query_res->isInTable(stmt_syn) && query_res->isInTable(non_stmt_syn)) {
        return query_res->filterTwoSyn(stmt_syn, non_stmt_syn, [&](string s1, string s2) {
            auto var = PKB::getInstance()->getAttribute(stmt_de,s1, getAttribute(left));
            return var == s2;
        });} else if (query_res->isInTable(stmt_syn) && !query_res->isInTable(non_stmt_syn)) { //addTwoSyn
        return query_res->filterAndAdd(stmt_syn, non_stmt_syn, [&](string s) {
            return stmt_in_table_func(stoi(s));
        });
    } else if (!query_res->isInTable(stmt_syn) && query_res->isInTable(non_stmt_syn)) { //addTwoSyn
        return query_res->filterAndAdd(non_stmt_syn, stmt_syn, [&](string s) {
            return PKB::getInstance()->convertIntToString(non_stmt_in_table_func(s));
        }); } else { // addTwoSyn
        vector<string> res;
        vector<string> res2;
        vector<pair<string, string>> pairs;
        auto names = PKB::getInstance()->getAllMatchedEntity(non_stmt_de);
        for (auto name: names) {
            auto stmts = non_stmt_in_table_func(name);
            if (stmts.empty()) {
                continue;
            }
            for (int i = 0; i < stmts.size(); i++) {
                res.push_back(name);
                string stmt = to_string(stmts[i]);
                res2.push_back(stmt);
                pairs.push_back(pair<string, string>(stmt, name));
            }
        }

        if(res.empty() || res2.empty()) {
            return false;
        }

        return query_res->addTwoSyn(stmt_syn, non_stmt_syn, pairs);
    }
}

bool WithClause::handleBothNonStmtNameAttribute(DesignEntity l_de, DesignEntity r_de, QueryResult *query_res, function<bool(string)> l_func,
                                                function<bool(string)> r_func) {

    auto syn_1 = getSynFromAttrRef(left);
    auto syn_2 = getSynFromAttrRef(right);


    if (query_res->isInTable(syn_1) && query_res->isInTable(syn_2)) {
        return query_res->filterTwoSyn(syn_1, syn_2, [&](string s1, string s2) {
            return s1==s2;
        });
    } else if (query_res->isInTable(syn_1) && !query_res->isInTable(syn_2)) { //addTwoSyn
        return query_res->filterAndAdd(syn_1, syn_2, [&](string s) {
            return r_func(s) ? vector<string>{s} : vector<string>{};
        });
    } else if (!query_res->isInTable(syn_1) && query_res->isInTable(syn_2)) { //addTwoSyn
        return query_res->filterAndAdd(syn_2, syn_1, [&](string s) {
            vector<string> filtered_res ={};
            return l_func(s) ? vector<string>{s} : vector<string>{};
        });
    } else { // addTwoSyn
        vector<string> res;
        vector<string> res2;
        vector<pair<string, string>> pairs;
        auto names_1 = PKB::getInstance()->getAllMatchedEntity(l_de);
        auto names_2 = PKB::getInstance()->getAllMatchedEntity(r_de);
        res = res2 = findIntersect(names_1, names_2);

        if(res.empty() || res2.empty()) {
            return false;
        }

        for (auto name: res) {
            pairs.push_back(make_pair(name, name));
        }
        return query_res->addTwoSyn(syn_1, syn_2, pairs);
    }
}

bool WithClause::handleBothStmtNameAttribute(DesignEntity l_de, DesignEntity r_de, QueryResult *query_res, function<vector<int>(string)> l_func,
                                             function<vector<int>(string)> r_func) {
    vector<string> res;
    vector<string> res2;
    vector<pair<string, string>> pairs;

    auto l_syn = getSynFromAttrRef(left);
    auto r_syn = getSynFromAttrRef(right);

    if (query_res->isInTable(l_syn) && query_res->isInTable(r_syn)) {
        return query_res->filterTwoSyn(l_syn, r_syn, [&](string s1, string s2) {
            auto var_1 = PKB::getInstance()->getAttribute(l_de,s1, getAttribute(left));
            auto var_2 = PKB::getInstance()->getAttribute(r_de,s2, getAttribute(right));
            return var_1 == var_2;
        });}else if (query_res->isInTable(l_syn) && !query_res->isInTable(r_syn)) { //addTwoSyn
        return query_res->filterAndAdd(l_syn, r_syn, [&](string s) {
                auto var = PKB::getInstance()->getAttribute(l_de,s, getAttribute(left));
                return PKB::getInstance()->convertIntToString(r_func(var));
        });
    } else if (!query_res->isInTable(l_syn) && query_res->isInTable(r_syn)) { //addTwoSyn
        return query_res->filterAndAdd(r_syn, l_syn, [&](string s) {
                auto var = PKB::getInstance()->getAttribute(r_de,s, getAttribute(right));
                return PKB::getInstance()->convertIntToString(l_func(var));
        });
    }else { // addTwoSyn
        auto l_stmts = PKB::getInstance()->getAllMatchedEntity(l_de);
        for (auto i: l_stmts) {
            string var = PKB::getInstance()->getAttribute(l_de, i, getAttribute(left));
            auto r_stmts = r_func(var);
            if (!r_stmts.empty()) {
                res.push_back(i);
                for (auto j: r_stmts) {
                    res2.push_back(to_string(j));
                    pairs.push_back(make_pair(i, to_string(j)));
                }
            }
        }

        if(res.empty() || res2.empty()) {
            return false;
        }
        return query_res->addTwoSyn(l_syn, r_syn, pairs);
    }

}


bool WithClause::handleBothIntAttribute(DesignEntity l_de, DesignEntity r_de, QueryResult *query_res) {
    vector<pair<string, string>> pairs;

    vector<string> res;
    vector<string> res2;
    string l_syn = getSynFromAttrRef(left);
    string r_syn = getSynFromAttrRef(right);
    auto left_res = PKB::getInstance()->getAllMatchedEntity(l_de);
    auto right_res = PKB::getInstance()->getAllMatchedEntity(r_de);
    auto intersect = findIntersect(left_res, right_res);
    for (auto i: intersect) {
        res.push_back(i);
        res2.push_back(i);
        pairs.push_back(make_pair(i, i));
    }

    if(res.empty() || res2.empty()) {
        return false;
    }

    if (query_res->isInTable(l_syn) && query_res->isInTable(r_syn)) {
        return query_res->filterTwoSyn(l_syn, r_syn, [&](string s1, string s2) {
            return s1 == s2;
        });
    } else if (query_res->isInTable(l_syn) && !query_res->isInTable(r_syn)) { //addTwoSyn
        return query_res->filterAndAdd(l_syn, r_syn, [&](string s) {
            vector<string> filtered_res ={};
            //for each value in syn value
            if(PKB::getInstance()->isInList(res2,s)){
                filtered_res.push_back(s);
            }
            return filtered_res;
        });
    } else if (!query_res->isInTable(l_syn) && query_res->isInTable(r_syn)) { //addTwoSyn
        return query_res->filterAndAdd(r_syn, l_syn, [&](string s) {
            vector<string> filtered_res ={};
            //for each value in syn value
            if(PKB::getInstance()->isInList(res,s)){
                filtered_res.push_back(s);
            }
            return filtered_res;
        });} else { // addTwoSyn
        return query_res->addTwoSyn(l_syn, r_syn, pairs);
    }
}

bool WithClause::handleIntAttributeAndInteger(DesignEntity de, QueryResult *query_res) {
    vector<string> res;
    string l_syn = getSynFromAttrRef(left);
    auto left_res = PKB::getInstance()->getAllMatchedEntity(de);
    if (find(left_res.begin(), left_res.end(), right) != left_res.end()) {
        res.push_back(right);
    }

    if (query_res->isInTable(l_syn)) {
        return query_res->filterOneSyn(l_syn, [&](string s) {
            return PKB::getInstance()->isInList(res, s);
        });
    } else {
        return query_res->addOneSyn(l_syn, res);
    }

}

bool WithClause::handleBothIdentOrBothInteger(QueryResult *query_res) {
    if (left == right) {
        // left is equal to right, query is satisfied
        return true;
    } else {
        // left is not equal to right, query is not satisfied
        return false;
    }
}

bool WithClause::handleSynAndInteger(DesignEntity de, QueryResult *query_res) {
    vector<string> res;
    if (PKB::getInstance()->getAllStmt().size() >= stoi(right) && stoi(right) > 0) {
        res.push_back(right);
    }

    if(res.empty()) {
        return false;
    }

    if (query_res->isInTable(left)) {
        return query_res->filterOneSyn(left, [&](string s) {
            return PKB::getInstance()->isInList(res, s);
        });
    } else {
        return query_res->addOneSyn(left, res);
    }
}

bool WithClause::handleBothSyn(DesignEntity de, QueryResult *query_res) {
    vector<string>  res =  PKB::getInstance()->getAllMatchedEntity(de);
    vector<pair<string, string>> pairs;

    if (query_res->isInTable(left) && query_res->isInTable(right)) {
        return query_res->filterTwoSyn(left, right, [&](string s1, string s2) {
            return s1 == s2;
        });
    } else if (query_res->isInTable(left) && !query_res->isInTable(right)) { //addTwoSyn
        return query_res->filterAndAdd(left, right, [&](string s) {
            return vector<string>{s};
        });
    } else if (!query_res->isInTable(left) && query_res->isInTable(right)) { //addTwoSyn
        return query_res->filterAndAdd(right, left, [&](string s) {
            return vector<string>{s};
        });
    } else { // addTwoSyn
        for (auto i: res) {
            pairs.push_back(make_pair(i, i));
        }

        if(res.empty()) {
            return false;
        }

        return query_res->addTwoSyn(left, right, pairs);
    }
}

vector<string> WithClause::findIntersect(vector<string> &res, vector<string> &temp) {
    sort(res.begin(), res.end());
    sort(temp.begin(), temp.end());
    vector<string> intersect;
    set_intersection(res.begin(), res.end(), temp.begin(), temp.end(), back_inserter(intersect));
    return intersect;
}

/* Since order of params dont matter in WithClause, they will be reordered to reduce the amount of
 * checks in the evaluate method
 * Order:
 *  VarVarName/ProcProcName > ReadVarName/PrintVarName/CallProcName > Ident
 *  stmtNo/value/Syn > Integer
 * */
void WithClause::reorderParams() {
    //For Idents
    bool swap = false;
    if (l_attribute == Attribute::ReadVarName || l_attribute == Attribute::CallProcName ||
        l_attribute == Attribute::PrintVarName) {
        if (r_attribute == Attribute::ProcProcName || r_attribute == Attribute::VarVarName) {
            swap = true;
        }
    } else if (l_attribute == Attribute::Ident && r_attribute != Attribute::Ident) {
        swap = true;
    } else if (l_attribute == Attribute::Integer && r_attribute != Attribute::Integer) {
        swap = true;
    }

    if (swap) {
        auto attr_temp = l_attribute;
        auto str_temp = left;
        l_attribute = r_attribute;
        left = right;
        r_attribute = attr_temp;
        right = str_temp;
    }
}

string WithClause::getAttribute(const string &attr_ref) {
    return attr_ref.substr(attr_ref.find(".") + 1);;
}

int WithClause::getRank() {
    return 1;
}
//bool WithClause::equals(WithClause *other) {
//    return this->getLeftParam() == other->getLeftParam() && this->getRightParam() == other->getRightParam();
//}

int WithClause::getNumOfSyn() {
    int count  = 0;
    if(!isName(left) && !isInteger(left)) {
        count++;
    }
    if(!isName(right) && !isInteger(right)) {
        count++;
    }
    return count;
}

set<string> WithClause::getSynonyms() {
    set<string> syns;
    if (isAttrRef(left) || isSynonym(left)) {
        auto syn = getSynFromAttrRef(left);
        syns.emplace(syn);
    }

    if (isAttrRef(right) || isSynonym(right)) {
        auto syn = getSynFromAttrRef(right);
        syns.emplace(syn);
    }
    return syns;
}
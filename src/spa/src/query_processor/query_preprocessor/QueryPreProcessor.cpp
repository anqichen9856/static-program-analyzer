#include "QueryPreProcessor.h"

QueryPreProcessor *QueryPreProcessor::instance = NULL;

QueryPreProcessor *QueryPreProcessor::getInstance() {
    if (instance == NULL) {
        instance = new QueryPreProcessor();
    }
    return instance;
}

Query QueryPreProcessor::constructQuery(string query_string) {
    try {
        if (QuerySyntaxChecker::hasInvalidSuchThat(query_string)) {
            throw InvalidQueryException("Invalid characters between 'such' and 'that'.");
        }

        regex rgx(R"(\s+)");
        query_string = regex_replace(query_string, rgx, " ");

        if (!QuerySyntaxChecker::checkQueryStructure(query_string)) {
            throw InvalidQueryException("Invalid query structure.");
        }
        unordered_map<string, DesignEntity> syn_entity_map = getDeclaration(query_string);
        vector<string> selected = getSelected(query_string);
        vector<Clause *> clauses = getClauses(query_string, syn_entity_map);

        if (QuerySyntaxChecker::hasRemaining(query_string)) {
            throw InvalidQueryException("Invalid query structure.");
        }

        //semantic check
        for (string syn : selected) {
            if (QueryParser::isSynAttrPair(syn)) {
                string synonym = QueryParser::getSyn(syn);
                string attribute = QueryParser::getAttr(syn);
                if (!isInMap(synonym, syn_entity_map)) {
                    throw InvalidSemanticException("Selected synonym not declared.");
                }
                if (!isValidSynAttrPair(syn_entity_map[synonym], attribute)) {
                    throw InvalidSemanticException("Selected synonym and attribute are not compatible.");
                }
            } else if (syn != "BOOLEAN" && !isInMap(syn, syn_entity_map)) {
                throw InvalidSemanticException("Selected synonym not declared.");
            }
        }

        for (Clause *c : clauses) {
            vector<string> params = {c->getLeftParam(), c->getRightParam()};
            for (string param : params) {
                if (c->isSynonym(param) && !isInMap(param, syn_entity_map)) {
                    throw InvalidSemanticException("Synonym used in clauses not declared.");
                }
            }
            c->validate(syn_entity_map);
        }
        return {syn_entity_map, selected, clauses};
    } catch (InvalidSemanticException &e) {
        if(!QueryParser::isSubString(QuerySyntaxChecker::getValidDeclaration(query_string), "BOOLEAN") && QueryParser::isSubString(QuerySyntaxChecker::getValidSelected(query_string), "BOOLEAN")) {
            throw InvalidSemanticException("Return False for semantically invalid query.");
        } else {
            throw InvalidQueryException("Return empty result for semantically invalid query.");
        }
    }
}

unordered_map<string, DesignEntity> QueryPreProcessor::getDeclaration(string query_string) {
    string decl = QuerySyntaxChecker::getValidDeclaration(query_string);
    vector<string> decl_parts = QueryParser::tokenizeBy(decl, ";");
    unordered_map<string, DesignEntity> syn_entity_map;
    for (string s : decl_parts) {
        vector<string> tokens = QueryParser::tokenizeBy(s, ",");
        vector<string> first_token_parts = QueryParser::tokenizeBy(tokens[0], " ");
        DesignEntity entity = toEntity(first_token_parts[0]);
        tokens[0] = first_token_parts[1];
        for (string syn : tokens) {
            syn = QueryParser::removeSpaces(syn);
            if (isInMap(syn, syn_entity_map)) {
                throw InvalidSemanticException("Duplicate synonyms in query.");
            } else if (syn != ",") {
                syn_entity_map.insert({syn, entity});
            } else {
                continue;
            }
        }
    }
    return syn_entity_map;
}

vector<string> QueryPreProcessor::getSelected(string query_string) {
    vector<string> selected;
    string selected_str = QuerySyntaxChecker::getValidSelected(query_string);
    if (selected_str.empty()) {
        throw InvalidQueryException("Invalid syntax for Select clause.");
    }
    selected_str = QueryParser::removeSpaces(selected_str);
    if (selected_str == "BOOLEAN") {
        selected.push_back("BOOLEAN");
        return selected;
    } else {
        selected_str = QueryParser::tokenizeBy(selected_str, "<")[0];
        selected_str = QueryParser::tokenizeBy(selected_str, ">")[0];
        vector<string> tokens = QueryParser::tokenizeBy(selected_str, ",");
        for (string token : tokens) {
            token = QueryParser::removeSpaces(token);
            selected.push_back(token);
        }
        return selected;
    }
}

vector<Clause *> QueryPreProcessor::getClauses(string query_string, unordered_map<string, DesignEntity> syn_entity_map) {
    vector<Clause *> clauses = {};
    if (QueryParser::getMatchedMulti(query_string, REGEX_REL_CLAUSE).size() > 0) {
        getRelClauses(query_string, clauses);
    }
    if (QueryParser::getMatchedMulti(query_string, REGEX_PATTERN).size() > 0) {
        getPatternClauses(query_string, clauses, syn_entity_map);
    }
    if (QueryParser::getMatchedMulti(query_string, REGEX_WITH).size() > 0) {
        getWithClauses(query_string, clauses, syn_entity_map);
    }
    return clauses;
}

void QueryPreProcessor::getRelClauses(string query_string, vector<Clause *> &clauses) {
    vector<string> rel_clause_groups = QuerySyntaxChecker::getValidRelClauseGroups(query_string);
    if (rel_clause_groups.empty()) {
        throw InvalidQueryException("Invalid rel clause.");
    }
    for (string s : rel_clause_groups) {
        string rel_group = QueryParser::removeFirstNWord(s, 2);
        vector<string> rel_clauses = QueryParser::tokenizeBy(rel_group, " and ");
        for (int i = 0; i < rel_clauses.size(); i++) {
            rel_clauses[i] = QueryParser::removeSpaces(rel_clauses[i]);
        }
        for (string rel_clause: rel_clauses) {
            if (!(QueryParser::isMatched(rel_clause, REGEX_FOLLOWS) ||
                  QueryParser::isMatched(rel_clause, REGEX_FOLLOWS_STAR) ||
                  QueryParser::isMatched(rel_clause, REGEX_PARENT) ||
                  QueryParser::isMatched(rel_clause, REGEX_PARENT_STAR) ||
                  QueryParser::isMatched(rel_clause, REGEX_MODIFIES_P) ||
                  QueryParser::isMatched(rel_clause, REGEX_MODIFIES_S) ||
                  QueryParser::isMatched(rel_clause, REGEX_USES_P) ||
                  QueryParser::isMatched(rel_clause, REGEX_USES_S) ||
                  QueryParser::isMatched(rel_clause, REGEX_CALLS) ||
                  QueryParser::isMatched(rel_clause, REGEX_CALLS_STAR) ||
                  QueryParser::isMatched(rel_clause, REGEX_NEXT) ||
                  QueryParser::isMatched(rel_clause, REGEX_NEXT_STAR) ||
                  QueryParser::isMatched(rel_clause, REGEX_AFFECTS) ||
                  QueryParser::isMatched(rel_clause, REGEX_AFFECTS_STAR))) {
                throw InvalidQueryException("Rel clause syntax is wrong.");
            }
            vector<string> parts = QueryParser::tokenizeBy(rel_clause, "(");
            string rel = QueryParser::removeSpaces(parts[0]);
            string bracket = QueryParser::tokenizeBy(parts[1], ")")[0];
            vector<string> params = QueryParser::tokenizeBy(bracket, ",");
            string left = QueryParser::removeSpaces(params[0]);
            string right = QueryParser::removeSpaces(params[1]);
            RelClause* relClause = constructRelClause(rel, left, right);
            bool existing = false;
            for (Clause* clause : clauses) {
                if (clause->equals(relClause)) {
                    existing = true;
                }
            }
            if (!existing) {
                clauses.push_back(relClause);
            }
        }
    }
}

void QueryPreProcessor::getPatternClauses(string query_string, vector<Clause *> &clauses, unordered_map<string, DesignEntity> syn_entity_map) {
    vector<string> pttn_clause_groups = QuerySyntaxChecker::getValidPatternClauseGroups(query_string);
    if (pttn_clause_groups.empty()) {
        throw InvalidQueryException("Invalid pattern clause.");
    }
    for (string s : pttn_clause_groups) {
        string pttn_group = QueryParser::removeFirstNWord(s, 1);
        vector<string> pttn_clauses = QueryParser::tokenizeBy(pttn_group, " and ");
        for (int i = 0; i < pttn_clauses.size(); i++) {
            pttn_clauses[i] = QueryParser::removeSpaces(pttn_clauses[i]);
        }
        for (string pttn_clause: pttn_clauses) {
            if (!(isPatternAssignWhile(pttn_clause) || isPatternIf(pttn_clause))) {
                throw InvalidQueryException("Pattern clause syntax is wrong.");
            }
            string pttn_syn = QueryParser::tokenizeBy(QueryParser::tokenizeBy(pttn_clause, "(")[0], " ")[0];
            if (syn_entity_map.find(pttn_syn) == syn_entity_map.end()) {
                throw InvalidSemanticException("Synonym in pattern clause not declared.");
            }
            string pttn = QueryParser::getMatched(pttn_clause, REGEX_PARENTHESES);
            pttn = pttn.substr(1, pttn.size() - 2);
            vector<string> params = QueryParser::tokenizeBy(pttn, ",");
            string left = QueryParser::removeSpaces(params[0]);
            string mid = QueryParser::removeSpaces(params[1]);

            if (isPatternAssignWhile(pttn_clause)) {
                if (syn_entity_map[pttn_syn] == DesignEntity::Assign) {
                    clauses.push_back(new PatternAssign(pttn_syn, left, mid));
                } else if (syn_entity_map[pttn_syn] == DesignEntity::While) {
                    clauses.push_back(new PatternWhile(pttn_syn, left, mid));
                } else {
                    throw InvalidSemanticException("Synonym in PatternAssign/PatternWhile clause is not assign/while.");
                }
            }

            if (isPatternIf(pttn_clause)) {
                if (syn_entity_map[pttn_syn] == DesignEntity::If) {
                    string right = QueryParser::removeSpaces(params[2]);
                    clauses.push_back(new PatternIf(pttn_syn, left, mid, right));
                } else {
                    throw InvalidSemanticException("Synonym in PatternIf clause is not if.");
                }
            }
        }
    }
}

void QueryPreProcessor::getWithClauses(string query_string, vector<Clause *> &clauses, unordered_map<string, DesignEntity> syn_entity_map) {
    vector<string> with_clause_groups = QuerySyntaxChecker::getValidWithClauseGroups(query_string);
    for (string s : with_clause_groups) {
        string with_group = QueryParser::removeFirstNWord(s, 1);
        vector<string> with_clauses = QueryParser::tokenizeBy(with_group, " and ");
        for (int i = 0; i < with_clauses.size(); i++) {
            with_clauses[i] = QueryParser::removeSpaces(with_clauses[i]);
        }
        for (string with_clause: with_clauses) {
            if (!(QueryParser::isMatched(with_clause, REGEX_WITH_ATTR_COMPARE))) {
                throw InvalidQueryException("With clause syntax is wrong.");
            }
            vector<string> parts = QueryParser::tokenizeBy(with_clause, "=");
            string left = QueryParser::removeSpaces(parts[0]);
            string right = QueryParser::removeSpaces(parts[1]);
            WithClause * with = new WithClause(left, right);
            with->validate(syn_entity_map);
            if (left != right) {
                clauses.push_back(with);
            }
        }
    }
}

DesignEntity QueryPreProcessor::toEntity(string s) {
    if (!s.compare("stmt")) return DesignEntity::Stmt;
    if (!s.compare("if")) return DesignEntity::If;
    if (!s.compare("while")) return DesignEntity::While;
    if (!s.compare("assign")) return DesignEntity::Assign;
    if (!s.compare("read")) return DesignEntity::Read;
    if (!s.compare("print")) return DesignEntity::Print;
    if (!s.compare("procedure")) return DesignEntity::Procedure;
    if (!s.compare("constant")) return DesignEntity::Constant;
    if (!s.compare("variable")) return DesignEntity::Variable;
    if (!s.compare("call")) return DesignEntity::Call;
    if (!s.compare("prog_line")) return DesignEntity::ProgLine;
    if (!s.compare("_")) return DesignEntity::Wildcard;
    throw InvalidQueryException("String is not a valid DesignEntity.");
}

RelClause *QueryPreProcessor::constructRelClause(string rel, string l, string r) {
    if (!rel.compare("Follows")) return new FollowsClause(l, r);
    if (!rel.compare("Follows*")) return new FollowsStarClause(l, r);
    if (!rel.compare("Parent")) return new ParentClause(l, r);
    if (!rel.compare("Parent*")) return new ParentStarClause(l, r);
    if (!rel.compare("Modifies")) return new ModifiesClause(l, r);
    if (!rel.compare("Uses")) return new UsesClause(l, r);
    if (!rel.compare("Calls")) return new CallsClause(l, r);
    if (!rel.compare("Calls*")) return new CallsStarClause(l, r);
    if (!rel.compare("Next")) return new NextClause(l, r);
    if (!rel.compare("Next*")) return new NextStarClause(l, r);
    if (!rel.compare("Affects")) return new AffectsClause(l, r);
    if (!rel.compare("Affects*")) return new AffectsStarClause(l, r);
    throw InvalidQueryException("Unidentified relational clause keyword.");
}

bool QueryPreProcessor::isInMap(string s, unordered_map<string, DesignEntity> map) {
    return map.find(s) != map.end();
}

bool QueryPreProcessor::isValidSynAttrPair(DesignEntity type, string attr) {
    bool validProcName = (type == DesignEntity::Procedure || type == DesignEntity::Call ) && attr == "procName";
    bool validVarName = (type == DesignEntity::Variable || type == DesignEntity::Read || type == DesignEntity::Print) && attr == "varName";
    bool validValue = type == DesignEntity::Constant && attr == "value";
    bool validStmtNo = (type == DesignEntity::Stmt || type == DesignEntity::Read || type == DesignEntity::Print
            || type == DesignEntity::Call || type == DesignEntity::While || type == DesignEntity::If
            || type == DesignEntity::Assign) && attr == "stmt#";
    return validProcName || validVarName || validValue || validStmtNo;
}
bool QueryPreProcessor::isPatternAssignWhile(string s) {
    return QueryParser::isMatched(s, REGEX_PATTERN_ASSIGN_WHILE) && QueryParser::getMatchedMulti(s, ",").size() == 1;
}

bool QueryPreProcessor::isPatternIf(string s) {
    return QueryParser::isMatched(s, REGEX_PATTERN_IF) && QueryParser::getMatchedMulti(s, ",").size() == 2;
}

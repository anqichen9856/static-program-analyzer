#ifndef AUTOTESTER_SPAENUMS_H
#define AUTOTESTER_SPAENUMS_H

enum class DesignEntity {
    If, While, Assign, Read, Print, Stmt, Procedure, Program, Constant, Variable, Op, Call, Undefined, Wildcard, ProgLine
};

enum class Attribute{
    Value, StmtNo, CallProcName, ProcProcName, VarVarName, ReadVarName, PrintVarName, Integer, Synonym, Ident
};
#endif //AUTOTESTER_SPAENUMS_H

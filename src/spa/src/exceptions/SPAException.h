#ifndef AUTOTESTER_SPAEXCEPTION_H
#define AUTOTESTER_SPAEXCEPTION_H

#include <string>
#include <stdexcept>

using std::string;
using std::exception;

class SPAException : public std::runtime_error {
public:
    explicit SPAException(const std::string &message);

};

class InvalidQueryException : public SPAException {
public:
    explicit InvalidQueryException(const std::string &message);

};


class InvalidSemanticException : public SPAException {
public:
    explicit InvalidSemanticException(const std::string &message);

};

class InvalidTokenizeException : public SPAException {
public:
    explicit InvalidTokenizeException(const std::string &message);

};

class InvalidNodeException : public SPAException {
public:
    explicit InvalidNodeException(const std::string &message);
};

class InvalidTokenException : public SPAException {
public:
    explicit InvalidTokenException(const std::string &message);
};

class InvalidParserException : public SPAException {
public:
    explicit InvalidParserException(const std::string &message);
};

class PKBException : public SPAException {
public:
    explicit PKBException(const std::string &message);
};

class InvalidProcedureException : public PKBException {
public:
    explicit InvalidProcedureException(const std::string &message);
};

class QueryResultException : public SPAException {
public:
    explicit QueryResultException(const std::string &message);
};

#endif //AUTOTESTER_SPAEXCEPTION_H




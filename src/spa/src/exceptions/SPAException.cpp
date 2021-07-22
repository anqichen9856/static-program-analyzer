#include "SPAException.h"

SPAException::SPAException(const std::string &message) : std::runtime_error(message) {}

InvalidQueryException::InvalidQueryException(const std::string &message) : SPAException(message) {}

InvalidSemanticException::InvalidSemanticException(const std::string &message) : SPAException(message) {}

InvalidTokenizeException::InvalidTokenizeException(const std::string &message) : SPAException(message) {}

InvalidNodeException::InvalidNodeException(const std::string &message) : SPAException(message) {}

InvalidTokenException::InvalidTokenException(const std::string &message) : SPAException(message){}

InvalidParserException::InvalidParserException(const std::string &message) : SPAException(message){}

PKBException::PKBException(const std::string &message) : SPAException(message) {}

InvalidProcedureException::InvalidProcedureException(const std::string &message) : PKBException(message){}

QueryResultException::QueryResultException(const std::string &message) : SPAException(message){}


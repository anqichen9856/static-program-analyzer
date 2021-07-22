#ifndef AUTOTESTER_INTRELCLAUSE_H
#define AUTOTESTER_INTRELCLAUSE_H

#include "RelClause.h"

class IntRelClause : public RelClause{
public:
	IntRelClause(string left, string right);

	void validate(unordered_map<string, DesignEntity> map) override;

	bool evaluate(Query* query, QueryResult* query_res) override;

private:
	virtual int getIntRelSize(int statement) = 0;

	virtual int getInvIntRel(int statement) = 0;

	virtual bool isIntRel(int before, int after) = 0;

	virtual unsigned int getIntRelListSize() = 0;

	virtual bool evalIntSyn(string left, string right, QueryResult* query_res, unordered_map<string, DesignEntity> syns) = 0;

	virtual bool evalIntWild(string left, string right) = 0;

	virtual bool evalSynInt(string left, string right, QueryResult* query_res, unordered_map<string, DesignEntity> syns) = 0;

	virtual bool evalSynSynHelper(string left, string right, QueryResult* query_res, unordered_map<string, DesignEntity> syns) = 0;
};

#endif //AUTOTESTER_INTRELCLAUSE_H
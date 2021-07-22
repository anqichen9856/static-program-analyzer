#ifndef AUTOTESTER_NAMESRELCLAUSE_H
#define AUTOTESTER_NAMESRELCLAUSE_H

#include "RelClause.h"

class NamesRelClause : public RelClause {
public:
	NamesRelClause(string left, string right);

	void validate(unordered_map<string, DesignEntity> map) override;

	bool evaluate(Query* query, QueryResult* query_res) override;

private:
	virtual bool isNamesRel(int s, string v) = 0;

	virtual bool isNamesRel(string p, string v) = 0;

	virtual vector<string> getAllNamesRel(int s) = 0;

	virtual vector<string> getAllNamesRel(string p) = 0;
};

#endif //AUTOTESTER_NAMESRELCLAUSE_H
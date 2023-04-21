
#ifndef SFWQUERY_H
#define SFWQUERY_H

#include "ExprTree.h"
#include "MyDB_LogicalOps.h"

// structure that stores an entire SFW query
struct SFWQuery {

private:

	// the various parts of the SQL query
	vector <ExprTreePtr> valuesToSelect;
	vector <pair <string, string>> tablesToProcess;
	vector <ExprTreePtr> allDisjunctions;
	vector <ExprTreePtr> groupingClauses;

public:
	SFWQuery () {}

	SFWQuery (struct ValueList *selectClause, struct FromList *fromClause, 
		struct CNF *cnf, struct ValueList *grouping);

	SFWQuery (struct ValueList *selectClause, struct FromList *fromClause, 
		struct CNF *cnf);

	SFWQuery (struct ValueList *selectClause, struct FromList *fromClause);
	
	// builds and optimizes a logical query plan for a SFW query, returning the resulting logical query plan
	//
	// allTables: this is the list of all of the tables currently in the system
	// allTableReaderWriters: this is so we can store the info that we need to be able to execute the query
	LogicalOpPtr buildLogicalQueryPlan (map <string, MyDB_TablePtr> &allTables, 
		map <string, MyDB_TableReaderWriterPtr> &allTableReaderWriters);

	~SFWQuery () {}

	void print ();

	#include "FriendDecls.h"
};

#endif

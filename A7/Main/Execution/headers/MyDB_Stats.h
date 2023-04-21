
#ifndef STATS_H
#define STATS_H

#include <string>
#include "MyDB_Table.h"
#include "ExprTree.h"

// create a smart pointer for database tables
using namespace std;
class MyDB_Stats;
typedef shared_ptr <MyDB_Stats> MyDB_StatsPtr;

class MyDB_Stats {

public:

	// build a set of statistics from a set MyDB_Table
	MyDB_Stats (MyDB_TablePtr, string aliasName);

	// build a set of empty statistics
	MyDB_Stats ();

	// get the number of tuples for this table
	double getTupleCount ();

	// get the number of vals for this attribute (-1 if not there)
	double getAttVals (string whichAtt);

	// print the stats
	void print ();

	// cost a join
	MyDB_StatsPtr costJoin (vector <ExprTreePtr> &allDisjunctions, MyDB_StatsPtr RHS);

	// cost a selection predicte
	MyDB_StatsPtr costSelection (vector <ExprTreePtr> &allDisjunctions);

private:

	vector <pair <double, string>> allAtts;
	double tupleCount;
};

#endif

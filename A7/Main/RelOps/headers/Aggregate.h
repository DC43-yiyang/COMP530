
#ifndef AGG_H
#define AGG_H

#include "MyDB_TableReaderWriter.h"
#include <string>
#include <utility>
#include <vector>

// This class encapulates a simple, hash-based aggregation + group by.  It does not
// need to work when there is not enough space in the buffer manager to store all of
// the groups.

enum MyDB_AggType {sum, avg, cnt};

class Aggregate {

public:
	// This aggregates the table pointed to by input, writing the result to the
	// table pointed to by output.
	//
	// The parameter aggsToCompute lists, in order, the various aggregates that
	// should be computed.  For example, the list:
	//
	// <(SUM, "+([att1], [att2])"), (AVG, "+([att1], [att2])")>
	//
	// corresponds to:
	//
	// SELECT SUM (att1 + att2), AVG (att1 + att2) 
	// FROM input
	//
	// The parameter groupings lists all of the computations that we need to 
	// group by.  For example, if groupings had:
	//
	// <"[att4]", "-([att6], [att8])">
	//
	// Then the computation should produce:
	//
	// SELECT SUM (att1 + att2), AVG (att1 + att2), att4, att6 - att8
	// FROM input
	// GROUP BY att4, att6 - att8
	//	
	// Note that the aggregates always come after the grouping attributes in the output.
	//
	// If groupings is empty, then no GROUP BY is performed.
	// 
	// Finally, input records are excluded from the computation if they are not 
	// accepted by selectionPredicate.  This effectively acts like a WHERE clause.
	//
	Aggregate (MyDB_TableReaderWriterPtr input, MyDB_TableReaderWriterPtr output,
		vector <pair <MyDB_AggType, string>> aggsToCompute,
		vector <string> groupings, string selectionPredicate);
	
	// execute the aggregation
	void run ();

private:

	MyDB_TableReaderWriterPtr input;
	MyDB_TableReaderWriterPtr output;
	vector <pair <MyDB_AggType, string>> aggsToCompute;
	vector <string> groupings;
	string selectionPredicate;

};

#endif

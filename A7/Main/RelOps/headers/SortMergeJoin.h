
#ifndef SORT_JOIN_H
#define SORT_JOIN_H

#include "MyDB_TableReaderWriter.h"
#include <string>
#include <utility>
#include <vector>

// This class encapulates a sort merge join.  This is to be used in the case that the
// two input tables are too large to be stored in RAM.
//
class SortMergeJoin {

public:
	// This creates a sort merge join of the tables maanged by leftInput and rightInput.
	//
	// The string finalSelectionPredicate encodes the predicate over record from
	// the table managed by the variable output; only records for which this predicate
	// evaluates to true should be appended to the output table.
	//
	// As records are read in from leftInput (resp. rightInput), they should be 
	// discarded if the predicate encoded by leftSelectionPredicate (resp.
	// rightSelectionPredicate) does not evaluate to true.  
	//
	// Next, the pair equalityCheck encodes a pair of computations, taken from the 
	// predicte finalSelectionPredicate, that must match from the left and the right 
	// records, in order for the final record to be accepted by the predicate.
	// Basically, to run the join, you sort the left relation using equalityCheck.first.
	// You sort the right relation using equalityCheck.second.  Then you merge them
	// using equalityCheck.first and equalityCheck.second.
	//
	// Finally, the vector projections contains all of the computations that are
	// performed to create the output records from the join.
	//
	SortMergeJoin (MyDB_TableReaderWriterPtr leftInput, MyDB_TableReaderWriterPtr rightInput,
		MyDB_TableReaderWriterPtr output, string finalSelectionPredicate, 
		vector <string> projections,
		pair <string, string> equalityCheck, string leftSelectionPredicate,
		string rightSelectionPredicate);
	
	// execute the join
	void run ();

private:

	int runSize;
	string finalSelectionPredicate;
	pair <string, string> equalityCheck;
	vector <string> projections;
	MyDB_TableReaderWriterPtr output;
	MyDB_TableReaderWriterPtr leftTable;
	MyDB_TableReaderWriterPtr rightTable;
	string leftSelectionPredicate;
	string rightSelectionPredicate;
};

#endif

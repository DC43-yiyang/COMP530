
#ifndef BPLUS_SELECTION_H
#define BPLUS_SELECTION_H

#include "MyDB_AttVal.h"
#include "MyDB_TableReaderWriter.h"
#include "MyDB_BPlusTreeReaderWriter.h"
#include <string>
#include <utility>
#include <vector>

// this class encapsulates a selection run using a B-Plus Tree

class BPlusSelection {

public:
	//
	// The string selectionPredicate encodes the predicate to be executed.  
	//
	// In addition, only records having values between low and high (inclusive)
	// for the B+-Tree's sort att are returned by the selection. 

	// Te vector projections contains all of the computations that are
	// performed to create the output records (see the ScanJoin for an example).
	//
	// Record are read from input, and written to output.
	//
	BPlusSelection (MyDB_BPlusTreeReaderWriterPtr input, MyDB_TableReaderWriterPtr output,
		MyDB_AttValPtr low, MyDB_AttValPtr high,
		string selectionPredicate, vector <string> projections);
	
	// execute the selection operation
	void run ();

private:

	MyDB_BPlusTreeReaderWriterPtr input;
	MyDB_TableReaderWriterPtr output;
	MyDB_AttValPtr low;
	MyDB_AttValPtr high;
	string selectionPredicate;
	vector <string> projections;

};

#endif

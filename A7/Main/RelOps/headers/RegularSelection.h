
#ifndef REG_SELECTION_H
#define REG_SELECTION_H

#include "MyDB_TableReaderWriter.h"
#include <string>
#include <utility>
#include <vector>

// this class encapsulates a simple, scan-based selection

class RegularSelection {

public:
	//
	// The string selectionPredicate encodes the predicate to be executed.  
	//
	// The vector projections contains all of the computations that are
	// performed to create the output records (see the ScanJoin for an example).
	//
	// Record are read from input, and written to output.
	//
	RegularSelection (MyDB_TableReaderWriterPtr input, MyDB_TableReaderWriterPtr output,
		string selectionPredicate, vector <string> projections);
	
	// execute the selection operation
	void run ();

private:

        MyDB_TableReaderWriterPtr input;
        MyDB_TableReaderWriterPtr output;
        string selectionPredicate;
        vector <string> projections;
};

#endif

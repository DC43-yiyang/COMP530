

#ifndef REC_COMPARATOR_H
#define REC_COMPARATOR_H

#include "MyDB_Record.h"
#include <iostream>
using namespace std;

class RecordComparator {

public:

	RecordComparator (function <bool ()> comparatorIn, MyDB_RecordPtr lhsIn,  MyDB_RecordPtr rhsIn) {
		comparator = comparatorIn;
		lhs = lhsIn;
		rhs = rhsIn;
	}

	bool operator () (void *lhsPtr, void *rhsPtr) {
		lhs->fromBinary (lhsPtr);
		rhs->fromBinary (rhsPtr);
		return comparator ();	
	}

private:

	function <bool ()> comparator;
	MyDB_RecordPtr lhs;
	MyDB_RecordPtr rhs;

};

#endif

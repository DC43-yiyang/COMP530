
#ifndef ITERATOR_COMP_H
#define ITERATOR_COMP_H

#include <fstream>
#include <queue>
#include "MyDB_PageReaderWriter.h"
#include "MyDB_TableRecIterator.h"
#include "MyDB_TableRecIteratorAlt.h"
#include "MyDB_TableReaderWriter.h"

using namespace std;

// used as a comparator for record iterators
class IteratorComparator {

public:

	IteratorComparator (function <bool ()> comparatorIn, MyDB_RecordPtr lhsIn, MyDB_RecordPtr rhsIn) {
		comparator = comparatorIn;
		lhs = lhsIn;
		rhs = rhsIn;
	}

	IteratorComparator (const IteratorComparator &fromMe) {
		comparator = fromMe.comparator;
		lhs = fromMe.lhs;
		rhs = fromMe.rhs;
	}

	IteratorComparator () {}

	bool operator() (const MyDB_RecordIteratorAltPtr leftIter, const MyDB_RecordIteratorAltPtr rightIter) const {
		leftIter->getCurrent (lhs);
		rightIter->getCurrent (rhs);
		return !comparator ();
	}

private:
	function <bool ()> comparator;
	MyDB_RecordPtr lhs;
	MyDB_RecordPtr rhs;
};

#endif

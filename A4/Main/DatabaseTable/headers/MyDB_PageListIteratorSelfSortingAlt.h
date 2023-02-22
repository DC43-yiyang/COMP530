

#ifndef PAGE_LIST_ITER_SORTING_ALT_H
#define PAGE_LIST_ITER_SORTING_ALT_H

#include "MyDB_RecordIteratorAlt.h"
#include "MyDB_PageRecIteratorAlt.h"
#include "MyDB_PageReaderWriter.h"
#include "MyDB_Record.h"
#include <vector>

using namespace std;

class MyDB_PageListIteratorSelfSortingAlt : public MyDB_RecordIteratorAlt {

public:

        // load the current record into the parameter
        void getCurrent (MyDB_RecordPtr intoMe) override {
		myIter->getCurrent (intoMe);
	}

        // after a call to advance (), a call to getCurrentPointer () will get the address
        // of the record.  At a later time, it is then possible to reconstitute the record
        // by calling MyDB_Record.fromBinary (obtainedPointer)... ASSUMING that the page that
        // the record is located on has not been swapped out
        void *getCurrentPointer () {
		return myIter->getCurrentPointer ();
	}

        // advance to the next record... returns true if there is a next record, and
        // false if there are no more records to iterate over.  Not that this cannot
        // be called until after getCurrent () has been called
        bool advance () override {
		while (true) {
			if (myIter->advance ()) {
				myIter->getCurrent (myRec);
				if (!lowComparator () && !highComparator ()) {
					return true;
				}
			} else if (curPage == forUs.size () - 1) {
				return false;
			} else {
				curPage++;
				if (sortOrNot)
					forUs[curPage].sortInPlace (comparator, lhs, rhs);	
				myIter = forUs[curPage].getIteratorAlt ();
			}
		}
	}

	// destructor and contructor
	MyDB_PageListIteratorSelfSortingAlt (vector <MyDB_PageReaderWriter> &forUsIn, MyDB_RecordPtr lhsIn, 
		MyDB_RecordPtr rhsIn, function <bool ()> comparatorIn, MyDB_RecordPtr myRecIn, function <bool ()> lowComparatorIn, 
		function <bool ()> highComparatorIn, bool sortOrNotIn) {

		// just remember all of the parameters
		lhs = lhsIn;
		rhs = rhsIn;
		forUs = forUsIn;
		comparator = comparatorIn;				
		lowComparator = lowComparatorIn;
		highComparator = highComparatorIn;
		myRec = myRecIn;
		curPage = 0;
		sortOrNot = sortOrNotIn;

		// set up the first iterator, and we are ready to go!!
		if (sortOrNot)
			forUs[curPage].sortInPlace (comparator, lhs, rhs);	
		myIter = forUsIn[curPage].getIteratorAlt ();
	}

	~MyDB_PageListIteratorSelfSortingAlt () {}

private:

	MyDB_RecordIteratorAltPtr myIter;
	vector <MyDB_PageReaderWriter> forUs;
	MyDB_RecordPtr lhs, rhs;
	function <bool ()> comparator;
	function <bool ()> lowComparator;
	function <bool ()> highComparator;
	int curPage;
	bool sortOrNot;
	MyDB_RecordPtr myRec;
};

#endif

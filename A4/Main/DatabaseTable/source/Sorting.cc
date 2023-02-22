
#ifndef SORT_C
#define SORT_C

#include <queue>
#include "MyDB_PageReaderWriter.h"
#include "MyDB_TableRecIterator.h"
#include "MyDB_TableRecIteratorAlt.h"
#include "MyDB_TableReaderWriter.h"
#include "MyDB_RunQueueIteratorAlt.h"
#include "IteratorComparator.h"
#include "Sorting.h"

using namespace std;

void appendRecord (MyDB_PageReaderWriter &curPage, vector <MyDB_PageReaderWriter> &returnVal, 
	MyDB_RecordPtr appendMe, MyDB_BufferManagerPtr parent) {

	// try to append to the current page
	if (!curPage.append (appendMe)) {

		// if we cannot, then add a new one to the output vector
		returnVal.push_back (curPage);
		MyDB_PageReaderWriter temp (*parent);
		temp.append (appendMe);
		curPage = temp;
	}
}

vector <MyDB_PageReaderWriter> mergeIntoList (MyDB_BufferManagerPtr parent, MyDB_RecordIteratorAltPtr leftIter, 
	MyDB_RecordIteratorAltPtr rightIter, function <bool ()> comparator, MyDB_RecordPtr lhs, MyDB_RecordPtr rhs) {
	
	vector <MyDB_PageReaderWriter> returnVal;
	MyDB_PageReaderWriter curPage (*parent);
	bool lhsLoaded = false, rhsLoaded = false;

	// if one of the runs is empty, get outta here
	if (!leftIter->advance ()) {
		while (rightIter->advance ()) {
			rightIter->getCurrent (rhs);
			appendRecord (curPage, returnVal, rhs, parent);
		}
	} else if (!rightIter->advance ()) {
		do {
			leftIter->getCurrent (lhs);
			appendRecord (curPage, returnVal, lhs, parent);
		} while (leftIter->advance ());
	} else {
		while (true) {
	
			// get the two records

			// here's a bit of an optimization... if one of the records is loaded, don't re-load
			if (!lhsLoaded) {
				leftIter->getCurrent (lhs);
				lhsLoaded = true;
			}

			if (!rhsLoaded) {
				rightIter->getCurrent (rhs);		
				rhsLoaded = true;
			}
	
			// see if the lhs is less
			if (comparator ()) {
				appendRecord (curPage, returnVal, lhs, parent);
				lhsLoaded = false;

				// deal with the case where we have to append all of the right records to the output
				if (!leftIter->advance ()) {
					appendRecord (curPage, returnVal, rhs, parent);
					while (rightIter->advance ()) {
						rightIter->getCurrent (rhs);
						appendRecord (curPage, returnVal, rhs, parent);
					}
					break;
				}
			} else {
				appendRecord (curPage, returnVal, rhs, parent);
				rhsLoaded = false;

				// deal with the ase where we have to append all of the right records to the output
				if (!rightIter->advance ()) {
					appendRecord (curPage, returnVal, lhs, parent);
					while (leftIter->advance ()) {
						leftIter->getCurrent (lhs);
						appendRecord (curPage, returnVal, lhs, parent);
					}
					break;
				}
			}
		}
	}
	
	// remember the current page
	returnVal.push_back (curPage);
	
	// outta here!
	return returnVal;
}
	
MyDB_RecordIteratorAltPtr buildItertorOverSortedRuns (int runSize, MyDB_TableReaderWriter &sortMe, 
	function <bool ()> comparator, MyDB_RecordPtr lhs, MyDB_RecordPtr rhs) {

	return buildItertorOverSortedRuns (runSize, sortMe, comparator, lhs, rhs, "bool[true]");
}

MyDB_RecordIteratorAltPtr buildItertorOverSortedRuns (int runSize, MyDB_TableReaderWriter &sortMe, 
	function <bool ()> comparator, MyDB_RecordPtr lhs, MyDB_RecordPtr rhs, string lhsPred) {

	bool skipPred = false;
	if (lhsPred == "bool[true]")
		skipPred = true;

	func f = lhs->compileComputation (lhsPred);

	// this is the list of all of the pages in the file
	vector <vector<MyDB_PageReaderWriter>> allPages;

	// this is the pages making up the current run
	vector <vector<MyDB_PageReaderWriter>> pagesToSort;

	// this is the list of all of the iterators, with one for each run
	vector <MyDB_RecordIteratorAltPtr> runIters;
	
	// process the file 
	MyDB_PageReaderWriter tempPage (true, *sortMe.getBufferMgr ());
	for (int i = 0; i < sortMe.getNumPages (); i++) {
		
		if (sortMe[i].getType () == MyDB_PageType :: RegularPage) {

			if (skipPred) {
				vector <MyDB_PageReaderWriter> run;
				run.push_back (*(sortMe[i].sort (comparator, lhs, rhs)));	
				pagesToSort.push_back (run);
			} else {
				MyDB_RecordIteratorAltPtr temp = sortMe[i].getIteratorAlt ();
				while (temp->advance ()) {
					temp->getCurrent (lhs);

					if (!f ()->toBool ())
						continue;

					if (!tempPage.append (lhs)) {
	
						// remember the old page
						vector <MyDB_PageReaderWriter> run;
						run.push_back (*(tempPage.sort (comparator, lhs, rhs)));
						pagesToSort.push_back (run);
	
						// get the new page
						tempPage = MyDB_PageReaderWriter (true, *sortMe.getBufferMgr ());	
						temp->getCurrent (lhs);
						tempPage.append (lhs);
					}
				}
			}
		}

		// if we are all done, remember the last page
		if (i == sortMe.getNumPages () - 1) {
			vector <MyDB_PageReaderWriter> run;
			run.push_back (*(tempPage.sort (comparator, lhs, rhs)));
			pagesToSort.push_back (run);
		}

		// if we are not done reading this run, go on to the next one
		if (pagesToSort.size () != runSize && i != sortMe.getNumPages () - 1)
			continue;

		// while we don't have a single sorted list
		while (pagesToSort.size () > 1) {
			
			// the new version of the pages to sort vector
			vector <vector<MyDB_PageReaderWriter>> newPagesToSort;
	
			// repeatedly merge the last two pages
			while (pagesToSort.size () > 0) {
	
				// if there is one run, then just add it
				if (pagesToSort.size () == 1) {
					newPagesToSort.push_back (pagesToSort.back ());
					pagesToSort.pop_back ();
					continue;
				}
	
				// get the next two runs
				vector<MyDB_PageReaderWriter> runOne = pagesToSort.back ();
				pagesToSort.pop_back ();
				vector<MyDB_PageReaderWriter> runTwo = pagesToSort.back ();
				pagesToSort.pop_back ();
		
				// merge them
				newPagesToSort.push_back (mergeIntoList (sortMe.getBufferMgr (), getIteratorAlt (runOne), 
					getIteratorAlt (runTwo), comparator, lhs, rhs));
			}
	
			pagesToSort = newPagesToSort;
		}

		
		// now we have a single list, so create an iterator for it
		runIters.push_back (getIteratorAlt (pagesToSort[0]));

		// and start over on the next run
		pagesToSort.clear ();
	}
	
	// and now, we are ready to merge everything
	MyDB_RunQueueIteratorAltPtr temp = make_shared <MyDB_RunQueueIteratorAlt> (comparator, lhs, rhs);

	// load up the set
	for (MyDB_RecordIteratorAltPtr m : runIters) {
		if (m->advance ()) {
			temp->getQ ().push (m);
		}
	}

	return temp;
}


void sort (int runSize, MyDB_TableReaderWriter &sortMe, MyDB_TableReaderWriter &sortIntoMe,
	function <bool ()> comparator, MyDB_RecordPtr lhs, MyDB_RecordPtr rhs) {

	// get the sorted runs
	MyDB_RecordIteratorAltPtr myIter = buildItertorOverSortedRuns (runSize, sortMe, comparator, lhs, rhs);

	// and write everyone out
	while (myIter->advance ()) {
		myIter->getCurrent (lhs);
		sortIntoMe.append (lhs);
	}
}


#endif

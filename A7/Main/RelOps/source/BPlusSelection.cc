
#ifndef BPLUS_SELECTION_C                                        
#define BPLUS_SELECTION_C

#include "BPlusSelection.h"

BPlusSelection :: BPlusSelection (MyDB_BPlusTreeReaderWriterPtr inputIn, MyDB_TableReaderWriterPtr outputIn,
                MyDB_AttValPtr lowIn, MyDB_AttValPtr highIn,
                string selectionPredicateIn, vector <string> projectionsIn) {

	input = inputIn;
	output = outputIn;
	low = lowIn;
	high = highIn;
	selectionPredicate = selectionPredicateIn;
	projections = projectionsIn;
}

void BPlusSelection :: run () {

	MyDB_RecordPtr inputRec = input->getEmptyRecord ();
	MyDB_RecordPtr outputRec = output->getEmptyRecord ();
	
	// compile all of the coputations that we need here
	vector <func> finalComputations;
	for (string s : projections) {
		finalComputations.push_back (inputRec->compileComputation (s));
	}
	func pred = inputRec->compileComputation (selectionPredicate);

	// now, iterate through the B+-tree query results
	MyDB_RecordIteratorAltPtr myIter = input->getRangeIteratorAlt (low, high);
	while (myIter->advance ()) {

		myIter->getCurrent (inputRec);

		// see if it is accepted by the predicate
		if (!pred()->toBool ()) {
			continue;
		}

		// run all of the computations
		int i = 0;
		for (auto &f : finalComputations) {
			outputRec->getAtt (i++)->set (f());
		}

		outputRec->recordContentHasChanged ();
		output->append (outputRec);
	}
}

#endif

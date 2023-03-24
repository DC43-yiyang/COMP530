
#ifndef SCAN_JOIN_C
#define SCAN_JOIN_C

#include "MyDB_Record.h"
#include "MyDB_PageReaderWriter.h"
#include "MyDB_TableReaderWriter.h"
#include "ScanJoin.h"
#include <unordered_map>

using namespace std;

ScanJoin :: ScanJoin (MyDB_TableReaderWriterPtr leftInputIn, MyDB_TableReaderWriterPtr rightInputIn,
                MyDB_TableReaderWriterPtr outputIn, string finalSelectionPredicateIn,
		vector <string> projectionsIn,
                vector <pair <string, string>> equalityChecksIn, string leftSelectionPredicateIn,
                string rightSelectionPredicateIn) {

	output = outputIn;
	finalSelectionPredicate = finalSelectionPredicateIn;
	projections = projectionsIn;

	// we need to make sure that the left table is smaller

	// see which table is bigger 
	if (leftInputIn->getNumPages () < rightInputIn->getNumPages ()) {

		// if the left is smaller, we are good
		equalityChecks = equalityChecksIn;
		leftTable = leftInputIn;
		rightTable = rightInputIn;
		leftSelectionPredicate = leftSelectionPredicateIn;
		rightSelectionPredicate = rightSelectionPredicateIn;

		hadToSwapThem = false;

	} else {

		// if the right is smaller, swap everything
		for (auto &a : equalityChecksIn) {
			equalityChecks.push_back (make_pair (a.second, a.first));
		}
		rightTable = leftInputIn;
		leftTable = rightInputIn;
		rightSelectionPredicate = leftSelectionPredicateIn;
		leftSelectionPredicate = rightSelectionPredicateIn;

		hadToSwapThem = true;
	}
}

void ScanJoin :: run () {

	// this is the hash map we'll use to look up data... the key is the hashed value
	// of all of the records' join keys, and the value is a list of pointers were all
	// of the records with that hsah value are located
	unordered_map <size_t, vector <void *>> myHash;

	// get all of the pages
	vector <MyDB_PageReaderWriter> allData;
	for (int i = 0; i < leftTable->getNumPages (); i++) {
		MyDB_PageReaderWriter temp = leftTable->getPinned (i);
		if (temp.getType () == MyDB_PageType :: RegularPage)
			allData.push_back (leftTable->getPinned (i));
	}
	
	// get the left input record 
	MyDB_RecordPtr leftInputRec = leftTable->getEmptyRecord ();

	// and get the various functions whose output we'll hash
	vector <func> leftEqualities;
	for (auto &p : equalityChecks) {
		leftEqualities.push_back (leftInputRec->compileComputation (p.first));
	}

	// now get the predicate
	func leftPred = leftInputRec->compileComputation (leftSelectionPredicate);

	// add all of the records to the hash table
	MyDB_RecordIteratorAltPtr myIter = getIteratorAlt (allData);

	while (myIter->advance ()) {

		// hash the current record
		myIter->getCurrent (leftInputRec);

		// see if it is accepted by the preicate
		if (!leftPred ()->toBool ()) {
			continue;
		}

		// compute its hash
		size_t hashVal = 0;
		for (auto &f : leftEqualities) {
			hashVal ^= f ()->hash ();
		}

		// see if it is in the hash table
		myHash [hashVal].push_back (myIter->getCurrentPointer ());
	}

	// and now we iterate through the other table
	
	// get the right input record, and get the various functions over it
	MyDB_RecordPtr rightInputRec = rightTable->getEmptyRecord ();
	vector <func> rightEqualities;
	for (auto &p : equalityChecks) {
		rightEqualities.push_back (rightInputRec->compileComputation (p.second));
	}

	// now get the predicate
	func rightPred = rightInputRec->compileComputation (rightSelectionPredicate);

	// and get the schema that results from combining the left and right records
	MyDB_SchemaPtr mySchemaOut = make_shared <MyDB_Schema> ();
	for (auto &p : leftTable->getTable ()->getSchema ()->getAtts ())
		mySchemaOut->appendAtt (p);
	for (auto &p : rightTable->getTable ()->getSchema ()->getAtts ())
		mySchemaOut->appendAtt (p);

	// get the combined record
	MyDB_RecordPtr combinedRec = make_shared <MyDB_Record> (mySchemaOut);
	combinedRec->buildFrom (leftInputRec, rightInputRec);

	// now, get the final predicate over it
	func finalPredicate = combinedRec->compileComputation (finalSelectionPredicate);

	// and get the final set of computatoins that will be used to buld the output record
	vector <func> finalComputations;
	for (string s : projections) {
		finalComputations.push_back (combinedRec->compileComputation (s));
	}

	// this is the output record
	MyDB_RecordPtr outputRec = output->getEmptyRecord ();
	
	// now, iterate through the right table
	MyDB_RecordIteratorPtr myIterAgain = rightTable->getIterator (rightInputRec);
	while (myIterAgain->hasNext ()) {

		myIterAgain->getNext ();

		// see if it is accepted by the preicate
		if (!rightPred ()->toBool ()) {
			continue;
		}

		// hash the current record
		size_t hashVal = 0;
		for (auto &f : rightEqualities) {
			hashVal ^= f ()->hash ();
		}

		// get the list of potential matches... first verify that there IS
		// a match in there
		if (myHash.count (hashVal) == 0) {
			continue;
		}

		// if there is a match, then get the list of matches
		vector <void *> &potentialMatches = myHash [hashVal];
		
		// and iterate though the potential matches, checking each of them
		for (auto &v : potentialMatches) {

			// build the combined record
			leftInputRec->fromBinary (v);

			// check to see if it is accepted by the join predicate
			if (finalPredicate ()->toBool ()) {

				// run all of the computations
				int i = 0;
				for (auto &f : finalComputations) {
					outputRec->getAtt (i++)->set (f());
				}

				// the record's content has changed because it 
				// is now a composite of two records whose content
				// has changed via a read... we have to tell it this,
				// or else the record's internal buffer may cause it
				// to write old values
				outputRec->recordContentHasChanged ();
				output->append (outputRec);	
			}
		}
	}
}

#endif


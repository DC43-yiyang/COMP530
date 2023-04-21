
#ifndef SORTMERGE_CC
#define SORTMERGE_CC

#include "Aggregate.h"
#include "MyDB_Record.h"
#include "MyDB_PageReaderWriter.h"
#include "MyDB_TableReaderWriter.h"
#include "SortMergeJoin.h"
#include "Sorting.h"
#include <unordered_map>

SortMergeJoin :: SortMergeJoin (MyDB_TableReaderWriterPtr leftInputIn, MyDB_TableReaderWriterPtr rightInputIn,
                MyDB_TableReaderWriterPtr outputIn, string finalSelectionPredicateIn, 
                vector <string> projectionsIn,
                pair <string, string> equalityCheckIn, string leftSelectionPredicateIn,
                string rightSelectionPredicateIn) {

	output = outputIn;
	finalSelectionPredicate = finalSelectionPredicateIn;
	projections = projectionsIn;
	equalityCheck = equalityCheckIn;
	leftTable = leftInputIn;
	rightTable = rightInputIn; 
	leftSelectionPredicate = leftSelectionPredicateIn;
	rightSelectionPredicate = rightSelectionPredicateIn;
	runSize = leftTable->getBufferMgr ()->numPages / 2;
}

void SortMergeJoin :: run () {

	// get two left input records
	MyDB_RecordPtr leftInputRec = leftTable->getEmptyRecord ();
	MyDB_RecordPtr leftInputRecOther = leftTable->getEmptyRecord ();

	// get two right input records
	MyDB_RecordPtr rightInputRec = rightTable->getEmptyRecord ();
	MyDB_RecordPtr rightInputRecOther = rightTable->getEmptyRecord ();

	// build comparators over them
	function <bool ()> leftComp = buildRecordComparator (leftInputRec, leftInputRecOther, equalityCheck.first);
	function <bool ()> leftCompRev = buildRecordComparator (leftInputRecOther, leftInputRec, equalityCheck.first);
	function <bool ()> rightComp = buildRecordComparator (rightInputRec, rightInputRecOther, equalityCheck.second);

	// now, sort the left and the right
	MyDB_RecordIteratorAltPtr right = buildItertorOverSortedRuns (runSize, *rightTable, rightComp, rightInputRec, 
		rightInputRecOther, rightSelectionPredicate);
	MyDB_RecordIteratorAltPtr left = buildItertorOverSortedRuns (runSize, *leftTable, leftComp, leftInputRec, 
		leftInputRecOther, leftSelectionPredicate);

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
	
	// compares the two input recs
	func leftSmaller = combinedRec->compileComputation (" < (" + equalityCheck.first + ", " + equalityCheck.second + ")");
	func rightSmaller = combinedRec->compileComputation (" > (" + equalityCheck.first + ", " + equalityCheck.second + ")");
	func areEqual = combinedRec->compileComputation (" == (" + equalityCheck.first + ", " + equalityCheck.second + ")");
	
	// this is the output record
	MyDB_RecordPtr outputRec = output->getEmptyRecord ();

	// it is time to run the merge!!
	MyDB_PageReaderWriter lastPage (true, *(leftTable->getBufferMgr ()));
	vector <MyDB_PageReaderWriter> allPages;

	// if we have no results...
	if (!left->advance () || !right->advance ())
		return;
	
	// otherwise, loop until one side is exhausted
	while (true) {
	
		bool allDone = false;
		left->getCurrent (leftInputRec);
		right->getCurrent (rightInputRec);

		if (leftSmaller ()->toBool ()) {

			// try to move the left forward
			if (!left->advance ()) {
				allDone = true;
			}

		} else if (rightSmaller ()->toBool ()) {

			// try to move the right forward
			if (!right->advance ()) {
				allDone = true;
			}

		} else if (areEqual ()->toBool ()) {

			lastPage.clear ();
			allPages.clear ();
			allPages.push_back (lastPage);
			lastPage.append (leftInputRec);
			
			// get all of the LHS records that have the same key
			int counter = 1;
			while (true) {

				if (!left->advance ()) {
					//cout << "Got " << counter << " in the group.\n";
					allDone = true;
					break;
				}

				left->getCurrent (leftInputRecOther);

				// it is the same!!
				if (!leftComp () && !leftCompRev ()) {
					if (!lastPage.append (leftInputRecOther)) {
						MyDB_PageReaderWriter nextPage (true, *(leftTable->getBufferMgr ()));
						lastPage = nextPage;
						allPages.push_back (lastPage);
						lastPage.append (leftInputRecOther);
					}
					counter++;

				// it is not the same!!
				} else {
					//cout << "Got " << counter << " in the group.\n";
					break;
				}
			}

			// now, keep pulling off RHS records while we have the same key
			counter = 0;
			while (true) {
			
				// the records are the same!!
				if (areEqual ()->toBool ()) {

					//cout << rightInputRec << "\n";
					counter++;

					// now we check all of the RHS records that have the same key
					MyDB_RecordIteratorAltPtr myIterAgain;
					if (allPages.size () == 1) {
						myIterAgain = allPages[0].getIteratorAlt ();
					} else {
						myIterAgain = getIteratorAlt (allPages);
					}

					// check for a match
					while (myIterAgain->advance ()) {
						myIterAgain->getCurrent (leftInputRec);		
						if (finalPredicate ()->toBool ()) {
							// got one!!
							int i = 0;
							for (auto &f : finalComputations) {
								outputRec->getAtt (i++)->set (f());
							}
							outputRec->recordContentHasChanged ();
							output->append (outputRec);	
						}
					}
					
				// if not, we moved past the end
				} else {
					//cout << "\t" << counter << " matches.\n";
					break;
				}
				
				// and try to avance
				if (!right->advance ()) {
					//cout << "\t**" << counter << " matches.\n";
					allDone = true;
					break;
				}
				
				right->getCurrent (rightInputRec);
			}
		}

		// at this point, we check to see if we have completed
		if (allDone)
			break;
		
	}
	
}

#endif

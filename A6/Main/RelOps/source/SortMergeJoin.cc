
#ifndef SORTMERGE_CC
#define SORTMERGE_CC

#include "Aggregate.h"
#include "MyDB_Record.h"
#include "MyDB_PageReaderWriter.h"
#include "MyDB_TableReaderWriter.h"
#include "SortMergeJoin.h"
#include "Sorting.h"

SortMergeJoin :: SortMergeJoin (MyDB_TableReaderWriterPtr leftInputIn, MyDB_TableReaderWriterPtr rightInputIn,
                MyDB_TableReaderWriterPtr outputIn, string finalSelectionPredicateIn, 
                vector <string> projectionsIn,
                pair <string, string> equalityCheckIn, string leftSelectionPredicateIn,
                string rightSelectionPredicateIn) {

    this->output = outputIn;
	this->finalSelectionPredicate = finalSelectionPredicateIn;
	this->projections = projectionsIn;
	this->equalityCheck = equalityCheckIn;
	this->leftInput = leftInputIn;
	this->rightInput = rightInputIn; 
	this->leftSelectionPredicate = leftSelectionPredicateIn;
	this->rightSelectionPredicate = rightSelectionPredicateIn;

}


void SortMergeJoin :: run () {
    // Set up the run size
    int runSize = leftInput -> getBufferMgr() -> numPages / 2;
	
    // Create a output record
    MyDB_RecordPtr outputRec = output -> getEmptyRecord();
	
	// Create the two left and right input record separately
    MyDB_RecordPtr leftInputRec = leftInput -> getEmptyRecord();
    MyDB_RecordPtr leftInputRec2 = leftInput -> getEmptyRecord();

    MyDB_RecordPtr rightInputRec = rightInput -> getEmptyRecord();
    MyDB_RecordPtr rightInputRec2 = rightInput -> getEmptyRecord();
    
    // Create comparators to sort the left and right input tables using the provided join condition.
	function<bool()> leftComparator = buildRecordComparator(leftInputRec, leftInputRec2, equalityCheck.first);
    function<bool()> rightComparator = buildRecordComparator(rightInputRec, rightInputRec2, equalityCheck.second);
	
    // Sort the input tables: Sort both input tables and create sorted iterators for them.
	MyDB_RecordIteratorAltPtr sortedLeft = buildItertorOverSortedRuns(runSize, *leftInput, leftComparator, leftInputRec, leftInputRec2, leftSelectionPredicate);
    MyDB_RecordIteratorAltPtr sortedRight = buildItertorOverSortedRuns(runSize, *rightInput, rightComparator, rightInputRec, rightInputRec2, rightSelectionPredicate);

    // Set up the schema and combined record: Create a schema for the combined record by appending the attributes from both input tables. 
    // Create a combined record by building from the left and right input records.
    MyDB_SchemaPtr schemaOut = make_shared<MyDB_Schema>();
    for(auto &left : leftInput -> getTable() -> getSchema() -> getAtts()){
		schemaOut -> appendAtt(left);
	}       
    for(auto &right : rightInput -> getTable() -> getSchema() -> getAtts()){
		schemaOut -> appendAtt(right);
	}
        
    MyDB_RecordPtr combinedRec = make_shared<MyDB_Record>(schemaOut);
    combinedRec -> buildFrom(leftInputRec, rightInputRec);

    // Compile the predicate and projection computations: Compile the final selection predicate and projections for the combined record.
    func predicate = combinedRec -> compileComputation(finalSelectionPredicate);

    // Get the list of finalcomputations
    vector<func> finalComputations;
    for(string proj : projections){
        finalComputations.push_back(combinedRec -> compileComputation(proj));
    }

    // Define comparison functions: Define functions to compare the join keys of the sorted left and right input records.
    func areEqual = combinedRec -> compileComputation(" == (" + equalityCheck.first + ", " + equalityCheck.second + ")");
    func leftSmall = combinedRec -> compileComputation(" < (" + equalityCheck.first + ", " + equalityCheck.second + ")");
    func rightSmall = combinedRec -> compileComputation(" > (" + equalityCheck.first + ", " + equalityCheck.second + ")");
    
    // Initialize pages: Create pages to store equal join key values from both tables.
    vector<MyDB_PageReaderWriter> finalPages;
    vector<MyDB_PageReaderWriter> allRightPages;
    MyDB_PageReaderWriter lastPage(true, *(leftInput -> getBufferMgr()));
    MyDB_PageReaderWriter rightPage(true, *(rightInput -> getBufferMgr()));

    function<bool()> leftComparator2 = buildRecordComparator(leftInputRec2, leftInputRec, equalityCheck.first);

    if(sortedLeft -> advance() && sortedRight -> advance()){
        while(true){
            // Set a flag
            bool flag = false;
			
            sortedLeft -> getCurrent(leftInputRec);
            sortedRight -> getCurrent(rightInputRec);

            // If left and right are not equal, check for the next value on both side
            if(leftSmall() -> toBool() && !sortedLeft -> advance()){
                flag = true;
                break;                    
            }
            else if(rightSmall() -> toBool() && !sortedRight -> advance()){
                flag = true;
                break;  
            }
            // we could perform merge
			else if(areEqual()->toBool()){
                // Add left side and right side to the pages
                lastPage.clear();
                finalPages.clear();
                finalPages.push_back(lastPage);
                lastPage.append(leftInputRec);

                // Find left equal pages
                while(true){
                    if(!sortedLeft -> advance()){
                        flag = true;
                        break;
                    }
                    sortedLeft -> getCurrent(leftInputRec2); 
                    // If it is the same
                    if(!leftComparator() && !leftComparator2()){
                        if(!lastPage.append(leftInputRec2)){
                            MyDB_PageReaderWriter newPage(true, *(leftInput -> getBufferMgr()));
                            lastPage = newPage;
                            finalPages.push_back(lastPage);
                            lastPage.append(leftInputRec2);
                        }
                    } 
                    else{
                        break;
                    }
                }

                // Find right equal pages
                while(true){

                    if(areEqual() -> toBool()){
                        MyDB_RecordIteratorAltPtr myIterLeft;
                        myIterLeft = getIteratorAlt(finalPages);

                        // Check for the left and right match
                        while(myIterLeft -> advance()){
                            myIterLeft->getCurrent(leftInputRec);

                            // Check for the predicate
                            if(predicate() -> toBool()){
                                int i = 0;
                                for(auto &final : finalComputations){
                                    outputRec->getAtt(i++)->set(final());
                                }
                                // Add new record to the output table
                                outputRec -> recordContentHasChanged();
                                output -> append(outputRec);
                            }
                        }
                    } 
                    else{
                        break;
                    }
                    if(!sortedRight->advance()){
                        flag = true;
                        break;
                    }  
                    // Get to the next right page
                    sortedRight -> getCurrent(rightInputRec);
                }
            }

            if(flag){
                break;
            }       
        }
    }
}
#endif

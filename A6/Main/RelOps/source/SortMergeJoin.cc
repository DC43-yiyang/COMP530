
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

void SortMergeJoin :: run () {}

#endif

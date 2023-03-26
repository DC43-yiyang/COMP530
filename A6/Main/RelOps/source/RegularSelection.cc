
#ifndef REG_SELECTION_C                                        
#define REG_SELECTION_C

#include "RegularSelection.h"

RegularSelection :: RegularSelection (MyDB_TableReaderWriterPtr inputIn, MyDB_TableReaderWriterPtr outputIn,
                string selectionPredicateIn, vector <string> projectionsIn) {
    this->input = inputIn;
    this->output = outputIn;
    this->selectionPredicate = selectionPredicateIn;
    this->projections = projectionsIn;

}

void RegularSelection :: run () {}

#endif

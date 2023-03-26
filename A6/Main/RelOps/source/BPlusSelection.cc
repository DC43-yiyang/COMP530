
#ifndef BPLUS_SELECTION_C                                        
#define BPLUS_SELECTION_C

#include "BPlusSelection.h"

BPlusSelection :: BPlusSelection (MyDB_BPlusTreeReaderWriterPtr inputIn, MyDB_TableReaderWriterPtr outputIn,
                MyDB_AttValPtr lowIn, MyDB_AttValPtr highIn,
                string selectionPredicateIn, vector <string> projectionsIn) {
        this->input = inputIn;
        this->output = outputIn;
        this->low = lowIn;
        this->high = highIn;
        this->selectionPredicate = selectionPredicateIn;
        this->projections = projectionsIn;

}

void BPlusSelection :: run () {}

#endif

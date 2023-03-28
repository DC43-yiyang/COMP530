
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

void RegularSelection::run() {
    // Compile the selection predicate
    MyDB_RecordPtr inputRec = input->getEmptyRecord();
    func predicateFunc = inputRec->compileComputation(selectionPredicate);

    // Compile the projection functions
    vector<func> projectionFuncs;
    for (auto &proj : projections) {
        projectionFuncs.push_back(inputRec->compileComputation(proj));
    }

    // Initialize the output record
    MyDB_RecordPtr outputRec = output->getEmptyRecord();

    // Get an iterator over the input table
    MyDB_RecordIteratorPtr myIter = input->getIterator(inputRec);

    // Iterate through the records
    while (myIter->hasNext()) {
        myIter->getNext();

        // Check if the record satisfies the selection predicate
        if (!predicateFunc()->toBool()) {
            continue;
        }

        // Apply the projection functions to create the output record
        int i = 0;
        for (auto &projFunc : projectionFuncs) {
            outputRec->getAtt(i++)->set(projFunc());
        }
        outputRec->recordContentHasChanged();

        // Append the output record to the output table
        output->append(outputRec);
    }
}


#endif

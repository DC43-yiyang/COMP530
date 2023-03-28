
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

void BPlusSelection::run() {
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

    // Search for records in the B+ tree with keys between low and high (inclusive)
    MyDB_RecordIteratorAltPtr myIter = input->getRangeIteratorAlt(low, high);

    // Iterate through the records
    while (myIter->advance()) {
        myIter-> getCurrent(inputRec);

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

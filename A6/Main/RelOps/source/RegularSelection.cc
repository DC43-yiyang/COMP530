
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

// void RegularSelection :: run () {
//     // Get the input record
//     MyDB_RecordPtr inputRec = input -> getEmptyRecord();

//     // Now get the predicate
//     // The predicate must be true for record in order to be stored into ouput table
//     func predicate = inputRec -> compileComputation(selectionPredicate);

//     // Get the final set of computations that we will used to build the output record
//     vector <func> finalComputations;
//     for(string s : projections){
//         finalComputations.push_back(inputRec -> compileComputation(s));
//     }

//     // Create a ouput record
//     MyDB_RecordPtr outputRec = output -> getEmptyRecord();

//     // Now we iterate through the input record
//     MyDB_RecordIteratorAltPtr myIter = input -> getIteratorAlt();

//     while(myIter -> advance()){
//         myIter -> getCurrent(inputRec);

//         // Check to see if it is accept by the predicate
//         if(predicate() -> toBool()){
//             // run all computations
//             int i = 0;
//             for(auto &f : finalComputations){
//                 outputRec -> getAtt(i++) -> set(f());
//             }

//             // Now we add outputRec to the output table
//             outputRec -> recordContentHasChanged();
//             output -> append(outputRec);
//         }
//     }
// }
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

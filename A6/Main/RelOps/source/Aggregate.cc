
#ifndef AGG_CC
#define AGG_CC

#include "MyDB_Record.h"
#include "MyDB_PageReaderWriter.h"
#include "MyDB_TableReaderWriter.h"
#include "Aggregate.h"
#include <unordered_map>

using namespace std;

Aggregate :: Aggregate (MyDB_TableReaderWriterPtr inputIn, MyDB_TableReaderWriterPtr outputIn,
                vector <pair <MyDB_AggType, string>> aggsToComputeIn,
                vector <string> groupingsIn, string selectionPredicateIn) {
    this->input = inputIn;
    this->output = outputIn;
    this->aggsToCompute = aggsToComputeIn;
    this->groupings = groupingsIn;
    this->selectionPredicate = selectionPredicateIn;
}

void Aggregate :: run () {}

#endif


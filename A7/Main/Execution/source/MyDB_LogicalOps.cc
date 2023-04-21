
#ifndef LOG_OP_CC
#define LOG_OP_CC

#include "MyDB_LogicalOps.h"

// fill this out!  This should actually run the aggregation via an appropriate RelOp, and then it is going to
// have to unscramble the output attributes and compute exprsToCompute using an execution of the RegularSelection 
// operation (why?  Note that the aggregate always outputs all of the grouping atts followed by the agg atts.
// After, a selection is required to compute the final set of aggregate expressions)
//
// Note that after the left and right hand sides have been executed, the temporary tables associated with the two 
// sides should be deleted (via a kill to killFile () on the buffer manager)
MyDB_TableReaderWriterPtr LogicalAggregate :: execute () {
	return nullptr;

}
// we don't really count the cost of the aggregate, so cost its subplan and return that
pair <double, MyDB_StatsPtr> LogicalAggregate :: cost () {
	return inputOp->cost ();
}
	
// this costs the entire query plan with the join at the top, returning the compute set of statistics for
// the output.  Note that it recursively costs the left and then the right, before using the statistics from
// the left and the right to cost the join itself
pair <double, MyDB_StatsPtr> LogicalJoin :: cost () {
	auto left = leftInputOp->cost ();
	auto right = rightInputOp->cost ();
	MyDB_StatsPtr outputStats = left.second->costJoin (outputSelectionPredicate, right.second);
	return make_pair (left.first + right.first + outputStats->getTupleCount (), outputStats);
}
	
// Fill this out!  This should recursively execute the left hand side, and then the right hand side, and then
// it should heuristically choose whether to do a scan join or a sort-merge join (if it chooses a scan join, it
// should use a heuristic to choose which input is to be hashed and which is to be scanned), and execute the join.
// Note that after the left and right hand sides have been executed, the temporary tables associated with the two 
// sides should be deleted (via a kill to killFile () on the buffer manager)
MyDB_TableReaderWriterPtr LogicalJoin :: execute () {
	return nullptr;
}

// this costs the table scan returning the compute set of statistics for the output
pair <double, MyDB_StatsPtr> LogicalTableScan :: cost () {
	MyDB_StatsPtr returnVal = inputStats->costSelection (selectionPred);
	return make_pair (returnVal->getTupleCount (), returnVal);	
}

// fill this out!  This should heuristically choose whether to use a B+-Tree (if appropriate) or just a regular
// table scan, and then execute the table scan using a relational selection.  Note that a desirable optimization
// is to somehow set things up so that if a B+-Tree is NOT used, that the table scan does not actually do anything,
// and the selection predicate is handled at the level of the parent (by filtering, for example, the data that is
// input into a join)
MyDB_TableReaderWriterPtr LogicalTableScan :: execute () {
	return nullptr;
}

#endif

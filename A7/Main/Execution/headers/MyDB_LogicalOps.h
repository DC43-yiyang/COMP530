
#ifndef LOG_OP_H
#define LOG_OP_H

#include "MyDB_Stats.h"
#include "MyDB_TableReaderWriter.h"
#include "ExprTree.h"


// create a smart pointer for database tables
using namespace std;
class LogicalOp;
typedef shared_ptr <LogicalOp> LogicalOpPtr;

// this is a pure virtual class that corresponds to a particular relational operation to be run
class LogicalOp {

public:

	// get the total cost to execute the logical plan, up to and including this loical operation.
	// Note that the MyDB_StatsPtr will point to the set of statistics that come out of executing this plan
	virtual pair <double, MyDB_StatsPtr> cost () = 0;

	// execute the entire plan (executing the children first), then execute this logical operation, and
	// once this operation has been executed, delete the temporary tables associated with child operations
	virtual MyDB_TableReaderWriterPtr execute () = 0;

	virtual ~LogicalOp () {}
};

// a logical aggregation operation--in practice, this is going to be implemented using an Aggregate operation,
// followed by a RegularSelection operation to de-scramble the output attributes (since the Aggregate always has
// the grouping atts first, followed by the aggregates, and this might not be the order that is given in 
// exprsToCompute)
//
class LogicalAggregate : public LogicalOp {

public: 

	//
	// inputOp: this is the input operation that we are reading from
	// outputSpec: this is the table (location and schema) that we are going to create by running the operation
	//    Note that that each attribute in this output spec should have a 1-1 correspondence with the expressions
	//    in exprsToCompute
	// exprsToCompute: the various projections to compute... the first item in exprsToComput corresponds to the
	//    first attribute in outputSpec, the second item in exprsToComput corresponds to the second attribute, etc.
	// groupings: the various GROUP BY computations we need to perform
	//
	LogicalAggregate (LogicalOpPtr inputOp, MyDB_TablePtr outputSpec, vector <ExprTreePtr> &exprsToCompute, 
		vector <ExprTreePtr> &groupings) : inputOp (inputOp), outputSpec (outputSpec), exprsToCompute (exprsToCompute),
		groupings (groupings) {}
	
	// fill this out!  This should actually run the aggregation via an appropriate RelOp, and then it is going to
	// have to unscramble the output attributes and compute exprsToCompute using an execution of the RegularSelection 
	// operation (why?  Note that the aggregate always outputs all of the grouping atts followed by the agg atts.
	// After, a selection is required to compute the final set of aggregate expressions)
	//
	// Note that after the subquery is executed (as well as any operations that produce temp data) any temporary 
	// database tables should be deleted (via a kill to killTable () on the buffer manager)
	MyDB_TableReaderWriterPtr execute ();

	// we don't really count the cost of the aggregate, so cost its subplan and return that
	pair <double, MyDB_StatsPtr> cost ();
	
private:

	LogicalOpPtr inputOp;
	MyDB_TablePtr outputSpec;
	vector <ExprTreePtr> exprsToCompute;
	vector <ExprTreePtr> groupings;

};
	
// a logical join operation---will be implemented with a ScanJoin or a SortMergeJoin
class LogicalJoin  : public LogicalOp {

public:

	//
	// leftInputOp: this is the input operation that we are reading from on the left
	// rightInputOp: this is the input operation that we are reading from on the left
	// outputSpec: this is the table (location and schema) that we are going to create by running the operation
	//    note that when "cost" is called, statistics should be returned for each of the attributes in the schema
	//    associated with outputSpec.  Note that each attribute in this output spec should have a 1-1 correspondence
	//    with the expressions in exprsToCompute
	// selectionPred: the selection predicate to execute using the join
	// exprsToCompute: the various projections to compute... the first item in exprsToComput corresponds to the
	//    first attribute in outputSpec, the second item in exprsToComput corresponds to the second attribute, etc.
	//	
	LogicalJoin (LogicalOpPtr leftInputOp, LogicalOpPtr rightInputOp, MyDB_TablePtr outputSpec,
		vector <ExprTreePtr> &outputSelectionPredicate, vector <ExprTreePtr> &exprsToCompute) : leftInputOp (leftInputOp),
		rightInputOp (rightInputOp), outputSpec (outputSpec), outputSelectionPredicate (outputSelectionPredicate),
		exprsToCompute (exprsToCompute) {}
			
	// this costs the entire query plan with the join at the top, returning the compute set of statistics for
	// the output.  Note that it recursively costs the left and then the right, before using the statistics from
	// the left and the right to cost the join itself
	pair <double, MyDB_StatsPtr> cost ();
	
	// Fill this out!  This should recursively execute the left hand side, and then the right hand side, and then
	// it should heuristically choose whether to do a scan join or a sort-merge join (if it chooses a scan join, it
	// should use a heuristic to choose which input is to be hashed and which is to be scanned), and execute the join.
	// Note that after the left and right hand sides have been executed, the temporary tables associated with the two 
	// sides should be deleted (via a kill to killTable () on the buffer manager)
	MyDB_TableReaderWriterPtr execute ();

private:

	LogicalOpPtr leftInputOp;
	LogicalOpPtr rightInputOp;
	MyDB_TablePtr outputSpec;
	vector <ExprTreePtr> outputSelectionPredicate;
	vector <ExprTreePtr> exprsToCompute;

};

// a logical table scan operation---will be implemented with a BPlusSelection or a RegularSelection... note that
// we assume that we only operate table scans over base tables, and not tables that are created as the result of
// running another logical operation
class LogicalTableScan : public LogicalOp {

public:

	//
	// inputSpec: this is the input table that we are operating over 
	// outputSpec: this is the table (location and schema) that we are going to create by running the operation
	//    note that when cost is called, statistics should be returned for each of the attributes in the schema
	//    associated with outputSpec
	// inputStats: the complete set of input statistics, going into this table scan.  Statistics should be included
	//    for each of the attributes in the table in inputSpec
	// selectionPred: the selection predicate to execute
	// exprsToCompute: the various projections to compute... for a logical table scan, this is always just a set
	//    of attributes that we are asking for from a base table
	//
	LogicalTableScan (MyDB_TableReaderWriterPtr inputSpec, MyDB_TablePtr outputSpec, MyDB_StatsPtr inputStats, 
		vector <ExprTreePtr> &selectionPred, vector <string> &exprsToCompute) : inputSpec (inputSpec), outputSpec (outputSpec),
		inputStats (inputStats), selectionPred (selectionPred), exprsToCompute (exprsToCompute) {}

	// this costs the table scan returning the compute set of statistics for the output
	pair <double, MyDB_StatsPtr> cost ();

	// fill this out!  This should heuristically choose whether to use a B+-Tree (if appropriate) or just a regular
	// table scan, and then execute the table scan using a relational selection.  Note that a desirable optimization
	// is to somehow set things up so that if a B+-Tree is NOT used, that the table scan does not actually do anything,
	// and the selection predicate is handled at the level of the parent (by filtering, for example, the data that is
	// input into a join)
	MyDB_TableReaderWriterPtr execute ();

private:

	MyDB_TableReaderWriterPtr inputSpec;
	MyDB_TablePtr outputSpec;
	MyDB_StatsPtr inputStats;
        vector <ExprTreePtr> selectionPred;
	vector <string> exprsToCompute;
};

#endif


#ifndef SCAN_JOIN_H
#define SCAN_JOIN_H

#include "MyDB_TableReaderWriter.h"
#include <string>
#include <utility>
#include <vector>

// This class encapulates a scan join, where one table is hashed, and then the 
// other is scanned and joined with the hashed table.  If the smaller table is
// too large to be stored in the buffer manager in its entirity, then the join
// will fail.
//
class ScanJoin {

public:
	// This creates a scan join of the tables maanged by leftInput and rightInput.
	// The smaller of the two tables is stored in its entirity into a hash table, then
	// the larger of the tables is scanned, and the output written to the table
	// managed by the variable output.
	//
	// The string finalSelectionPredicate encodes the predicate over records
	// created by appending records from leftInput and rightInput.  Only records
	// for which this predicate evaluates to true should be appended to the output
	// table.
	//
	// As records are read in from leftInput (resp. rightInput), they should be 
	// discarded if the predicate encoded by leftSelectionPredicate (resp.
	// rightSelectionPredicate) does not evaluate to true.  
	//
	// Next, the vector equalityChecks encodes a bunch of pairs of computations,
	// taken from the predicte finalSelectionPredicate, that must match from the 
	// left and the right records, in order for the final record to be accepted
	// by the predicate. These are used to perform the hashing required by the 
	// ScanJoin object.  For example, if the final selection predicate (in SQL)
	// is (left.att1 = right.att2 + 4) and (left.att4 = right.att5) and (left.att5 > 6)
	// then equalityChecks would might contain strings encoding the pairs:
	//
	// <left.att1, right.att2 + 4>
	// <left.att4, right.att5>
	//
	// For a (left, right) pair to be accepted by finalSelectionPredicate, both of 
	// these pairs of computations must match up.  This is useful because we can
	// hash a left record on (left.att1) and (left.att4), and hash a right record
	// on (right.att2 + 4) and (right.att5); for the pair to be in the result set,
	// the two hashed must match.  
	//
	// Finally, the vector projections contains all of the computations that are
	// performed to create the output records from the join.
	//
	// For example, given that both supplierLeft and supplierRight have the 
	// "supplier" schema (and assuming that all atts in supplierLeft are pre-pended
	// with "l_" and all of the atts in supplierRight are pre-pended with "r_", then
	// we can compute the following with a scan join:
        //
        // SELECT supplierLeft.l_name, supplierLeft.l_comment + " " + supplierRight.r_comment
        // FROM supplierLeft, supplierRight
        // WHERE (supplierLeft.l_nationkey = 4 OR
        //        supplierLeft.l_nationkey = 3) AND
        //       (supplierRight.r_nationkey = 3) AND
        //       (supplierLeft.l_suppkey = supplierRight.r_suppkey) AND
        //       (supplierLeft.l_name = supplierRight.r_name)
        //
	// The code is roughlt as follows:
	//
        //      vector <pair <string, string>> hashAtts;
        //      hashAtts.push_back (make_pair (string ("[l_suppkey]"), string ("[r_suppkey]")));
        //      hashAtts.push_back (make_pair (string ("[l_name]"), string ("[r_name]")));
	//
        //      vector <string> projections;
        //      projections.push_back ("[l_name]");
        //      projections.push_back ("+ (+ ([l_comment], string[ ]), [r_comment])");
	//
        //      ScanJoin myOp (supplierTableL, supplierTableR, supplierTableOut,
        //         "&& ( == ([l_suppkey], [r_suppkey]), == ([l_name], [r_name]))", projections, hashAtts,
        //         "|| ( == ([l_nationkey], int[3]), == ([l_nationkey], int[4]))",
        //         "== ([r_nationkey], int[3])");
	//
	ScanJoin (MyDB_TableReaderWriterPtr leftInput, MyDB_TableReaderWriterPtr rightInput,
		MyDB_TableReaderWriterPtr output, string finalSelectionPredicate, 
		vector <string> projections,
		vector <pair <string, string>> equalityChecks, string leftSelectionPredicate,
		string rightSelectionPredicate);
	
	// execute the join
	void run ();

private:

	string finalSelectionPredicate;
	vector <pair <string, string>> equalityChecks;
	vector <string> projections;
	MyDB_TableReaderWriterPtr output;	
	MyDB_TableReaderWriterPtr leftTable;
	MyDB_TableReaderWriterPtr rightTable;
	string leftSelectionPredicate;
	string rightSelectionPredicate;
	bool hadToSwapThem;
};

#endif

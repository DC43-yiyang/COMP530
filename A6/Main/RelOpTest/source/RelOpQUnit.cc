
#ifndef BPLUS_TEST_H
#define BPLUS_TEST_H

#include "MyDB_AttType.h"  
#include "MyDB_BufferManager.h"
#include "MyDB_Catalog.h"  
#include "MyDB_Page.h"
#include "MyDB_PageReaderWriter.h"
#include "MyDB_Record.h"
#include "MyDB_Table.h"
#include "MyDB_TableReaderWriter.h"
#include "MyDB_BPlusTreeReaderWriter.h"
#include "MyDB_Schema.h"
#include "QUnit.h"
#include "Sorting.h"
#include "ScanJoin.h"
#include "Aggregate.h"
#include "BPlusSelection.h"
#include "RegularSelection.h"
#include "ScanJoin.h"
#include "SortMergeJoin.h"
#include <iostream>
#include <vector>
#include <utility>

using namespace std;

int main () {

	QUnit::UnitTest qunit(cerr, QUnit::verbose);

	// create a catalog
	MyDB_CatalogPtr myCatalog = make_shared <MyDB_Catalog> ("catFile");

	// now make a schema
	MyDB_SchemaPtr mySchemaL = make_shared <MyDB_Schema> ();
	mySchemaL->appendAtt (make_pair ("l_suppkey", make_shared <MyDB_IntAttType> ()));
	mySchemaL->appendAtt (make_pair ("l_name", make_shared <MyDB_StringAttType> ()));
	mySchemaL->appendAtt (make_pair ("l_address", make_shared <MyDB_StringAttType> ()));
	mySchemaL->appendAtt (make_pair ("l_nationkey", make_shared <MyDB_IntAttType> ()));
	mySchemaL->appendAtt (make_pair ("l_phone", make_shared <MyDB_StringAttType> ()));
	mySchemaL->appendAtt (make_pair ("l_acctbal", make_shared <MyDB_DoubleAttType> ()));
	mySchemaL->appendAtt (make_pair ("l_comment", make_shared <MyDB_StringAttType> ()));

	// and a right schema
	MyDB_SchemaPtr mySchemaR = make_shared <MyDB_Schema> ();
	mySchemaR->appendAtt (make_pair ("r_suppkey", make_shared <MyDB_IntAttType> ()));
	mySchemaR->appendAtt (make_pair ("r_name", make_shared <MyDB_StringAttType> ()));
	mySchemaR->appendAtt (make_pair ("r_address", make_shared <MyDB_StringAttType> ()));
	mySchemaR->appendAtt (make_pair ("r_nationkey", make_shared <MyDB_IntAttType> ()));
	mySchemaR->appendAtt (make_pair ("r_phone", make_shared <MyDB_StringAttType> ()));
	mySchemaR->appendAtt (make_pair ("r_acctbal", make_shared <MyDB_DoubleAttType> ()));
	mySchemaR->appendAtt (make_pair ("r_comment", make_shared <MyDB_StringAttType> ()));

	// use the schema to create a table
	MyDB_TablePtr myTableLeft = make_shared <MyDB_Table> ("supplierLeft", "supplierLeft.bin", mySchemaL);
	MyDB_TablePtr myTableRight = make_shared <MyDB_Table> ("supplierRight", "supplierRight.bin", mySchemaR);

	// get the tables
	MyDB_BufferManagerPtr myMgr = make_shared <MyDB_BufferManager> (131072, 128, "tempFile");
	MyDB_TableReaderWriterPtr supplierTableL = make_shared <MyDB_TableReaderWriter> (myTableLeft, myMgr);
	MyDB_TableReaderWriterPtr supplierTableRNoBPlus = make_shared <MyDB_TableReaderWriter> (myTableRight, myMgr);

	// load up from a text file
	cout << "loading left table.\n";
	supplierTableL->loadFromTextFile ("supplier.tbl");
	cout << "loading right table.\n";
	supplierTableRNoBPlus->loadFromTextFile ("supplierBig.tbl");

	{
		// get the output schema and table
		MyDB_SchemaPtr mySchemaOut = make_shared <MyDB_Schema> ();
		mySchemaOut->appendAtt (make_pair ("l_name", make_shared <MyDB_StringAttType> ()));
		mySchemaOut->appendAtt (make_pair ("l_nationkey", make_shared <MyDB_StringAttType> ()));
		mySchemaOut->appendAtt (make_pair ("combined_stuff", make_shared <MyDB_StringAttType> ()));
		MyDB_TablePtr myTableOut = make_shared <MyDB_Table> ("supplierOut", "supplierOut.bin", mySchemaOut);
		MyDB_TableReaderWriterPtr supplierTableOut = make_shared <MyDB_TableReaderWriter> (myTableOut, myMgr);

		vector <string> projections;
		projections.push_back ("[l_name]");
		projections.push_back ("[l_nationkey]");
		projections.push_back ("+ (+ (+ ([l_phone], string[ ]), + ([l_acctbal], string[ ])), [l_comment])");

		RegularSelection myOp (supplierTableL, supplierTableOut, "== ([l_nationkey], int[1])", projections);
		myOp.run ();

		cout << "\nRunning selection.";
		cout << "\nFirst result should be:\n";
		cout << "Supplier#000000003|1|11-383-516-1199 4192.400000 furiously regular instructions impress slyly! carefu|\n\n";
                MyDB_RecordPtr temp = supplierTableOut->getEmptyRecord ();
                MyDB_RecordIteratorAltPtr myIter = supplierTableOut->getIteratorAlt ();

                while (myIter->advance ()) {
                        myIter->getCurrent (temp);
			cout << temp << "\n";
		}

		// now, we count the total number of records 
		vector <pair <MyDB_AggType, string>> aggsToCompute;
		aggsToCompute.push_back (make_pair (MyDB_AggType :: cnt, "int[0]"));

		vector <string> groupings;
		MyDB_SchemaPtr mySchemaOutAgain  = make_shared <MyDB_Schema> ();
		mySchemaOutAgain->appendAtt (make_pair ("mycnt", make_shared <MyDB_IntAttType> ()));
		MyDB_TablePtr aggTable = make_shared <MyDB_Table> ("aggOut", "aggOut.bin", mySchemaOutAgain);
		MyDB_TableReaderWriterPtr aggTableOut = make_shared <MyDB_TableReaderWriter> (aggTable, myMgr);

		Aggregate myOpAgain (supplierTableOut, aggTableOut, aggsToCompute, groupings, "bool[true]");
		cout << "running aggregate\n";
		myOpAgain.run ();
		
                temp = aggTableOut->getEmptyRecord ();
                myIter = aggTableOut->getIteratorAlt ();

		cout << "Now we count the records.";
		cout << "\nThe output should be 413:\n";
                while (myIter->advance ()) {
                        myIter->getCurrent (temp);
			cout << temp << "\n";
		}
	}

	{
		// get the output schema and table
		MyDB_SchemaPtr mySchemaOut = make_shared <MyDB_Schema> ();
		mySchemaOut->appendAtt (make_pair ("l_name", make_shared <MyDB_StringAttType> ()));
		mySchemaOut->appendAtt (make_pair ("l_nationkey", make_shared <MyDB_StringAttType> ()));
		mySchemaOut->appendAtt (make_pair ("combined_stuff", make_shared <MyDB_StringAttType> ()));
		MyDB_TablePtr myTableOut = make_shared <MyDB_Table> ("supplierOut", "supplierOut.bin", mySchemaOut);
		MyDB_TableReaderWriterPtr supplierTableOut = make_shared <MyDB_TableReaderWriter> (myTableOut, myMgr);

		vector <string> projections;
		projections.push_back ("[l_name]");
		projections.push_back ("[l_nationkey]");
		projections.push_back ("+ (+ (+ ([l_phone], string[ ]), + ([l_acctbal], string[ ])), [l_comment])");

		RegularSelection myOp (supplierTableL, supplierTableOut, "&& (== ([l_nationkey], int[1]), > ([l_name], string [Supplier#000009378]))", projections);
		myOp.run ();

		cout << "\nFirst result should be:\n";
		cout << "Supplier#000009428|1|11-896-966-5146 5429.370000 furiously regular pinto beans caj|\n\n";
                MyDB_RecordPtr temp = supplierTableOut->getEmptyRecord ();
                MyDB_RecordIteratorAltPtr myIter = supplierTableOut->getIteratorAlt ();

                while (myIter->advance ()) {
                        myIter->getCurrent (temp);
			cout << temp << "\n";
		}

		// now, we count the total number of records 
		vector <pair <MyDB_AggType, string>> aggsToCompute;
		aggsToCompute.push_back (make_pair (MyDB_AggType :: cnt, "int[0]"));

		vector <string> groupings;
		MyDB_SchemaPtr mySchemaOutAgain  = make_shared <MyDB_Schema> ();
		mySchemaOutAgain->appendAtt (make_pair ("mycnt", make_shared <MyDB_IntAttType> ()));
		MyDB_TablePtr aggTable = make_shared <MyDB_Table> ("aggOut", "aggOut.bin", mySchemaOutAgain);
		MyDB_TableReaderWriterPtr aggTableOut = make_shared <MyDB_TableReaderWriter> (aggTable, myMgr);

		Aggregate myOpAgain (supplierTableOut, aggTableOut, aggsToCompute, groupings, "bool[true]");
		cout << "running aggregate\n";
		myOpAgain.run ();
		
                temp = aggTableOut->getEmptyRecord ();
                myIter = aggTableOut->getIteratorAlt ();

		cout << "Now we count the records.";
		cout << "\nThe output should be 29:\n";
                while (myIter->advance ()) {
                        myIter->getCurrent (temp);
			cout << temp << "\n";
		}
	}
	
	{
		
		// get the output schema and table
		MyDB_SchemaPtr mySchemaOut = make_shared <MyDB_Schema> ();
		mySchemaOut->appendAtt (make_pair ("l_name", make_shared <MyDB_StringAttType> ()));
		mySchemaOut->appendAtt (make_pair ("combined_comment", make_shared <MyDB_StringAttType> ()));
		MyDB_TablePtr myTableOut = make_shared <MyDB_Table> ("supplierOut", "supplierOut.bin", mySchemaOut);
		MyDB_TableReaderWriterPtr supplierTableOut = make_shared <MyDB_TableReaderWriter> (myTableOut, myMgr);

		// This basically runs:
		//
		// SELECT supplierLeft.l_name, supplierLeft.l_comment + " " + supplierRight.r_comment
		// FROM supplierLeft, supplierRight
		// WHERE (supplierLeft.l_nationkey = 4 OR
		//        supplierLeft.l_nationkey = 3) AND
		//       (supplierRight.r_nationkey = 3) AND
		//       (supplierLeft.l_suppkey = supplierRight.r_suppkey) AND
		//       (supplierLeft.l_name = supplierRight.r_name)
		// 
		// It does this by hashing the smaller table (supplierLeft) on 
		// supplierLeft.l_suppkey and supplierLeft.l_name.  It then scans
		// supplierRight, probing the hash table for matches

		vector <pair <string, string>> hashAtts;
		hashAtts.push_back (make_pair (string ("[l_suppkey]"), string ("[r_suppkey]")));
		hashAtts.push_back (make_pair (string ("[l_name]"), string ("[r_name]")));

		vector <string> projections;
		projections.push_back ("[l_name]");
		projections.push_back ("+ (+ ([l_comment], string[ ]), [r_comment])");

		cout << "Do you want to run a:\n";
		cout << "\t1. Sort merge join.\n";
		cout << "\t2. Scan join.\n";
		cout << "Enter 1 or 2:\n";
		int res;
		cin >> res;

		if (res == 2) {
			ScanJoin myOp (supplierTableL, supplierTableRNoBPlus, supplierTableOut, 
				"&& ( == ([l_suppkey], [r_suppkey]), == ([l_name], [r_name]))", projections, hashAtts,
				"|| ( == ([l_nationkey], int[3]), == ([l_nationkey], int[4]))",
				"== ([r_nationkey], int[3])");
			cout << "running join\n";
			myOp.run ();
		} else if (res == 1) {
			SortMergeJoin myOp (supplierTableL, supplierTableRNoBPlus, supplierTableOut, 
				"&& ( == ([l_suppkey], [r_suppkey]), == ([l_name], [r_name]))", projections, 
				make_pair (string ("[l_suppkey]"), string ("[r_suppkey]")),
				"|| ( == ([l_nationkey], int[3]), == ([l_nationkey], int[4]))",
				"== ([r_nationkey], int[3])");
			cout << "running join\n";
			myOp.run ();
		} else {
			cout << "I said 1 or 2!!!\n";
			return 3;
		}

		// now, we count the total number of records with each nation name
		vector <pair <MyDB_AggType, string>> aggsToCompute;
		aggsToCompute.push_back (make_pair (MyDB_AggType :: cnt, "int[0]"));

		vector <string> groupings;
		groupings.push_back ("[l_name]");

		MyDB_SchemaPtr mySchemaOutAgain  = make_shared <MyDB_Schema> ();
		mySchemaOutAgain->appendAtt (make_pair ("l_name", make_shared <MyDB_StringAttType> ()));
		mySchemaOutAgain->appendAtt (make_pair ("mycnt", make_shared <MyDB_IntAttType> ()));
		MyDB_TablePtr aggTable = make_shared <MyDB_Table> ("aggOut", "aggOut.bin", mySchemaOutAgain);
		MyDB_TableReaderWriterPtr aggTableOut = make_shared <MyDB_TableReaderWriter> (aggTable, myMgr);

		Aggregate myOpAgain (supplierTableOut, aggTableOut, aggsToCompute, groupings, 
			"&& ( > ([l_name], string[Supplier#000002243]), < ([l_name], string[Supplier#000002303]))");
		cout << "running aggregate\n";
		myOpAgain.run ();
		
                MyDB_RecordPtr temp = aggTableOut->getEmptyRecord ();
                MyDB_RecordIteratorAltPtr myIter = aggTableOut->getIteratorAlt ();

		cout << "\nThe output should be, in some order:\n";
		cout << "Supplier#000002245|32|\n";
		cout << "Supplier#000002264|32|\n";
		cout << "Supplier#000002265|32|\n";
		cout << "Supplier#000002272|32|\n";
		cout << "Supplier#000002282|32|\n";
		cout << "\nHere goes:\n";
                while (myIter->advance ()) {
                        myIter->getCurrent (temp);
			cout << temp << "\n";
                }
	}

	MyDB_BPlusTreeReaderWriterPtr supplierTableR = make_shared <MyDB_BPlusTreeReaderWriter> ("r_address", myTableRight, myMgr);
	MyDB_TablePtr myTableRightNoBPlus = make_shared <MyDB_Table> ("supplierRightNoBPlus", "supplierRightNoBPlus.bin", mySchemaR);

	cout << "loading right into B+-Tree indexed on r_address.\n";
	supplierTableR->loadFromTextFile ("supplierBig.tbl");

	{

		// get the output schema and table
		MyDB_SchemaPtr mySchemaOut = make_shared <MyDB_Schema> ();
		mySchemaOut->appendAtt (make_pair ("r_name", make_shared <MyDB_StringAttType> ()));
		mySchemaOut->appendAtt (make_pair ("r_address", make_shared <MyDB_StringAttType> ()));
		mySchemaOut->appendAtt (make_pair ("comment", make_shared <MyDB_StringAttType> ()));

		MyDB_TablePtr myTableOut = make_shared <MyDB_Table> ("supplierOut", "supplierOut.bin", mySchemaOut);
		MyDB_TableReaderWriterPtr supplierTableOut = make_shared <MyDB_TableReaderWriter> (myTableOut, myMgr);

		// This basically runs:
		//
		// SELECT r_name, r_address, "I love comments! " + r_comment
		// FROM supplierRight
		// WHERE r_address > "aa" AND r_address < "ab"

		vector <string> projections;
		projections.push_back ("[r_name]");
		projections.push_back ("[r_address]");
		projections.push_back ("+ (string[I love comments! ], [r_comment])");

		MyDB_StringAttValPtr low = make_shared <MyDB_StringAttVal> ();
		MyDB_StringAttValPtr high = make_shared <MyDB_StringAttVal> ();
		low->set ("aa");
		high->set ("ab");
		BPlusSelection myOp (supplierTableR, supplierTableOut, low, high, 
			"&& (&& ( > ([r_address], string[aa]), < ([r_address], string[ab])), > ([r_name], string[Supplier#000009000]))", projections);
			
		cout << "running selection\n";
		myOp.run ();

                MyDB_RecordPtr temp = supplierTableOut->getEmptyRecord ();
                MyDB_RecordIteratorAltPtr myIter = supplierTableOut->getIteratorAlt ();

		cout << "This should return 32 copies of 'Supplier#000009436|aaY,0sdTlrtKjse|I love comments! unusual, regular...'" << "\n";
                while (myIter->advance ()) {
                        myIter->getCurrent (temp);
			cout << temp << "\n";
		}	

	}

	{
		vector <pair <MyDB_AggType, string>> aggsToCompute;
		aggsToCompute.push_back (make_pair (MyDB_AggType :: avg, "* ([r_suppkey], double[1.0])"));
		aggsToCompute.push_back (make_pair (MyDB_AggType :: avg, "[r_acctbal]"));
		aggsToCompute.push_back (make_pair (MyDB_AggType :: cnt, "int[0]"));

		vector <string> groupings;
		groupings.push_back ("[r_suppkey]");
		groupings.push_back ("[r_name]");

		MyDB_SchemaPtr mySchemaOutAgain  = make_shared <MyDB_Schema> ();
		mySchemaOutAgain->appendAtt (make_pair ("r_suppkey", make_shared <MyDB_IntAttType> ()));
		mySchemaOutAgain->appendAtt (make_pair ("r_name", make_shared <MyDB_StringAttType> ()));
		mySchemaOutAgain->appendAtt (make_pair ("r_suppkey_avg", make_shared <MyDB_DoubleAttType> ()));
		mySchemaOutAgain->appendAtt (make_pair ("r_acctbal_avg", make_shared <MyDB_DoubleAttType> ()));
		mySchemaOutAgain->appendAtt (make_pair ("r_cnt", make_shared <MyDB_IntAttType> ()));
		MyDB_TablePtr aggTable = make_shared <MyDB_Table> ("aggOut", "aggOut.bin", mySchemaOutAgain);
		MyDB_TableReaderWriterPtr aggTableOut = make_shared <MyDB_TableReaderWriter> (aggTable, myMgr);

		Aggregate myOpAgain (supplierTableR, aggTableOut, aggsToCompute, groupings, "< ([r_suppkey], int[10])");

		// This basically runs:
		//
		// SELECT r_suppkey, r_name, AVG (r_suppkey * 1.0), AVG (r_acctbal), COUNT (0)
		// FROM supplierLeft
		// WHERE r_suppkey < 10
		// GROUP BY r_suppkey, r_name
		//
		cout << "running agg\n";
		myOpAgain.run ();

                MyDB_RecordPtr temp = aggTableOut->getEmptyRecord ();
                MyDB_RecordIteratorAltPtr myIter = aggTableOut->getIteratorAlt ();

		cout << "\nThere should be nine groups, each with 32 records.\n";
		cout << "One of them should be '5|Supplier#000000005|5.000000|-283.840000|32|'.\n";
                while (myIter->advance ()) {
                        myIter->getCurrent (temp);
			cout << temp << "\n";
		}
	}

	{
		vector <pair <MyDB_AggType, string>> aggsToCompute;
		aggsToCompute.push_back (make_pair (MyDB_AggType :: avg, "* ([r_suppkey], double[1.0])"));
		aggsToCompute.push_back (make_pair (MyDB_AggType :: avg, "[r_acctbal]"));
		aggsToCompute.push_back (make_pair (MyDB_AggType :: cnt, "int[0]"));

		vector <string> groupings;
		groupings.push_back ("/ ([r_suppkey], int[100])");

		MyDB_SchemaPtr mySchemaOutAgain  = make_shared <MyDB_Schema> ();
		mySchemaOutAgain->appendAtt (make_pair ("r_suppkey", make_shared <MyDB_IntAttType> ()));
		mySchemaOutAgain->appendAtt (make_pair ("r_suppkey_avg", make_shared <MyDB_DoubleAttType> ()));
		mySchemaOutAgain->appendAtt (make_pair ("r_acctbal_avg", make_shared <MyDB_DoubleAttType> ()));
		mySchemaOutAgain->appendAtt (make_pair ("r_cnt", make_shared <MyDB_IntAttType> ()));
		MyDB_TablePtr aggTable = make_shared <MyDB_Table> ("aggOut", "aggOut.bin", mySchemaOutAgain);
		MyDB_TableReaderWriterPtr aggTableOut = make_shared <MyDB_TableReaderWriter> (aggTable, myMgr);

		Aggregate myOpAgain (supplierTableR, aggTableOut, aggsToCompute, groupings, "bool [true]");

		// This basically runs:
		//
		// SELECT r_suppkey / 100, AVG (r_suppkey * 1.0), AVG (r_acctbal), COUNT (0)
		// FROM supplierLeft
		// GROUP BY r_suppkey / 100
		//
		cout << "running agg\n";
		myOpAgain.run ();
                MyDB_RecordPtr temp = aggTableOut->getEmptyRecord ();
                MyDB_RecordIteratorAltPtr myIter = aggTableOut->getIteratorAlt ();

		cout << "\nThere should be 101 groups, each with 3200 records, except for the first and last,\n";
		cout << "These should be '0|50.000000|4017.558586|3168|' and\n";
		cout << "'100|10000.000000|8968.420000|32|' respectively.\n";
                while (myIter->advance ()) {
                        myIter->getCurrent (temp);
			cout << temp << "\n";
		}

		aggsToCompute.clear ();
		aggsToCompute.push_back (make_pair (MyDB_AggType :: sum, "[r_cnt]"));

		groupings.clear ();
		
		MyDB_SchemaPtr mySchemaOutAgainAgain = make_shared <MyDB_Schema> ();
		mySchemaOutAgainAgain->appendAtt (make_pair ("final_cnt", make_shared <MyDB_IntAttType> ()));
		MyDB_TablePtr aggTableFinal = make_shared <MyDB_Table> ("aggOutOut", "aggOutOut.bin", mySchemaOutAgainAgain);
		MyDB_TableReaderWriterPtr aggTableOutFinal = make_shared <MyDB_TableReaderWriter> (aggTableFinal, myMgr);

		Aggregate myOpOnceAgain (aggTableOut, aggTableOutFinal, aggsToCompute, groupings, "bool [true]");
		//
		// Assuming that the output of the last query has the schema (r_suppkey, r_suppkey_avg, r_acctbal_avg, r_cnt)
		//
		// This basically runs:
		//
		// SELECT SUM (r_cnt)
		// FROM lastResult
		//
		myOpOnceAgain.run ();

		cout << "\nThere should be one result: 320000.\n";
                temp = aggTableOutFinal->getEmptyRecord ();
                myIter = aggTableOutFinal->getIteratorAlt ();

                while (myIter->advance ()) {
                        myIter->getCurrent (temp);
			cout << temp << "\n";
		}
	}

	{
		// get the output schema and table
		MyDB_SchemaPtr mySchemaOut = make_shared <MyDB_Schema> ();
		mySchemaOut->appendAtt (make_pair ("name", make_shared <MyDB_StringAttType> ()));
		mySchemaOut->appendAtt (make_pair ("acctbal", make_shared <MyDB_DoubleAttType> ()));
		mySchemaOut->appendAtt (make_pair ("nation", make_shared <MyDB_IntAttType> ()));
		MyDB_TablePtr myTableOut = make_shared <MyDB_Table> ("supplierOut", "supplierOut.bin", mySchemaOut);
		MyDB_TableReaderWriterPtr supplierTableOut = make_shared <MyDB_TableReaderWriter> (myTableOut, myMgr);

		// This basically runs:
		//
		// SELECT supplierLeft.l_name + " " supplierRight.r_name, supplierRight.r_acctbal * supplierLeft.l_acctbal
		// FROM supplierLeft, supplierRight
		// WHERE (supplierLeft.l_nationkey = supplierRight.r_nationkey) AND
		//       (supplierLeft.l_nationkey > 6)
		// 
		// It does this by hashing the smaller table (supplierLeft) on 
		// supplierLeft.l_nationkey.  It then scans
		// supplierRight, probing the hash table for matches

		vector <pair <string, string>> hashAtts;
		hashAtts.push_back (make_pair (string ("[l_nationkey]"), string ("[r_nationkey]")));

		vector <string> projections;
		projections.push_back ("+(+ ([l_name], string[ ]), [r_name])");
		projections.push_back ("* ([l_acctbal], [r_acctbal])");
		projections.push_back ("[l_nationkey]");

		cout << "Do you want to run a:\n";
		cout << "\t1. Sort merge join.\n";
		cout << "\t2. Scan join.\n";
		cout << "Enter 1 or 2:\n";
		int res;
		cin >> res;

		if (res == 2) {
			ScanJoin myOp (supplierTableL, supplierTableRNoBPlus, supplierTableOut, 
				"== ([l_nationkey], [r_nationkey]))", projections, hashAtts,
				"&& (< ([l_acctbal], int[4500]), > ([l_acctbal], int[4450]))",
				"&& (< ([r_acctbal], int[2500]), > ([r_acctbal], int[2450]))");
			cout << "running join\n";
			myOp.run ();
		} else if (res == 1) {
			SortMergeJoin myOp (supplierTableL, supplierTableRNoBPlus, supplierTableOut, 
				"== ([l_nationkey], [r_nationkey]))", projections, 
				make_pair (string ("[l_nationkey]"), string ("[r_nationkey]")),
				"&& (< ([l_acctbal], int[4500]), > ([l_acctbal], int[4450]))",
				"&& (< ([r_acctbal], int[2500]), > ([r_acctbal], int[2450]))");
			cout << "running join\n";
			myOp.run ();
		} else {
			cout << "I said 1 or 2!!!\n";
			return 3;
		}

		cout << "Done with the join, now I am counting the result size.\n";

		// now, we count the total number of records with each nation name
		vector <pair <MyDB_AggType, string>> aggsToCompute;
		aggsToCompute.push_back (make_pair (MyDB_AggType :: cnt, "int[0]"));

		vector <string> groupings;
		groupings.push_back ("[nation]");
		MyDB_SchemaPtr mySchemaOutAgain  = make_shared <MyDB_Schema> ();
		mySchemaOutAgain->appendAtt (make_pair ("nation", make_shared <MyDB_IntAttType> ()));
		mySchemaOutAgain->appendAtt (make_pair ("mycnt", make_shared <MyDB_IntAttType> ()));
		MyDB_TablePtr aggTable = make_shared <MyDB_Table> ("aggOut", "aggOut.bin", mySchemaOutAgain);
		MyDB_TableReaderWriterPtr aggTableOut = make_shared <MyDB_TableReaderWriter> (aggTable, myMgr);

		Aggregate myOpAgain (supplierTableOut, aggTableOut, aggsToCompute, groupings, "bool[true]");
		cout << "running aggregate\n";
		myOpAgain.run ();
		
                MyDB_RecordPtr temp = aggTableOut->getEmptyRecord ();
                MyDB_RecordIteratorAltPtr myIter = aggTableOut->getIteratorAlt ();

		cout << "\nThe output should be\n\t0|64|\n\t1|96|\n\t2|64|\n\t3|96|\n\t4|192|\n\t5|256|\n\t6|192|\n\t7|96|\n\t8|64|\n\t9|128|\n\t11|288|\n\t14|96|\n\t15|128|\n\t16|128|\n\t17|32|\n\t19|64|\n\t20|384|\n\t24|64|\n\n";
                while (myIter->advance ()) {
                        myIter->getCurrent (temp);
			cout << temp << "\n";
                }
	}

	{
		// get the output schema and table
		MyDB_SchemaPtr mySchemaOut = make_shared <MyDB_Schema> ();
		mySchemaOut->appendAtt (make_pair ("name", make_shared <MyDB_StringAttType> ()));
		mySchemaOut->appendAtt (make_pair ("acctbal", make_shared <MyDB_DoubleAttType> ()));
		MyDB_TablePtr myTableOut = make_shared <MyDB_Table> ("supplierOut", "supplierOut.bin", mySchemaOut);
		MyDB_TableReaderWriterPtr supplierTableOut = make_shared <MyDB_TableReaderWriter> (myTableOut, myMgr);

		// This basically runs:
		//
		// SELECT supplierLeft.l_name + " " supplierRight.r_name, supplierRight.r_acctbal * supplierLeft.l_acctbal
		// FROM supplierLeft, supplierRight
		// WHERE (supplierLeft.l_nationkey = supplierRight.r_nationkey) AND
		//       (supplierLeft.l_nationkey > 6)
		// 
		// It does this by hashing the smaller table (supplierLeft) on 
		// supplierLeft.l_nationkey.  It then scans
		// supplierRight, probing the hash table for matches

		vector <pair <string, string>> hashAtts;
		hashAtts.push_back (make_pair (string ("[l_nationkey]"), string ("[r_nationkey]")));

		vector <string> projections;
		projections.push_back ("+(+ ([l_name], string[ ]), [r_name])");
		projections.push_back ("* ([l_acctbal], [r_acctbal])");

		cout << "Do you want to run a:\n";
		cout << "\t1. Sort merge join.\n";
		cout << "\t2. Scan join.\n";
		cout << "Enter 1 or 2:\n";
		int res;
		cin >> res;

		if (res == 2) {
			ScanJoin myOp (supplierTableL, supplierTableRNoBPlus, supplierTableOut, 
				"== ([l_nationkey], [r_nationkey]))", projections, hashAtts,
				"< ([l_nationkey], int[2])",
				"bool[true]");
			cout << "running join (may take some time)\n";
			myOp.run ();
		} else if (res == 1) {
			SortMergeJoin myOp (supplierTableL, supplierTableRNoBPlus, supplierTableOut, 
				"== ([l_nationkey], [r_nationkey]))", projections, 
				make_pair (string ("[l_nationkey]"), string ("[r_nationkey]")),
				"< ([l_nationkey], int[2])",
				"bool[true]");
			cout << "running join (may take some time)\n";
			myOp.run ();
		} else {
			cout << "I said 1 or 2!!!\n";
			return 3;
		}

		cout << "Done with the join, now I am counting the result size.\n";

		// now, we count the total number of records with each nation name
		vector <pair <MyDB_AggType, string>> aggsToCompute;
		aggsToCompute.push_back (make_pair (MyDB_AggType :: cnt, "int[0]"));

		vector <string> groupings;
		MyDB_SchemaPtr mySchemaOutAgain  = make_shared <MyDB_Schema> ();
		mySchemaOutAgain->appendAtt (make_pair ("mycnt", make_shared <MyDB_IntAttType> ()));
		MyDB_TablePtr aggTable = make_shared <MyDB_Table> ("aggOut", "aggOut.bin", mySchemaOutAgain);
		MyDB_TableReaderWriterPtr aggTableOut = make_shared <MyDB_TableReaderWriter> (aggTable, myMgr);

		Aggregate myOpAgain (supplierTableOut, aggTableOut, aggsToCompute, groupings, "bool[true]");
		cout << "running aggregate (may take some time)\n";
		myOpAgain.run ();
		
                MyDB_RecordPtr temp = aggTableOut->getEmptyRecord ();
                MyDB_RecordIteratorAltPtr myIter = aggTableOut->getIteratorAlt ();

		cout << "\nThe output should be 11103008:\n";
                while (myIter->advance ()) {
                        myIter->getCurrent (temp);
			cout << temp << "\n";
                }
	}

}

#endif

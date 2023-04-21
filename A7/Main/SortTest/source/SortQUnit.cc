
#ifndef SORT_TEST_H
#define SORT_TEST_H

#include "MyDB_AttType.h"  
#include "MyDB_BufferManager.h"
#include "MyDB_Catalog.h"  
#include "MyDB_Page.h"
#include "MyDB_PageReaderWriter.h"
#include "MyDB_Record.h"
#include "MyDB_Table.h"
#include "MyDB_TableReaderWriter.h"
#include "MyDB_Schema.h"
#include "QUnit.h"
#include "Sorting.h"
#include <iostream>


int main (int argc, char *argv[]) {

	//Get 
	int start = 1;
	if (argc > 1 && argv[1][0] >= '0' && argv[1][0] <= '9') {
		start = argv[1][0] - '0';
	}
	
	QUnit::UnitTest qunit(cerr, QUnit::normal);
	int countCorrect;

	
	
	switch (start) {
	case 1:	
	cout << endl << "Test 1: Index - Value pair: Sort by Index:" << endl << flush;
	countCorrect = 0;
	
	//Index - Value pairs:
	cout << "Initialization.." << flush;
	{
		// create a catalog
		MyDB_CatalogPtr myCatalog = make_shared <MyDB_Catalog> ("catFile");

		// now make a schema
		MyDB_SchemaPtr mySchema = make_shared <MyDB_Schema> ();
		mySchema->appendAtt (make_pair ("index", make_shared <MyDB_IntAttType> ()));
		mySchema->appendAtt (make_pair ("value", make_shared <MyDB_DoubleAttType> ()));


		// use the schema to create a table
		MyDB_TablePtr myTable = make_shared <MyDB_Table> ("indexvalue", "indexvalue.bin", mySchema);
		MyDB_BufferManagerPtr myMgr = make_shared <MyDB_BufferManager> (131072, 128, "tempFile");
		MyDB_TableReaderWriter ivTable (myTable, myMgr);
		
		// load it from a text file
		ivTable.loadFromTextFile ("indexvalue.tbl");
		
		// put the IV table into the catalog
		myTable->putInCatalog (myCatalog);
	}	
	
	cout << "Sort a table.."  << flush;
	{
		// load up the table indexvalue table from the catalog
		MyDB_CatalogPtr myCatalog = make_shared <MyDB_Catalog> ("catFile");
		map <string, MyDB_TablePtr> allTables = MyDB_Table :: getAllTables (myCatalog);
		MyDB_BufferManagerPtr myMgr = make_shared <MyDB_BufferManager> (131072, 128, "tempFile");
		MyDB_TableReaderWriter indexvalueTable (allTables["indexvalue"], myMgr);

		// use the schema to create a table
		MyDB_TablePtr outTable = make_shared <MyDB_Table> ("indexvalueSorted", "indexvalueSorted.bin", allTables["indexvalue"]->getSchema ());
		MyDB_TableReaderWriter outputTable (outTable, myMgr);

		// get two empty records
		MyDB_RecordPtr rec1 = indexvalueTable.getEmptyRecord ();
		MyDB_RecordPtr rec2 = indexvalueTable.getEmptyRecord ();

		// and get a comparator
		function <bool ()> myComp = buildRecordComparator (rec1, rec2, "[index]");

		// and sort
		sort (64, indexvalueTable, outputTable, myComp, rec1, rec2);

		MyDB_RecordIteratorAltPtr myIter = outputTable.getIteratorAlt ();

		// there should be 13 records
		int counter = 0;
		while (myIter->advance ()) {
			myIter->getCurrent (rec1);
			counter++;
		}

		//Check?
		if (counter == 13) {
			countCorrect++;
		}
		
		// put the indexvalue table into the catalog
		outTable->putInCatalog (myCatalog);
	}


	cout << "Compare Sorted Table.." << endl << flush;
	{
		// load up the two tables from the catalog
		MyDB_CatalogPtr myCatalog = make_shared <MyDB_Catalog> ("catFile");
		map <string, MyDB_TablePtr> allTables = MyDB_Table :: getAllTables (myCatalog);
		MyDB_BufferManagerPtr myMgr = make_shared <MyDB_BufferManager> (131072, 128, "tempFile");
		MyDB_TableReaderWriter sortedTable (allTables["indexvalueSorted"], myMgr);
		MyDB_TableReaderWriter otherSortedTable (allTables["indexvalue"], myMgr);

		// load up the sorted text file
		otherSortedTable.loadFromTextFile ("indexvalueBigSorted.tbl");

		// get two empty records
		MyDB_RecordPtr rec1 = sortedTable.getEmptyRecord ();
		MyDB_RecordPtr rec2 = otherSortedTable.getEmptyRecord ();

		// and get a comparator
		function <bool ()> myComp = buildRecordComparator (rec1, rec2, "[index]");

		// get two iterators
		MyDB_RecordIteratorAltPtr myIterOne = sortedTable.getIteratorAlt ();
		MyDB_RecordIteratorAltPtr myIterTwo = otherSortedTable.getIteratorAlt ();

		// make sure the results are the same
		int matches = 0;
        while (myIterOne->advance ()) {
			myIterTwo->advance ();

			// get the two records
			myIterOne->getCurrent (rec1);
			myIterTwo->getCurrent (rec2);

			if (!myComp ()) {
				myIterOne->getCurrent (rec2);
				myIterTwo->getCurrent (rec1);
				if (!myComp ())
					matches++;
			}
        }

		//Check?
		if (matches == 13) {
			countCorrect++;
		}
	}	

	QUNIT_IS_EQUAL (countCorrect, 2);
	if (countCorrect == 2) {
		cout << "PASS" << endl << flush;
	}
	else {
		cout << endl << endl << "***FAIL****" << endl << endl << flush;
	}	
	
	
	case 2:
	cout << endl << "Test 2: Index - Value pair: Sort by Value:" << endl << flush;
	countCorrect = 0;
	cout << "Sort a table.."  << flush;
	{
		// load up the table indexvalue table from the catalog
		MyDB_CatalogPtr myCatalog = make_shared <MyDB_Catalog> ("catFile");
		map <string, MyDB_TablePtr> allTables = MyDB_Table :: getAllTables (myCatalog);
		MyDB_BufferManagerPtr myMgr = make_shared <MyDB_BufferManager> (131072, 128, "tempFile");
		MyDB_TableReaderWriter indexvalueTable (allTables["indexvalue"], myMgr);

		// use the schema to create a table
		MyDB_TablePtr outTable = make_shared <MyDB_Table> ("indexvalueSortedTwo", "indexvalueSortedTwo.bin", allTables["indexvalue"]->getSchema ());
		MyDB_TableReaderWriter outputTable (outTable, myMgr);

		// get two empty records
		MyDB_RecordPtr rec1 = indexvalueTable.getEmptyRecord ();
		MyDB_RecordPtr rec2 = indexvalueTable.getEmptyRecord ();

		// and get a comparator
		function <bool ()> myComp = buildRecordComparator (rec1, rec2, "[value]");

		// and sort
		sort (64, indexvalueTable, outputTable, myComp, rec1, rec2);

		MyDB_RecordIteratorAltPtr myIter = outputTable.getIteratorAlt ();

		// there should be 13 records
		int counter = 0;
		while (myIter->advance ()) {
			myIter->getCurrent (rec1);
			counter++;
		}

		//Check?
		if (counter == 13) {
			countCorrect++;
		}
		
		// put the indexvalue table into the catalog
		outTable->putInCatalog (myCatalog);
	}


	cout << "Compare Sorted Table.." << endl << flush;
	{
		// load up the two tables from the catalog
		MyDB_CatalogPtr myCatalog = make_shared <MyDB_Catalog> ("catFile");
		map <string, MyDB_TablePtr> allTables = MyDB_Table :: getAllTables (myCatalog);
		MyDB_BufferManagerPtr myMgr = make_shared <MyDB_BufferManager> (131072, 128, "tempFile");
		MyDB_TableReaderWriter sortedTable (allTables["indexvalueSortedTwo"], myMgr);
		MyDB_TableReaderWriter otherSortedTable (allTables["indexvalue"], myMgr);

		// load up the sorted text file
		otherSortedTable.loadFromTextFile ("indexvalueBigSortedTwo.tbl");

		// get two empty records
		MyDB_RecordPtr rec1 = sortedTable.getEmptyRecord ();
		MyDB_RecordPtr rec2 = otherSortedTable.getEmptyRecord ();

		// and get a comparator
		function <bool ()> myComp = buildRecordComparator (rec1, rec2, "[value]");

		// get two iterators
		MyDB_RecordIteratorAltPtr myIterOne = sortedTable.getIteratorAlt ();
		MyDB_RecordIteratorAltPtr myIterTwo = otherSortedTable.getIteratorAlt ();

		// make sure the results are the same
		int matches = 0;
        while (myIterOne->advance ()) {
			myIterTwo->advance ();

			// get the two records
			myIterOne->getCurrent (rec1);
			myIterTwo->getCurrent (rec2);

			if (!myComp ()) {
				myIterOne->getCurrent (rec2);
				myIterTwo->getCurrent (rec1);
				if (!myComp ())
					matches++;
			}
        }

		//Check?
		if (matches == 13) {
			countCorrect++;
		}
	}	

	QUNIT_IS_EQUAL (countCorrect, 2);
	if (countCorrect == 2) {
		cout << "PASS" << endl << flush;
	}
	else {
		cout << endl << endl << "***FAIL****" << endl << endl << flush;
	}		
	
	
	case 3:
	cout << endl << "Test 3: Index - Value pair: Sort an already sorted table:" << endl << flush;
	countCorrect = 0;
	cout << "Sort a table.."  << flush;
	{
		// load up the table indexvalue table from the catalog
		MyDB_CatalogPtr myCatalog = make_shared <MyDB_Catalog> ("catFile");
		map <string, MyDB_TablePtr> allTables = MyDB_Table :: getAllTables (myCatalog);
		MyDB_BufferManagerPtr myMgr = make_shared <MyDB_BufferManager> (131072, 128, "tempFile");
		MyDB_TableReaderWriter indexvalueTable (allTables["indexvalueSortedTwo"], myMgr);   //already sorted by value.

		// use the schema to create a table
		MyDB_TablePtr outTable = make_shared <MyDB_Table> ("indexvalueSortedThree", "indexvalueSortedThree.bin", allTables["indexvalue"]->getSchema ());
		MyDB_TableReaderWriter outputTable (outTable, myMgr);

		// get two empty records
		MyDB_RecordPtr rec1 = indexvalueTable.getEmptyRecord ();
		MyDB_RecordPtr rec2 = indexvalueTable.getEmptyRecord ();

		// and get a comparator
		function <bool ()> myComp = buildRecordComparator (rec1, rec2, "[value]");

		// and sort
		sort (64, indexvalueTable, outputTable, myComp, rec1, rec2);

		MyDB_RecordIteratorAltPtr myIter = outputTable.getIteratorAlt ();

		// there should be 13 records
		int counter = 0;
		while (myIter->advance ()) {
			myIter->getCurrent (rec1);
			counter++;
		}

		//Check?
		if (counter == 13) {
			countCorrect++;
		}
		
		// put the indexvalue table into the catalog
		outTable->putInCatalog (myCatalog);
	}

	cout << "Compare Sorted Table.." << endl << flush;
	{
		// load up the two tables from the catalog
		MyDB_CatalogPtr myCatalog = make_shared <MyDB_Catalog> ("catFile");
		map <string, MyDB_TablePtr> allTables = MyDB_Table :: getAllTables (myCatalog);
		MyDB_BufferManagerPtr myMgr = make_shared <MyDB_BufferManager> (131072, 128, "tempFile");
		MyDB_TableReaderWriter sortedTable (allTables["indexvalueSortedThree"], myMgr);
		MyDB_TableReaderWriter otherSortedTable (allTables["indexvalue"], myMgr);

		// load up the sorted text file
		otherSortedTable.loadFromTextFile ("indexvalueBigSortedTwo.tbl");		//pre-sorted by value table

		// get two empty records
		MyDB_RecordPtr rec1 = sortedTable.getEmptyRecord ();
		MyDB_RecordPtr rec2 = otherSortedTable.getEmptyRecord ();

		// and get a comparator
		function <bool ()> myComp = buildRecordComparator (rec1, rec2, "[value]");

		// get two iterators
		MyDB_RecordIteratorAltPtr myIterOne = sortedTable.getIteratorAlt ();
		MyDB_RecordIteratorAltPtr myIterTwo = otherSortedTable.getIteratorAlt ();

		// make sure the results are the same
		int matches = 0;
		while (myIterOne->advance ()) {
			myIterTwo->advance ();

			// get the two records
			myIterOne->getCurrent (rec1);
			myIterTwo->getCurrent (rec2);

			if (!myComp ()) {
				myIterOne->getCurrent (rec2);
				myIterTwo->getCurrent (rec1);
				if (!myComp ())
					matches++;
			}
		}

		//Check?
		if (matches == 13) {
			countCorrect++;
		}
	}	

	QUNIT_IS_EQUAL (countCorrect, 2);
	if (countCorrect == 2) {
		cout << "PASS" << endl << flush;
	}
	else {
		cout << endl << endl << "***FAIL****" << endl << endl << flush;
	}		


	
	case 4:
	cout << endl << "Test 4: Index - Value pair: Sort previous table with reduced run-length:" << endl << flush;
	countCorrect = 0;
	cout << "Sort a table.."  << flush;
	{
		// load up the table indexvalue table from the catalog
		MyDB_CatalogPtr myCatalog = make_shared <MyDB_Catalog> ("catFile");
		map <string, MyDB_TablePtr> allTables = MyDB_Table :: getAllTables (myCatalog);
		MyDB_BufferManagerPtr myMgr = make_shared <MyDB_BufferManager> (131072, 128, "tempFile");
		MyDB_TableReaderWriter indexvalueTable (allTables["indexvalueSortedTwo"], myMgr);   //already sorted by value.

		// use the schema to create a table
		MyDB_TablePtr outTable = make_shared <MyDB_Table> ("indexvalueSortedFour", "indexvalueSortedFour.bin", allTables["indexvalue"]->getSchema ());
		MyDB_TableReaderWriter outputTable (outTable, myMgr);

		// get two empty records
		MyDB_RecordPtr rec1 = indexvalueTable.getEmptyRecord ();
		MyDB_RecordPtr rec2 = indexvalueTable.getEmptyRecord ();

		// and get a comparator
		function <bool ()> myComp = buildRecordComparator (rec1, rec2, "[index]");

		// and sort
		sort (32, indexvalueTable, outputTable, myComp, rec1, rec2);

		MyDB_RecordIteratorAltPtr myIter = outputTable.getIteratorAlt ();

		// there should be 13 records
		int counter = 0;
		while (myIter->advance ()) {
			myIter->getCurrent (rec1);
			counter++;
		}

		//Check?
		if (counter == 13) {
			countCorrect++;
		}
		
		// put the indexvalue table into the catalog
		outTable->putInCatalog (myCatalog);
	}

	cout << "Compare Sorted Table.." << endl << flush;
	{
		// load up the two tables from the catalog
		MyDB_CatalogPtr myCatalog = make_shared <MyDB_Catalog> ("catFile");
		map <string, MyDB_TablePtr> allTables = MyDB_Table :: getAllTables (myCatalog);
		MyDB_BufferManagerPtr myMgr = make_shared <MyDB_BufferManager> (131072, 128, "tempFile");
		MyDB_TableReaderWriter sortedTable (allTables["indexvalueSortedFour"], myMgr);
		MyDB_TableReaderWriter otherSortedTable (allTables["indexvalue"], myMgr);

		// load up the sorted text file
		otherSortedTable.loadFromTextFile ("indexvalueBigSorted.tbl");		//pre-sorted by index table

		// get two empty records
		MyDB_RecordPtr rec1 = sortedTable.getEmptyRecord ();
		MyDB_RecordPtr rec2 = otherSortedTable.getEmptyRecord ();

		// and get a comparator
		function <bool ()> myComp = buildRecordComparator (rec1, rec2, "[index]");

		// get two iterators
		MyDB_RecordIteratorAltPtr myIterOne = sortedTable.getIteratorAlt ();
		MyDB_RecordIteratorAltPtr myIterTwo = otherSortedTable.getIteratorAlt ();

		// make sure the results are the same
		int matches = 0;
		while (myIterOne->advance ()) {
			myIterTwo->advance ();

			// get the two records
			myIterOne->getCurrent (rec1);
			myIterTwo->getCurrent (rec2);

			if (!myComp ()) {
				myIterOne->getCurrent (rec2);
				myIterTwo->getCurrent (rec1);
				if (!myComp ())
					matches++;
			}
		}

		//Check?
		if (matches == 13) {
			countCorrect++;
		}
	}	

	QUNIT_IS_EQUAL (countCorrect, 2);
	if (countCorrect == 2) {
		cout << "PASS" << endl << flush;
	}
	else {
		cout << endl << endl << "***FAIL****" << endl << endl << flush;
	}	

	
	

	case 5:
	cout << endl << "Test 5: Empty table:" << endl << flush;
	countCorrect = 0;
	cout << "Initialization.." << flush;
	{
		// create a catalog
		MyDB_CatalogPtr myCatalog = make_shared <MyDB_Catalog> ("catFile");

		// now make a schema
		MyDB_SchemaPtr mySchema = make_shared <MyDB_Schema> ();
		mySchema->appendAtt (make_pair ("index", make_shared <MyDB_IntAttType> ()));
		mySchema->appendAtt (make_pair ("value", make_shared <MyDB_DoubleAttType> ()));

		// use the schema to create a table
		MyDB_TablePtr myTable = make_shared <MyDB_Table> ("empty", "empty.bin", mySchema);
		MyDB_BufferManagerPtr myMgr = make_shared <MyDB_BufferManager> (131072, 128, "tempFile");
		MyDB_TableReaderWriter ivTable (myTable, myMgr);
		
		// load it from a text file
		ivTable.loadFromTextFile ("empty.tbl");
		
		// put the IV table into the catalog
		myTable->putInCatalog (myCatalog);
	}	
	
	cout << "Sort a table.."  << endl << flush;
	{
		// load up the table indexvalue table from the catalog
		MyDB_CatalogPtr myCatalog = make_shared <MyDB_Catalog> ("catFile");
		map <string, MyDB_TablePtr> allTables = MyDB_Table :: getAllTables (myCatalog);
		MyDB_BufferManagerPtr myMgr = make_shared <MyDB_BufferManager> (131072, 128, "tempFile");
		MyDB_TableReaderWriter indexvalueTable (allTables["empty"], myMgr);   

		// use the schema to create a table
		MyDB_TablePtr outTable = make_shared <MyDB_Table> ("emptySorted", "emptySorted.bin", allTables["empty"]->getSchema ());
		MyDB_TableReaderWriter outputTable (outTable, myMgr);

		// get two empty records
		MyDB_RecordPtr rec1 = indexvalueTable.getEmptyRecord ();
		MyDB_RecordPtr rec2 = indexvalueTable.getEmptyRecord ();

		// and get a comparator
		function <bool ()> myComp = buildRecordComparator (rec1, rec2, "[index]");

		// and sort
		sort (64, indexvalueTable, outputTable, myComp, rec1, rec2);

		MyDB_RecordIteratorAltPtr myIter = outputTable.getIteratorAlt ();

		// there should be 0 records
		int counter = 0;
		while (myIter->advance ()) {
			myIter->getCurrent (rec1);
			counter++;
		}

		//Check?
		if (counter == 0) {
			countCorrect++;
		}
		
		// put the indexvalue table into the catalog
		outTable->putInCatalog (myCatalog);
	}	
	
	QUNIT_IS_EQUAL (countCorrect, 1);
	if (countCorrect == 1) {
		cout << "PASS" << endl << flush;
	}
	else {
		cout << endl << endl << "***FAIL****" << endl << endl << flush;
	}		

	
	
	case 6:
	//Given Test Case:
	cout << endl << "Test 6: Pre-released test cases with assignment:" << endl << flush;
	countCorrect = 0;
	cout << "Initialization.." << flush;
	{
		// create a catalog
		MyDB_CatalogPtr myCatalog = make_shared <MyDB_Catalog> ("catFile");

		// now make a schema
		MyDB_SchemaPtr mySchema = make_shared <MyDB_Schema> ();
		mySchema->appendAtt (make_pair ("suppkey", make_shared <MyDB_IntAttType> ()));
		mySchema->appendAtt (make_pair ("name", make_shared <MyDB_StringAttType> ()));
		mySchema->appendAtt (make_pair ("address", make_shared <MyDB_StringAttType> ()));
		mySchema->appendAtt (make_pair ("nationkey", make_shared <MyDB_IntAttType> ()));
		mySchema->appendAtt (make_pair ("phone", make_shared <MyDB_StringAttType> ()));
		mySchema->appendAtt (make_pair ("acctbal", make_shared <MyDB_DoubleAttType> ()));
		mySchema->appendAtt (make_pair ("comment", make_shared <MyDB_StringAttType> ()));

		// use the schema to create a table
		MyDB_TablePtr myTable = make_shared <MyDB_Table> ("supplier", "supplier.bin", mySchema);
		MyDB_BufferManagerPtr myMgr = make_shared <MyDB_BufferManager> (131072, 128, "tempFile");
		MyDB_TableReaderWriter supplierTable (myTable, myMgr);

		// load it from a text file
		supplierTable.loadFromTextFile ("supplierBig.tbl");

		// put the supplier table into the catalog
		myTable->putInCatalog (myCatalog);
	}

	cout << "Sort a table.."  << flush;
	{
		// load up the table supplier table from the catalog
		MyDB_CatalogPtr myCatalog = make_shared <MyDB_Catalog> ("catFile");
		map <string, MyDB_TablePtr> allTables = MyDB_Table :: getAllTables (myCatalog);
		MyDB_BufferManagerPtr myMgr = make_shared <MyDB_BufferManager> (131072, 128, "tempFile");
		MyDB_TableReaderWriter supplierTable (allTables["supplier"], myMgr);

		// use the schema to create a table
		MyDB_TablePtr outTable = make_shared <MyDB_Table> ("supplierSorted", "supplierSorted.bin", allTables["supplier"]->getSchema ());
		MyDB_TableReaderWriter outputTable (outTable, myMgr);

		// get two empty records
		MyDB_RecordPtr rec1 = supplierTable.getEmptyRecord ();
		MyDB_RecordPtr rec2 = supplierTable.getEmptyRecord ();

		// and get a comparator
		function <bool ()> myComp = buildRecordComparator (rec1, rec2, "[acctbal]");

		// and sort
		sort (64, supplierTable, outputTable, myComp, rec1, rec2);

		MyDB_RecordIteratorAltPtr myIter = outputTable.getIteratorAlt ();

		// there should be 320000 records
		int counter = 0;
		while (myIter->advance ()) {
			myIter->getCurrent (rec1);
			counter++;
		}

		//Check?
		if (counter == 320000) {
			countCorrect++;
		}
		
		// put the supplier table into the catalog
		outTable->putInCatalog (myCatalog);
	}	
	
	cout << "Compare Sorted Table.." << endl << flush;
	{
		// load up the two tables from the catalog
		MyDB_CatalogPtr myCatalog = make_shared <MyDB_Catalog> ("catFile");
		map <string, MyDB_TablePtr> allTables = MyDB_Table :: getAllTables (myCatalog);
		MyDB_BufferManagerPtr myMgr = make_shared <MyDB_BufferManager> (131072, 128, "tempFile");
		MyDB_TableReaderWriter sortedTable (allTables["supplierSorted"], myMgr);
		MyDB_TableReaderWriter otherSortedTable (allTables["supplier"], myMgr);

		// load up the sorted text file
		otherSortedTable.loadFromTextFile ("supplierBigSorted.tbl");

		// get two empty records
		MyDB_RecordPtr rec1 = sortedTable.getEmptyRecord ();
		MyDB_RecordPtr rec2 = otherSortedTable.getEmptyRecord ();

		// and get a comparator
		function <bool ()> myComp = buildRecordComparator (rec1, rec2, "[acctbal]");

		// get two iterators
		MyDB_RecordIteratorAltPtr myIterOne = sortedTable.getIteratorAlt ();
		MyDB_RecordIteratorAltPtr myIterTwo = otherSortedTable.getIteratorAlt ();

		// make sure the results are the same
		int matches = 0;
        while (myIterOne->advance ()) {
			myIterTwo->advance ();

			// get the two records
			myIterOne->getCurrent (rec1);
			myIterTwo->getCurrent (rec2);

			if (!myComp ()) {
				myIterOne->getCurrent (rec2);
				myIterTwo->getCurrent (rec1);
				if (!myComp ())
					matches++;
			}
        }

		//Check?
		if (matches == 320000) {
			countCorrect++;
		}
	}	
	
	QUNIT_IS_EQUAL (countCorrect, 2);
	if (countCorrect == 2) {
		cout << "PASS" << endl << flush;
	}
	else {
		cout << endl << endl << "***FAIL****" << endl << endl << flush;
	}
		
		

	case 7:
	cout << endl << "Test 7: Sort file by pages:" << endl << flush;
	countCorrect = 0;
	{
		// load up the table supplier table from the catalog
		MyDB_CatalogPtr myCatalog = make_shared <MyDB_Catalog> ("catFile");
		map <string, MyDB_TablePtr> allTables = MyDB_Table :: getAllTables (myCatalog);
		MyDB_BufferManagerPtr myMgr = make_shared <MyDB_BufferManager> (131072, 128, "tempFile");
		MyDB_TableReaderWriter sortedTable (allTables["supplierSorted"], myMgr);		//The file is already sorted.

		
		// Go to ith page and sort again:
		for (int i = 0; i < sortedTable.getNumPages (); i++) {
			MyDB_RecordPtr temp = sortedTable.getEmptyRecord ();
			MyDB_RecordPtr temp2 = sortedTable.getEmptyRecord ();
			
			function <bool ()> myComp = buildRecordComparator (temp, temp2, "[acctbal]");
			MyDB_PageReaderWriterPtr sorted = sortedTable[i].sort (myComp, temp, temp2);	
		}

		

		//Compare with pre-sorted table:
		MyDB_TableReaderWriter otherSortedTable (allTables["supplier"], myMgr);

		// load up the sorted text file
		otherSortedTable.loadFromTextFile ("supplierBigSorted.tbl");

		// get two empty records
		MyDB_RecordPtr rec1 = sortedTable.getEmptyRecord ();
		MyDB_RecordPtr rec2 = otherSortedTable.getEmptyRecord ();

		// and get a comparator
		function <bool ()> myComp = buildRecordComparator (rec1, rec2, "[acctbal]");

		// get two iterators
		MyDB_RecordIteratorAltPtr myIterOne = sortedTable.getIteratorAlt ();
		MyDB_RecordIteratorAltPtr myIterTwo = otherSortedTable.getIteratorAlt ();

		// make sure the results are the same
		int matches = 0;
        while (myIterOne->advance ()) {
			myIterTwo->advance ();

			// get the two records
			myIterOne->getCurrent (rec1);
			myIterTwo->getCurrent (rec2);

			if (!myComp ()) {
				myIterOne->getCurrent (rec2);
				myIterTwo->getCurrent (rec1);
				if (!myComp ())
					matches++;
			}
        }

		//Check?
		if (matches == 320000) {
			countCorrect++;
		}
	}		
		
	QUNIT_IS_EQUAL (countCorrect, 1);
	if (countCorrect == 1) {
		cout << "PASS" << endl << flush;
	}
	else {
		cout << endl << endl << "***FAIL****" << endl << endl << flush;
	}	
		

	case 8:	
	cout << endl << "Test 8: Load a sorted file and sort again:" << endl << flush;
	countCorrect = 0;
	cout << "Initialization.." << flush;
	{
		// create a catalog
		MyDB_CatalogPtr myCatalog = make_shared <MyDB_Catalog> ("catFile");

		// now make a schema
		MyDB_SchemaPtr mySchema = make_shared <MyDB_Schema> ();
		mySchema->appendAtt (make_pair ("suppkey", make_shared <MyDB_IntAttType> ()));
		mySchema->appendAtt (make_pair ("name", make_shared <MyDB_StringAttType> ()));
		mySchema->appendAtt (make_pair ("address", make_shared <MyDB_StringAttType> ()));
		mySchema->appendAtt (make_pair ("nationkey", make_shared <MyDB_IntAttType> ()));
		mySchema->appendAtt (make_pair ("phone", make_shared <MyDB_StringAttType> ()));
		mySchema->appendAtt (make_pair ("acctbal", make_shared <MyDB_DoubleAttType> ()));
		mySchema->appendAtt (make_pair ("comment", make_shared <MyDB_StringAttType> ()));

		// use the schema to create a table
		MyDB_TablePtr myTable = make_shared <MyDB_Table> ("newsupplier", "newsupplier.bin", mySchema);
		MyDB_BufferManagerPtr myMgr = make_shared <MyDB_BufferManager> (131072, 128, "tempFile");
		MyDB_TableReaderWriter supplierTable (myTable, myMgr);

		// load it from a text file
		supplierTable.loadFromTextFile ("supplierBigSorted.tbl");

		// put the supplier table into the catalog
		myTable->putInCatalog (myCatalog);
	}

	cout << "Sort a table.."  << flush;
	{
		// load up the table supplier table from the catalog
		MyDB_CatalogPtr myCatalog = make_shared <MyDB_Catalog> ("catFile");
		map <string, MyDB_TablePtr> allTables = MyDB_Table :: getAllTables (myCatalog);
		MyDB_BufferManagerPtr myMgr = make_shared <MyDB_BufferManager> (131072, 128, "tempFile");
		MyDB_TableReaderWriter supplierTable (allTables["newsupplier"], myMgr);

		// use the schema to create a table
		MyDB_TablePtr outTable = make_shared <MyDB_Table> ("newsupplierSorted", "newsupplierSorted.bin", allTables["supplier"]->getSchema ());
		MyDB_TableReaderWriter outputTable (outTable, myMgr);

		// get two empty records
		MyDB_RecordPtr rec1 = supplierTable.getEmptyRecord ();
		MyDB_RecordPtr rec2 = supplierTable.getEmptyRecord ();

		// and get a comparator
		function <bool ()> myComp = buildRecordComparator (rec1, rec2, "[acctbal]");

		// and sort
		sort (64, supplierTable, outputTable, myComp, rec1, rec2);

		MyDB_RecordIteratorAltPtr myIter = outputTable.getIteratorAlt ();

		// there should be 320000 records
		int counter = 0;
		while (myIter->advance ()) {
			myIter->getCurrent (rec1);
			counter++;
		}

		//Check?
		if (counter == 320000) {
			countCorrect++;
		}
		
		// put the supplier table into the catalog
		outTable->putInCatalog (myCatalog);
	}	
	
	cout << "Compare Sorted Table.." << endl << flush;
	{
		// load up the two tables from the catalog
		MyDB_CatalogPtr myCatalog = make_shared <MyDB_Catalog> ("catFile");
		map <string, MyDB_TablePtr> allTables = MyDB_Table :: getAllTables (myCatalog);
		MyDB_BufferManagerPtr myMgr = make_shared <MyDB_BufferManager> (131072, 128, "tempFile");
		MyDB_TableReaderWriter sortedTable (allTables["newsupplierSorted"], myMgr);
		MyDB_TableReaderWriter otherSortedTable (allTables["supplier"], myMgr);

		// load up the sorted text file
		otherSortedTable.loadFromTextFile ("supplierBigSorted.tbl");

		// get two empty records
		MyDB_RecordPtr rec1 = sortedTable.getEmptyRecord ();
		MyDB_RecordPtr rec2 = otherSortedTable.getEmptyRecord ();

		// and get a comparator
		function <bool ()> myComp = buildRecordComparator (rec1, rec2, "[acctbal]");

		// get two iterators
		MyDB_RecordIteratorAltPtr myIterOne = sortedTable.getIteratorAlt ();
		MyDB_RecordIteratorAltPtr myIterTwo = otherSortedTable.getIteratorAlt ();

		// make sure the results are the same
		int matches = 0;
        while (myIterOne->advance ()) {
			myIterTwo->advance ();

			// get the two records
			myIterOne->getCurrent (rec1);
			myIterTwo->getCurrent (rec2);

			if (!myComp ()) {
				myIterOne->getCurrent (rec2);
				myIterTwo->getCurrent (rec1);
				if (!myComp ())
					matches++;
			}
        }

		//Check?
		if (matches == 320000) {
			countCorrect++;
		}
	}	
	
	QUNIT_IS_EQUAL (countCorrect, 2);
	if (countCorrect == 2) {
		cout << "PASS" << endl << flush;
	}
	else {
		cout << endl << endl << "***FAIL****" << endl << endl << flush;
	}
		

	case 9:
	cout << endl << "Test 9: Reduce run size:" << endl << flush;
	countCorrect = 0;		
	cout << "Sort a table.."  << flush;
	{
		// load up the table supplier table from the catalog
		MyDB_CatalogPtr myCatalog = make_shared <MyDB_Catalog> ("catFile");
		map <string, MyDB_TablePtr> allTables = MyDB_Table :: getAllTables (myCatalog);
		MyDB_BufferManagerPtr myMgr = make_shared <MyDB_BufferManager> (131072, 128, "tempFile");
		MyDB_TableReaderWriter supplierTable (allTables["supplier"], myMgr);

		// use the schema to create a table
		MyDB_TablePtr outTable = make_shared <MyDB_Table> ("supplierSorted", "supplierSorted.bin", allTables["supplier"]->getSchema ());
		MyDB_TableReaderWriter outputTable (outTable, myMgr);

		// get two empty records
		MyDB_RecordPtr rec1 = supplierTable.getEmptyRecord ();
		MyDB_RecordPtr rec2 = supplierTable.getEmptyRecord ();

		// and get a comparator
		function <bool ()> myComp = buildRecordComparator (rec1, rec2, "[acctbal]");

		// and sort
		sort (32, supplierTable, outputTable, myComp, rec1, rec2);

		MyDB_RecordIteratorAltPtr myIter = outputTable.getIteratorAlt ();

		// there should be 320000 records
		int counter = 0;
		while (myIter->advance ()) {
			myIter->getCurrent (rec1);
			counter++;
		}

		//Check?
		if (counter == 320000) {
			countCorrect++;
		}
		
		// put the supplier table into the catalog
		outTable->putInCatalog (myCatalog);
	}	
	
	cout << "Compare Sorted Table.." << endl << flush;
	{
		// load up the two tables from the catalog
		MyDB_CatalogPtr myCatalog = make_shared <MyDB_Catalog> ("catFile");
		map <string, MyDB_TablePtr> allTables = MyDB_Table :: getAllTables (myCatalog);
		MyDB_BufferManagerPtr myMgr = make_shared <MyDB_BufferManager> (131072, 128, "tempFile");
		MyDB_TableReaderWriter sortedTable (allTables["supplierSorted"], myMgr);
		MyDB_TableReaderWriter otherSortedTable (allTables["supplier"], myMgr);

		// load up the sorted text file
		otherSortedTable.loadFromTextFile ("supplierBigSorted.tbl");

		// get two empty records
		MyDB_RecordPtr rec1 = sortedTable.getEmptyRecord ();
		MyDB_RecordPtr rec2 = otherSortedTable.getEmptyRecord ();

		// and get a comparator
		function <bool ()> myComp = buildRecordComparator (rec1, rec2, "[acctbal]");

		// get two iterators
		MyDB_RecordIteratorAltPtr myIterOne = sortedTable.getIteratorAlt ();
		MyDB_RecordIteratorAltPtr myIterTwo = otherSortedTable.getIteratorAlt ();

		// make sure the results are the same
		int matches = 0;
        while (myIterOne->advance ()) {
			myIterTwo->advance ();

			// get the two records
			myIterOne->getCurrent (rec1);
			myIterTwo->getCurrent (rec2);

			if (!myComp ()) {
				myIterOne->getCurrent (rec2);
				myIterTwo->getCurrent (rec1);
				if (!myComp ())
					matches++;
			}
        }

		//Check?
		if (matches == 320000) {
			countCorrect++;
		}
	}	
	
	QUNIT_IS_EQUAL (countCorrect, 2);
	if (countCorrect == 2) {
		cout << "PASS" << endl << flush;
	}
	else {
		cout << endl << endl << "***FAIL****" << endl << endl << flush;
	}	
	
	
	case 10:
	cout << endl << "Test 10: Reduce run size even more:" << endl << flush;
	countCorrect = 0;		
	cout << "Sort a table.."  << flush;
	{
		// load up the table supplier table from the catalog
		MyDB_CatalogPtr myCatalog = make_shared <MyDB_Catalog> ("catFile");
		map <string, MyDB_TablePtr> allTables = MyDB_Table :: getAllTables (myCatalog);
		MyDB_BufferManagerPtr myMgr = make_shared <MyDB_BufferManager> (131072, 128, "tempFile");
		MyDB_TableReaderWriter supplierTable (allTables["supplier"], myMgr);

		// use the schema to create a table
		MyDB_TablePtr outTable = make_shared <MyDB_Table> ("supplierSortedRRS", "supplierSortedRRS.bin", allTables["supplier"]->getSchema ());
		MyDB_TableReaderWriter outputTable (outTable, myMgr);

		// get two empty records
		MyDB_RecordPtr rec1 = supplierTable.getEmptyRecord ();
		MyDB_RecordPtr rec2 = supplierTable.getEmptyRecord ();

		// and get a comparator
		function <bool ()> myComp = buildRecordComparator (rec1, rec2, "[acctbal]");

		// and sort
		sort (7, supplierTable, outputTable, myComp, rec1, rec2);

		MyDB_RecordIteratorAltPtr myIter = outputTable.getIteratorAlt ();

		// there should be 320000 records
		int counter = 0;
		while (myIter->advance ()) {
			myIter->getCurrent (rec1);
			counter++;
		}

		//Check?
		if (counter == 320000) {
			countCorrect++;
		}
		
		// put the supplier table into the catalog
		outTable->putInCatalog (myCatalog);
	}	
	
	cout << "Compare Sorted Table.." << endl << flush;
	{
		// load up the two tables from the catalog
		MyDB_CatalogPtr myCatalog = make_shared <MyDB_Catalog> ("catFile");
		map <string, MyDB_TablePtr> allTables = MyDB_Table :: getAllTables (myCatalog);
		MyDB_BufferManagerPtr myMgr = make_shared <MyDB_BufferManager> (131072, 128, "tempFile");
		MyDB_TableReaderWriter sortedTable (allTables["supplierSortedRRS"], myMgr);
		MyDB_TableReaderWriter otherSortedTable (allTables["supplier"], myMgr);

		// load up the sorted text file
		otherSortedTable.loadFromTextFile ("supplierBigSorted.tbl");

		// get two empty records
		MyDB_RecordPtr rec1 = sortedTable.getEmptyRecord ();
		MyDB_RecordPtr rec2 = otherSortedTable.getEmptyRecord ();

		// and get a comparator
		function <bool ()> myComp = buildRecordComparator (rec1, rec2, "[acctbal]");

		// get two iterators
		MyDB_RecordIteratorAltPtr myIterOne = sortedTable.getIteratorAlt ();
		MyDB_RecordIteratorAltPtr myIterTwo = otherSortedTable.getIteratorAlt ();

		// make sure the results are the same
		int matches = 0;
        while (myIterOne->advance ()) {
			myIterTwo->advance ();

			// get the two records
			myIterOne->getCurrent (rec1);
			myIterTwo->getCurrent (rec2);

			if (!myComp ()) {
				myIterOne->getCurrent (rec2);
				myIterTwo->getCurrent (rec1);
				if (!myComp ())
					matches++;
			}
        }

		//Check?
		if (matches == 320000) {
			countCorrect++;
		}
	}	
	
	QUNIT_IS_EQUAL (countCorrect, 2);
	if (countCorrect == 2) {
		cout << "PASS" << endl << flush;
	}
	else {
		cout << endl << endl << "***FAIL****" << endl << endl << flush;
	}		
	
	default:
		break;
  }
  
	
}

#endif

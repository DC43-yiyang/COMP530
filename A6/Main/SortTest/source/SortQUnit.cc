
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

int main () {

	QUnit::UnitTest qunit(cerr, QUnit::verbose);

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

	{
		// use the alternative iterator on the file
	        // load up the table supplier table from the catalog
                MyDB_CatalogPtr myCatalog = make_shared <MyDB_Catalog> ("catFile");
                map <string, MyDB_TablePtr> allTables = MyDB_Table :: getAllTables (myCatalog);
                MyDB_BufferManagerPtr myMgr = make_shared <MyDB_BufferManager> (131072, 128, "tempFile");
                MyDB_TableReaderWriter supplierTable (allTables["supplier"], myMgr);

                // test the iterator by looping through all of the records in the file
                MyDB_RecordPtr temp = supplierTable.getEmptyRecord ();
                MyDB_RecordIteratorAltPtr myIter = supplierTable.getIteratorAlt ();

                // there should be 320000 records
                int counter = 0;
                while (myIter->advance ()) {
                        myIter->getCurrent (temp);
                        counter++;
                }
                QUNIT_IS_EQUAL (counter, 320000);

		vector <MyDB_PageReaderWriter> allPages;
		for (int i = 0; i < supplierTable.getNumPages (); i++) {
			allPages.push_back (supplierTable[i]);	
		}

		myIter = getIteratorAlt (allPages);
                counter = 0;
                while (myIter->advance ()) {
                        myIter->getCurrent (temp);
                        counter++;
                }
                QUNIT_IS_EQUAL (counter, 320000);

	}

	{

		// load up the table supplier table from the catalog
		MyDB_CatalogPtr myCatalog = make_shared <MyDB_Catalog> ("catFile");
		map <string, MyDB_TablePtr> allTables = MyDB_Table :: getAllTables (myCatalog);
		MyDB_BufferManagerPtr myMgr = make_shared <MyDB_BufferManager> (131072, 128, "tempFile");
		MyDB_TableReaderWriter supplierTable (allTables["supplier"], myMgr);

		// now, go to the 37th page and iterate over it
		MyDB_RecordPtr temp = supplierTable.getEmptyRecord ();
		MyDB_RecordIteratorPtr myIter = supplierTable[36].getIterator (temp);
		while (myIter->hasNext ()) {
			myIter->getNext ();
		}


		// sort the contents of the page
		MyDB_RecordPtr temp2 = supplierTable.getEmptyRecord ();
		function <bool ()> myComp = buildRecordComparator (temp, temp2, "[acctbal]");
		MyDB_PageReaderWriterPtr sorted = supplierTable[36].sort (myComp, temp, temp2);

		// now, iterate again
		myIter = sorted->getIterator (temp);
		while (myIter->hasNext ()) {
			myIter->getNext ();
		}

	}

	{
		// load up the table supplier table from the catalog
		MyDB_CatalogPtr myCatalog = make_shared <MyDB_Catalog> ("catFile");
		map <string, MyDB_TablePtr> allTables = MyDB_Table :: getAllTables (myCatalog);
		MyDB_BufferManagerPtr myMgr = make_shared <MyDB_BufferManager> (131072, 128, "tempFile");
		MyDB_TableReaderWriter supplierTable (allTables["supplier"], myMgr);

		// use the schema to create a table
		MyDB_TablePtr outTable = make_shared <MyDB_Table> ("supplierSorted", 
			"supplierSorted.bin", allTables["supplier"]->getSchema ());
		MyDB_TableReaderWriter outputTable (outTable, myMgr);

		// get two empty records
		MyDB_RecordPtr rec1 = supplierTable.getEmptyRecord ();
		MyDB_RecordPtr rec2 = supplierTable.getEmptyRecord ();

		// and get a comparator
		function <bool ()> myComp = buildRecordComparator (rec1, rec2, "[acctbal]");

		// and sort
		system ("date");
		sort (64, supplierTable, outputTable, myComp, rec1, rec2);
		system ("date");

                MyDB_RecordIteratorAltPtr myIter = outputTable.getIteratorAlt ();

                // there should be 320000 records
                int counter = 0;
                while (myIter->advance ()) {
                        myIter->getCurrent (rec1);
                        counter++;
                }
                QUNIT_IS_EQUAL (counter, 320000);

		// put the supplier table into the catalog
		outTable->putInCatalog (myCatalog);
	}

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

			cout << rec1 << "\n";

			if (!myComp ()) {
                        	myIterOne->getCurrent (rec2);
                        	myIterTwo->getCurrent (rec1);
				if (!myComp ())
					matches++;
			}
                }

                QUNIT_IS_EQUAL (matches, 320000);
	}
}

#endif

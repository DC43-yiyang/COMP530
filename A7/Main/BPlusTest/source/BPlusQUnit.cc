
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
		MyDB_BPlusTreeReaderWriter supplierTable ("suppkey", myTable, myMgr);

		// load it from a text file
		supplierTable.loadFromTextFile ("supplierBig.tbl");

                // there should be 320000 records
                MyDB_RecordPtr temp = supplierTable.getEmptyRecord ();
                MyDB_RecordIteratorAltPtr myIter = supplierTable.getIteratorAlt ();

                int counter = 0;
                while (myIter->advance ()) {
                        myIter->getCurrent (temp);
                        counter++;
                }
                QUNIT_IS_EQUAL (counter, 320000);

		// now, we check 100 different random suppliers queries
		for (int time = 0; time < 2; time++) {
			for (int i = 0; i < 100; i++) {
	
				// we are looping through twice; the first time, ask only point queries
				srand48 (i);
				int lowBound = lrand48 () % 10000;
				int highBound = lrand48 () % 10000;
				if (time % 2 == 0)
					highBound = lowBound;
	
				// make sure the low bound is less than the high bound
				if (lowBound > highBound) {
					int temp = lowBound;
					lowBound = highBound;
					highBound = temp;
				}
	
				// ask a range query
				MyDB_IntAttValPtr low = make_shared <MyDB_IntAttVal> ();
				low->set (lowBound);
				MyDB_IntAttValPtr high = make_shared <MyDB_IntAttVal> ();
				high->set (highBound);

				if (i % 2 == 0) 
					myIter = supplierTable.getRangeIteratorAlt (low, high);
				else
					myIter = supplierTable.getSortedRangeIteratorAlt (low, high);
		
				// verify we got exactly the correct count back
				int counter = 0;
       		         	while (myIter->advance ()) {
       		                	myIter->getCurrent (temp);
					counter++;
       	         		}
	
       	         		QUNIT_IS_EQUAL (counter, 32 * (highBound - lowBound + 1));
			}
		}
	}
}

#endif

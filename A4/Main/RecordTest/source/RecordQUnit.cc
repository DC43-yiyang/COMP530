
#ifndef RECORD_TEST_H
#define RECORD_TEST_H

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
#include <cstring>
#include <iostream>
#include <time.h>
#include <unistd.h>
#include <vector>

#define FALLTHROUGH_INTENDED do {} while (0)

void initialize() {
	cout << "start initialization..." << flush;

	// create a catalog
	MyDB_CatalogPtr myCatalog = make_shared <MyDB_Catalog>("catFile");

	// now make a schema
	MyDB_SchemaPtr mySchema = make_shared <MyDB_Schema>();
	mySchema->appendAtt(make_pair("suppkey", make_shared <MyDB_IntAttType>()));
	mySchema->appendAtt(make_pair("name", make_shared <MyDB_StringAttType>()));
	mySchema->appendAtt(make_pair("address", make_shared <MyDB_StringAttType>()));
	mySchema->appendAtt(make_pair("nationkey", make_shared <MyDB_IntAttType>()));
	mySchema->appendAtt(make_pair("phone", make_shared <MyDB_StringAttType>()));
	mySchema->appendAtt(make_pair("acctbal", make_shared <MyDB_DoubleAttType>()));
	mySchema->appendAtt(make_pair("comment", make_shared <MyDB_StringAttType>()));

	// use the schema to create a table
	MyDB_TablePtr myTable = make_shared <MyDB_Table>("supplier", "supplier.bin", mySchema);
	MyDB_BufferManagerPtr myMgr = make_shared <MyDB_BufferManager>(1024, 16, "tempFile");
	MyDB_TableReaderWriter supplierTable(myTable, myMgr);

	// load it from a text file
	supplierTable.loadFromTextFile("supplier.tbl");

	// put the supplier table into the catalog
	myTable->putInCatalog(myCatalog);

	cout << "finish initialization..." << flush;
}

int main(int argc, char *argv[]) {
	int start = 1;
	if (argc > 1 && argv[1][0] >= '0' && argv[1][0] <= '9') {
		start = argv[1][0] - '0';
	}
	cout << "start from test " << start << endl << flush;

	QUnit::UnitTest qunit(cerr, QUnit::normal);

	// dependency: the provided supplier.tbl
	// dependency: matching precision for streaming out double numbers

	switch (start) {
	case 1:
	{
		// table hasNext
		cout << "TEST 1..." << flush;
		initialize();
		bool result = false;
		{
			cout << "create manager..." << flush;
			MyDB_CatalogPtr myCatalog = make_shared <MyDB_Catalog>("catFile");
			map <string, MyDB_TablePtr> allTables = MyDB_Table::getAllTables(myCatalog);
			MyDB_BufferManagerPtr myMgr = make_shared <MyDB_BufferManager>(1024, 16, "tempFile");

			cout << "create TableReaderWriter..." << flush;
			MyDB_TableReaderWriter supplierTable(allTables["supplier"], myMgr);
			MyDB_RecordPtr temp = supplierTable.getEmptyRecord();

			cout << "create TableIterator..." << flush;
			MyDB_RecordIteratorPtr myIter = supplierTable.getIterator(temp);

			cout << "get result..." << flush;
			result = myIter->hasNext();

			cout << "shutdown manager..." << flush;
		}
		if (result) cout << "CORRECT" << endl << flush;
		else cout << "***FAIL***" << endl << flush;
		QUNIT_IS_TRUE(result);
	}
	FALLTHROUGH_INTENDED;
	case 2:
	{
		// page hasNext
		cout << "TEST 2..." << flush;
		initialize();
		bool result = false;
		{
			cout << "create manager..." << flush;
			MyDB_CatalogPtr myCatalog = make_shared <MyDB_Catalog>("catFile");
			map <string, MyDB_TablePtr> allTables = MyDB_Table::getAllTables(myCatalog);
			MyDB_BufferManagerPtr myMgr = make_shared <MyDB_BufferManager>(1024, 16, "tempFile");

			cout << "create TableReaderWriter..." << flush;
			MyDB_TableReaderWriter supplierTable(allTables["supplier"], myMgr);
			MyDB_RecordPtr temp = supplierTable.getEmptyRecord();

			cout << "create PageIterator..." << flush;
			MyDB_RecordIteratorPtr myIter = supplierTable[0].getIterator(temp);

			cout << "get result..." << flush;
			result = myIter->hasNext();

			cout << "shutdown manager..." << flush;
		}
		if (result) cout << "CORRECT" << endl << flush;
		else cout << "***FAIL***" << endl << flush;
		QUNIT_IS_TRUE(result);
	}
	FALLTHROUGH_INTENDED;
	case 3:
	{
		// count records with table iterator
		cout << "TEST 3..." << flush;
		initialize();
		int counter = 0;
		{
			cout << "create manager..." << flush;
			MyDB_CatalogPtr myCatalog = make_shared <MyDB_Catalog>("catFile");
			map <string, MyDB_TablePtr> allTables = MyDB_Table::getAllTables(myCatalog);
			MyDB_BufferManagerPtr myMgr = make_shared <MyDB_BufferManager>(1024, 16, "tempFile");

			cout << "create TableReaderWriter..." << flush;
			MyDB_TableReaderWriter supplierTable(allTables["supplier"], myMgr);
			MyDB_RecordPtr temp = supplierTable.getEmptyRecord();

			cout << "create TableIterator..." << flush;
			MyDB_RecordIteratorPtr myIter = supplierTable.getIterator(temp);

			cout << "count..." << flush;
			while (myIter->hasNext()) {
				myIter->getNext();
				counter++;
			}

			cout << "shutdown manager..." << flush;
		}
		if (counter == 10000) cout << "CORRECT" << endl << flush;
		else cout << "***FAIL***" << endl << flush;
		QUNIT_IS_EQUAL(counter, 10000);
	}
	FALLTHROUGH_INTENDED;
	case 4:
	{
		// table append record
		cout << "TEST 4..." << flush;
		initialize();
		int counter = 0;
		{
			cout << "create manager..." << flush;
			MyDB_CatalogPtr myCatalog = make_shared <MyDB_Catalog>("catFile");
			map <string, MyDB_TablePtr> allTables = MyDB_Table::getAllTables(myCatalog);
			MyDB_BufferManagerPtr myMgr = make_shared <MyDB_BufferManager>(1024, 16, "tempFile");

			cout << "create TableReaderWriter..." << flush;
			MyDB_TableReaderWriter supplierTable(allTables["supplier"], myMgr);
			MyDB_RecordPtr temp = supplierTable.getEmptyRecord();

			cout << "generate record..." << flush;
			string s = "10001|Supplier#000010001|00000000|999|12-345-678-9012|1234.56|the special record|";
			temp->fromString(s);

			cout << "append record..." << flush;
			supplierTable.append(temp);

			cout << "create TableIterator..." << flush;
			MyDB_RecordIteratorPtr myIter = supplierTable.getIterator(temp);

			cout << "count..." << flush;
			while (myIter->hasNext()) {
				myIter->getNext();
				counter++;
			}

			cout << "shutdown manager..." << flush;
		}
		if (counter == 10001) cout << "CORRECT" << endl << flush;
		else cout << "***FAIL***" << endl << flush;
		QUNIT_IS_EQUAL(counter, 10001);
	}
	FALLTHROUGH_INTENDED;
	case 5:
	{
		// verify the 2nd record with table iterator
		cout << "TEST 5..." << flush;
		initialize();
		string result = "";
		{
			cout << "create manager..." << flush;
			MyDB_CatalogPtr myCatalog = make_shared <MyDB_Catalog>("catFile");
			map <string, MyDB_TablePtr> allTables = MyDB_Table::getAllTables(myCatalog);
			MyDB_BufferManagerPtr myMgr = make_shared <MyDB_BufferManager>(1024, 16, "tempFile");

			cout << "create TableReaderWriter..." << flush;
			MyDB_TableReaderWriter supplierTable(allTables["supplier"], myMgr);
			MyDB_RecordPtr temp = supplierTable.getEmptyRecord();

			cout << "create TableIterator..." << flush;
			MyDB_RecordIteratorPtr myIter = supplierTable.getIterator(temp);

			cout << "next 2nd record..." << flush;
			if (myIter->hasNext()) {
				myIter->getNext();
			}
			if (myIter->hasNext()) {
				myIter->getNext();
			}
			
			cout << "read record..." << flush;
			stringstream ss;
			ss << temp;
			result = ss.str();

			cout << "shutdown manager..." << flush;
		}
		const string answer = "2|Supplier#000000002|TRMhVHz3XiFuhapxucPo1|5|15-679-861-2259|4032.680000|furiously stealthy frays thrash alongside of the slyly express deposits. blithely regular req|";
		if (result == answer) cout << "CORRECT" << endl << flush;
		else cout << "***FAIL***" << endl << flush;
		QUNIT_IS_EQUAL(result, answer);
	}
	FALLTHROUGH_INTENDED;
	case 6:
	{
		// verify the 10000th record with page iterator
		// you will fail if you store only one record per page
		cout << "TEST 6..." << flush;
		initialize();
		string result = "";
		{
			cout << "create manager..." << flush;
			MyDB_CatalogPtr myCatalog = make_shared <MyDB_Catalog>("catFile");
			map <string, MyDB_TablePtr> allTables = MyDB_Table::getAllTables(myCatalog);
			MyDB_BufferManagerPtr myMgr = make_shared <MyDB_BufferManager>(1024, 16, "tempFile");

			cout << "create TableReaderWriter..." << flush;
			MyDB_TableReaderWriter supplierTable(allTables["supplier"], myMgr);
			MyDB_RecordPtr temp = supplierTable.getEmptyRecord();

			cout << "page by page..." << flush;
			int counter = 0;
			int page = 0;
			bool flag = true;
			while (flag) {
				MyDB_RecordIteratorPtr myIter = supplierTable[page].getIterator(temp);
				while (flag && myIter->hasNext()) {
					myIter->getNext();
					counter++;
					if (counter >= 10000) flag = false;
				}
				page++;
				if (page > 5000) flag = false;
			}
			cout << "page " << page << "...counter " << counter << "..." << flush;

			cout << "read record..." << flush;
			stringstream ss;
			ss << temp;
			result = ss.str();

			cout << "shutdown manager..." << flush;
		}
		const string answer = "10000|Supplier#000010000|R7kfmyzoIfXlrbnqNwUUW3phJctocp0J|19|29-578-432-2146|8968.420000|furiously final ideas believe furiously. furiously final ideas|";
		if (result == answer) cout << "CORRECT" << endl << flush;
		else cout << "***FAIL***" << endl << flush;
		QUNIT_IS_EQUAL(result, answer);
	}
	FALLTHROUGH_INTENDED;
	case 7:
	{
		// independent table iterators
		cout << "TEST 7..." << flush;
		initialize();
		int counter = 0;
		{
			cout << "create manager..." << flush;
			MyDB_CatalogPtr myCatalog = make_shared <MyDB_Catalog>("catFile");
			map <string, MyDB_TablePtr> allTables = MyDB_Table::getAllTables(myCatalog);
			MyDB_BufferManagerPtr myMgr = make_shared <MyDB_BufferManager>(1024, 16, "tempFile");

			cout << "create TableReaderWriter..." << flush;
			MyDB_TableReaderWriter supplierTable(allTables["supplier"], myMgr);
			MyDB_RecordPtr temp = supplierTable.getEmptyRecord();

			cout << "create TableIterator..." << flush;
			MyDB_RecordIteratorPtr myIter1 = supplierTable.getIterator(temp);
			MyDB_RecordIteratorPtr myIter2 = supplierTable.getIterator(temp);

			cout << "count..." << flush;
			while (myIter1->hasNext() || myIter2->hasNext()) {
				if (myIter1->hasNext()) {
					myIter1->getNext();
					counter++;
				}
				if (myIter1->hasNext()) {
					myIter1->getNext();
					counter++;
				}
				if (myIter2->hasNext()) {
					myIter2->getNext();
					counter++;
				}
			}

			cout << "shutdown manager..." << flush;
		}
		if (counter == 20000) cout << "CORRECT" << endl << flush;
		else cout << "***FAIL***" << endl << flush;
		QUNIT_IS_EQUAL(counter, 20000);
	}
	FALLTHROUGH_INTENDED;
	case 8:
	{
		// clear the 33rd page
		cout << "TEST 8..." << flush;
		initialize();
		int counter = 0;
		{
			cout << "create manager..." << flush;
			MyDB_CatalogPtr myCatalog = make_shared <MyDB_Catalog>("catFile");
			map <string, MyDB_TablePtr> allTables = MyDB_Table::getAllTables(myCatalog);
			MyDB_BufferManagerPtr myMgr = make_shared <MyDB_BufferManager>(1024, 16, "tempFile");

			cout << "create TableReaderWriter..." << flush;
			MyDB_TableReaderWriter supplierTable(allTables["supplier"], myMgr);
			MyDB_RecordPtr temp = supplierTable.getEmptyRecord();

			cout << "create PageIterator..." << flush;
			MyDB_RecordIteratorPtr myIter1 = supplierTable[33].getIterator(temp);

			cout << "count records in page 33..." << flush;
			while (myIter1->hasNext()) {
				myIter1->getNext();
				counter++;
			}

			cout << "clear page 33..." << flush;
			supplierTable[33].clear();

			cout << "create TableIterator..." << flush;
			MyDB_RecordIteratorPtr myIter2 = supplierTable.getIterator(temp);

			cout << "count records in table..." << flush;
			while (myIter2->hasNext()) {
				myIter2->getNext();
				counter++;
			}

			cout << "shutdown manager..." << flush;
		}
		if (counter == 10000) cout << "CORRECT" << endl << flush;
		else cout << "***FAIL***" << endl << flush;
		QUNIT_IS_EQUAL(counter, 10000);
	}
	FALLTHROUGH_INTENDED;
	case 9:
	{
		// replace the 55th page with the last page
		cout << "TEST 9..." << flush;
		initialize();
		int counter = 0;
		{
			cout << "create manager..." << flush;
			MyDB_CatalogPtr myCatalog = make_shared <MyDB_Catalog>("catFile");
			map <string, MyDB_TablePtr> allTables = MyDB_Table::getAllTables(myCatalog);
			MyDB_BufferManagerPtr myMgr = make_shared <MyDB_BufferManager>(1024, 16, "tempFile");

			cout << "create TableReaderWriter..." << flush;
			MyDB_TableReaderWriter supplierTable(allTables["supplier"], myMgr);
			MyDB_RecordPtr temp = supplierTable.getEmptyRecord();

			cout << "create PageIterator..." << flush;
			MyDB_RecordIteratorPtr myIter1 = supplierTable[55].getIterator(temp);
			MyDB_RecordIteratorPtr myIter2 = supplierTable.last().getIterator(temp);

			cout << "count records in page 55..." << flush;
			while (myIter1->hasNext()) {
				myIter1->getNext();
				counter++;
			}

			cout << "clear page 55..." << flush;
			supplierTable[55].clear();

			cout << "count records in the last page and copy to page 55..." << flush;
			while (myIter2->hasNext()) {
				myIter2->getNext();
				supplierTable[55].append(temp);
				counter--;
			}

			cout << "create TableIterator..." << flush;
			MyDB_RecordIteratorPtr myIter3 = supplierTable.getIterator(temp);

			cout << "count records in table..." << flush;
			while (myIter3->hasNext()) {
				myIter3->getNext();
				counter++;
			}

			cout << "shutdown manager..." << flush;
		}
		if (counter == 10000) cout << "CORRECT" << endl << flush;
		else cout << "***FAIL***" << endl << flush;
		QUNIT_IS_EQUAL(counter, 10000);
	}
	FALLTHROUGH_INTENDED;
	case 0:
	{
		// table hasNext with all pages cleared
		cout << "TEST 0..." << flush;
		initialize();
		bool result = false;
		{
			cout << "create manager..." << flush;
			MyDB_CatalogPtr myCatalog = make_shared <MyDB_Catalog>("catFile");
			map <string, MyDB_TablePtr> allTables = MyDB_Table::getAllTables(myCatalog);
			MyDB_BufferManagerPtr myMgr = make_shared <MyDB_BufferManager>(1024, 16, "tempFile");

			cout << "create TableReaderWriter..." << flush;
			MyDB_TableReaderWriter supplierTable(allTables["supplier"], myMgr);
			MyDB_RecordPtr temp = supplierTable.getEmptyRecord();

			cout << "page by page..." << flush;
			int counter = 0;
			int page = 0;
			bool flag = true;
			while (flag) {
				MyDB_RecordIteratorPtr myIter = supplierTable[page].getIterator(temp);
				while (flag && myIter->hasNext()) {
					myIter->getNext();
					counter++;
					if (counter >= 10000) flag = false;
				}
				supplierTable[page].clear();
				page++;
				if (page > 10000) flag = false;
			}
			cout << "page " << page << "...counter " << counter << "..." << flush;

			cout << "create TableIterator..." << flush;
			MyDB_RecordIteratorPtr myIter = supplierTable.getIterator(temp);

			cout << "get result..." << flush;
			result = myIter->hasNext();

			cout << "shutdown manager..." << flush;
		}
		if (result == false) cout << "CORRECT" << endl << flush;
		else cout << "***FAIL***" << endl << flush;
		QUNIT_IS_FALSE(result);
	}
	FALLTHROUGH_INTENDED;
	default:
		break;
	}
}

#endif

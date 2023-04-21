
#ifndef CATALOG_UNIT_H
#define CATALOG_UNIT_H

#include "MyDB_BufferManager.h"
#include "MyDB_PageHandle.h"
#include "MyDB_Table.h"
#include "QUnit.h"
#include <cstring>
#include <iostream>
#include <time.h>
#include <unistd.h>
#include <vector>

using namespace std;

int main () {

	//QUnit::UnitTest qunit(cerr, QUnit::verbose);
	QUnit::UnitTest qunit(cerr, QUnit::normal);

	// buffer manager and temp page
	cout << "TEST 1..." << flush;
	{
		cout << "create manager..." << flush;
		MyDB_BufferManager myMgr(64, 16, "tempDSFSD");
		cout << "get page..." << flush;
		MyDB_PageHandle page1 = myMgr.getPage();
		cout << "get bytes..." << flush;
		char *bytes = (char *)page1->getBytes();
		cout << "write bytes..." << flush;
		memset(bytes, 'A', 64);
		page1->wroteBytes();
		cout << "shutdown manager..." << flush;
	}
	cout << "COMPLETE" << endl << flush;
	QUNIT_IS_TRUE(true);

	// write unpinned and pinned page
	cout << "TEST 2..." << flush;
	{
		cout << "create manager..." << flush;
		MyDB_BufferManager myMgr(64, 16, "tempDSFSD");
		cout << "get page..." << flush;
		MyDB_TablePtr table1 = make_shared <MyDB_Table>("table1", "file1");
		MyDB_TablePtr table2 = make_shared <MyDB_Table>("table2", "file2");
		MyDB_PageHandle page1 = myMgr.getPage(table1, 0);
		MyDB_PageHandle page2 = myMgr.getPinnedPage(table2, 1);
		cout << "get bytes..." << flush;
		char *bytes1 = (char *)page1->getBytes();
		char *bytes2 = (char *)page2->getBytes();
		cout << "write bytes..." << flush;
		memset(bytes1, 'A', 64);
		page1->wroteBytes();
		memset(bytes2, 'B', 64);
		page2->wroteBytes();
		cout << "shutdown manager..." << flush;
	}
	cout << "COMPLETE" << endl << flush;
	QUNIT_IS_TRUE(true);

	// read unpinned and pinned page (requires write unpinned and pinned page)
	bool flag3 = true;
	cout << "TEST 3..." << flush;
	{
		cout << "create manager..." << flush;
		MyDB_BufferManager myMgr(64, 16, "tempDSFSD");
		cout << "get page..." << flush;
		MyDB_TablePtr table1 = make_shared <MyDB_Table>("table1", "file1");
		MyDB_TablePtr table2 = make_shared <MyDB_Table>("table2", "file2");
		MyDB_PageHandle page1 = myMgr.getPage(table1, 0);
		MyDB_PageHandle page2 = myMgr.getPinnedPage(table2, 1);
		cout << "get bytes..." << flush;
		char *bytes1 = (char *)page1->getBytes();
		char *bytes2 = (char *)page2->getBytes();
		cout << "compare bytes..." << flush;
		for (int i = 0; i < 64; i++) {
			if (bytes1[i] != 'A') flag3 = false;
			if (bytes2[i] != 'B') flag3 = false;
		}
		if (flag3) cout << "correct..." << flush;
		else cout << "INCORRECT..." << flush;
		cout << "shutdown manager..." << flush;
	}
	cout << "COMPLETE" << endl << flush;
	QUNIT_IS_TRUE(flag3);

	// write large pages
	cout << "TEST 4..." << flush;
	{
		cout << "create manager..." << flush;
		MyDB_BufferManager myMgr(1048576, 16, "tempDSFSD");
		cout << "get page..." << flush;
		MyDB_TablePtr table1 = make_shared <MyDB_Table>("table1", "file1");
		vector<MyDB_PageHandle> pages(16);
		for (int i = 0; i < 16; i++) {
			pages[i] = myMgr.getPinnedPage(table1, i);
		}
		cout << "get bytes..." << flush;
		vector<char*> bytes(16);
		for (int i = 0; i < 16; i++) {
			bytes[i] = (char *)pages[i]->getBytes();
		}
		cout << "write bytes..." << flush;
		for (int i = 0; i < 16; i++) {
			memset(bytes[i], 'C', 1048576);
			pages[i]->wroteBytes();
		}
		cout << "shutdown manager..." << flush;
	}
	cout << "COMPLETE" << endl << flush;
	QUNIT_IS_TRUE(true);

	// large LRU
	cout << "TEST 5..." << flush;
	{
		cout << "create manager..." << flush;
		MyDB_BufferManager myMgr(64, 100000, "tempDSFSD");
		cout << "get page..." << flush;
		MyDB_TablePtr table1 = make_shared <MyDB_Table>("table1", "file1");
		vector<MyDB_PageHandle> pages(100000);
		for (int i = 0; i < 100000; i++) {
			pages[i] = myMgr.getPage(table1, i);
		}
		cout << "get bytes..." << flush;
		vector<char*> bytes(100000);
		for (int i = 0; i < 100000; i++) {
			bytes[i] = (char *)pages[i]->getBytes();
		}
		cout << "shutdown manager..." << flush;
	}
	cout << "COMPLETE" << endl << flush;
	QUNIT_IS_TRUE(true);

	// alternate slot
	cout << "TEST 6..." << flush;
	{
		cout << "create manager..." << flush;
		MyDB_BufferManager myMgr(64, 16, "tempDSFSD");
		cout << "get page..." << flush;
		MyDB_TablePtr table1 = make_shared <MyDB_Table>("table1", "file1");
		vector<MyDB_PageHandle> pages(17);
		for (int i = 0; i < 15; i++) {
			pages[i] = myMgr.getPinnedPage(table1, i);
		}
		for (int i = 15; i < 17; i++) {
			pages[i] = myMgr.getPage(table1, i);
		}
		cout << "get bytes..." << flush;
		clock_t t1, t2, t3;
		volatile char *bytes1, *bytes2;
		t1 = clock(); 
		for (int i = 0; i < 100000; i++) {
			bytes1 = (char *)pages[13]->getBytes();
			bytes2 = (char *)pages[14]->getBytes();
		}
		t2 = clock();
		for (int i = 0; i < 100000; i++) {
			bytes1 = (char *)pages[15]->getBytes();
			bytes2 = (char *)pages[16]->getBytes();
		}
		t3 = clock();
		cout << t2 - t1 << "..." << t3 - t2 << "...";
		cout << "shutdown manager..." << flush;
	}
	cout << "COMPLETE" << endl << flush;
	QUNIT_IS_TRUE(true);

	// rolling LRU
	cout << "TEST 7..." << flush;
	{
		cout << "create manager..." << flush;
		MyDB_BufferManager myMgr(64, 100, "tempDSFSD");
		cout << "get page..." << flush;
		MyDB_TablePtr table1 = make_shared <MyDB_Table>("table1", "file1");
		vector<MyDB_PageHandle> pages(101);
		for (int i = 0; i < 101; i++) {
			pages[i] = myMgr.getPage(table1, i);
		}
		cout << "get bytes..." << flush;
		clock_t t1, t2, t3;
		volatile char *bytes1;
		t1 = clock(); 
		for (int i = 0; i < 1000; i++) {
			for (int j = 0; j < 100; j++) {
				bytes1 = (char *)pages[j]->getBytes();
			}
		}
		t2 = clock();
		for (int i = 0; i < 1000; i++) {
			for (int j = 0; j < 101; j++) {
				bytes1 = (char *)pages[j]->getBytes();
			}
		}
		t3 = clock();
		cout << t2 - t1 << "..." << t3 - t2 << "...";
		cout << "shutdown manager..." << flush;
	}
	cout << "COMPLETE" << endl << flush;
	QUNIT_IS_TRUE(true);

	// rolling temp
	cout << "TEST 8..." << flush;
	bool flag8 = true;
	{
		cout << "create manager..." << flush;
		MyDB_BufferManager myMgr(64, 16, "tempDSFSD");
		cout << "get page..." << flush;
		vector<MyDB_PageHandle> pages(50);
		for (int i = 0; i < 50; i++) {
			pages[i] = myMgr.getPage();
		}
		cout << "write bytes..." << flush;
		vector<char*> bytes(50);
		for (int i = 0; i < 50; i++) {
			bytes[i] = (char *)pages[i]->getBytes();
			memset(bytes[i], (char)('A' + i), 64);
			pages[i]->wroteBytes();
		}
		cout << "read bytes..." << flush;
		for (int i = 0; i < 50; i++) {
			bytes[i] = (char *)pages[i]->getBytes();
			char c = (char)('A' + i);
			for (int j = 0; j < 64; j++) {
				if (bytes[i][j] != c) flag8 = false;
			}
		}
		if (flag8) cout << "correct..." << flush;
		else cout << "INCORRECT..." << flush;
		cout << "shutdown manager..." << flush;
	}
	cout << "COMPLETE" << endl << flush;
	QUNIT_IS_TRUE(flag8);

	// multiple handles
	bool flag9 = true;
	cout << "TEST 9..." << flush;
	{
		cout << "create manager..." << flush;
		MyDB_BufferManager myMgr(64, 16, "tempDSFSD");
		cout << "get page..." << flush;
		MyDB_TablePtr table1 = make_shared <MyDB_Table>("table1", "file1");
		vector<MyDB_PageHandle> pagesA(16);
		vector<MyDB_PageHandle> pagesB(16);
		vector<MyDB_PageHandle> pagesC(16);
		for (int i = 0; i < 16; i++) {
			pagesA[i] = myMgr.getPage(table1, i);
			pagesB[i] = myMgr.getPage(table1, i);
			pagesC[i] = myMgr.getPage(table1, i);
		}
		cout << "write bytes..." << flush;
		for (int i = 0; i < 16; i++) {
			char *bytes = (char *)pagesA[i]->getBytes();
			memset(bytes, (char)('A' + i), 64);
			pagesA[i]->wroteBytes();
		}
		for (int i = 0; i < 16; i++) {
			char *bytes = (char *)pagesB[i]->getBytes();
			memset(bytes, (char)('a' + i), 64);
			pagesB[i]->wroteBytes();
		}
		cout << "read bytes..." << flush;
		for (int i = 0; i < 16; i++) {
			char *bytes = (char *)pagesC[i]->getBytes();
			char c = (char)('a' + i);
			for (int j = 0; j < 64; j++) {
				if (bytes[j] != c) flag9 = false;
			}
		}
		cout << "shutdown manager..." << flush;
	}
	cout << "COMPLETE" << endl << flush;
	QUNIT_IS_TRUE(flag9);
}

#endif


#ifndef CATALOG_UNIT_H
#define CATALOG_UNIT_H

#define FALLTHROUGH_INTENDED do {} while (0)

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

// these functions write a bunch of characters to a string... used to produce data
void writeNums (char *bytes, size_t len, int i) {
	for (size_t j = 0; j < len - 1; j++) {
		bytes[j] = '0' + (i % 10);
	}
	bytes[len - 1] = 0;
}

void writeLetters (char *bytes, size_t len, int i) {
	for (size_t j = 0; j < len - 1; j++) {
		bytes[j] = 'i' + (i % 10);
	}
	bytes[len - 1] = 0;
}

void writeSymbols (char *bytes, size_t len, int i) {
	for (size_t j = 0; j < len - 1; j++) {
		bytes[j] = '!' + (i % 10);
	}
	bytes[len - 1] = 0;
}

int main (int argc, char *argv[]) {
	int start = 1;
	if (argc > 1 && argv[1][0] >= '0' && argv[1][0] <= '9') {
		start = argv[1][0] - '0';
	}
	cout << "start from test " << start << endl;

	QUnit::UnitTest qunit(cerr, QUnit::normal);

	switch (start) {
	case 1:
		{
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
		}
		FALLTHROUGH_INTENDED;
	case 2:
		{
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
		}
		FALLTHROUGH_INTENDED;
	case 3:
		{
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
				cout << "shutdown manager..." << flush;
			}
			if (flag3) cout << "CORRECT" << endl << flush;
			else cout << "***FAIL***" << endl << flush;
			QUNIT_IS_TRUE(flag3);
		}
		FALLTHROUGH_INTENDED;
	case 4:
		{
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
		}
		FALLTHROUGH_INTENDED;
	case 5:
		{
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
		}
		FALLTHROUGH_INTENDED;
	case 6:
		{
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
		}
		FALLTHROUGH_INTENDED;
	case 7:
		{
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
		}
		FALLTHROUGH_INTENDED;
	case 8:
		{
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
				cout << "shutdown manager..." << flush;
			}
			if (flag8) cout << "CORRECT" << endl << flush;
			else cout << "***FAIL***" << endl << flush;
			QUNIT_IS_TRUE(flag8);
		}
		FALLTHROUGH_INTENDED;
	case 9:
		{
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
		FALLTHROUGH_INTENDED;
	case 0:
		{
			// test case by Professor Chris
			cout << "TEST 0..." << flush;
			{

				// create a buffer manager
				cout << "create manager..." << flush;
				MyDB_BufferManager myMgr (64, 16, "tempDSFSD");
				MyDB_TablePtr table1 = make_shared <MyDB_Table> ("tempTable", "foobar");

				// allocate a pinned page
				cout << "get pinned page 0..." << flush;
				MyDB_PageHandle pinnedPage = myMgr.getPinnedPage (table1, 0);
				char *bytes = (char *) pinnedPage->getBytes ();
				writeNums (bytes, 64, 0);
				pinnedPage->wroteBytes ();

		
				// create a bunch of pinned pages and remember them
				cout << "get pinned page 1~9..." << flush;
				vector <MyDB_PageHandle> myHandles;
				for (int i = 1; i < 10; i++) {
					MyDB_PageHandle temp = myMgr.getPinnedPage (table1, i);
					char *bytes = (char *) temp->getBytes ();
					writeNums (bytes, 64, i);
					temp->wroteBytes ();
					myHandles.push_back (temp);
				}

				// now forget the pages we created
				cout << "forget pinned page 1~9..." << flush;
				vector <MyDB_PageHandle> temp;
				myHandles = temp;

				// now remember 8 more pages
				cout << "get pinned page 0~7..." << flush;
				for (int i = 0; i < 8; i++) {
					MyDB_PageHandle temp = myMgr.getPinnedPage (table1, i);
					char *bytes = (char *) temp->getBytes ();

					// write numbers at the 0th position
					if (i == 0)
						writeNums (bytes, 64, i);
					else
						writeSymbols (bytes, 64, i);
					temp->wroteBytes ();
					myHandles.push_back (temp);
				}

				// now correctly write nums at the 0th position
				cout << "get unpinned page 0..." << flush;
				MyDB_PageHandle anotherDude = myMgr.getPage (table1, 0);
				bytes = (char *) anotherDude->getBytes ();
				writeSymbols (bytes, 64, 0);
				anotherDude->wroteBytes ();
		
				// now do 90 more pages, for which we forget the handle immediately
				cout << "get and forget unpinned page 10~99..." << flush;
				for (int i = 10; i < 100; i++) {
					MyDB_PageHandle temp = myMgr.getPage (table1, i);
					char *bytes = (char *) temp->getBytes ();
					writeNums (bytes, 64, i);
					temp->wroteBytes ();
				}

				// now forget all of the pinned pages we were remembering
				cout << "forget pinned page 0~7..." << flush;
				vector <MyDB_PageHandle> temp2;
				myHandles = temp2;

				// now get a pair of pages and write them
				cout << "get and forget 200 temp pages..." << flush;
				for (int i = 0; i < 100; i++) {
					MyDB_PageHandle oneHandle = myMgr.getPinnedPage ();
					char *bytes = (char *) oneHandle->getBytes ();
					writeNums (bytes, 64, i);
					oneHandle->wroteBytes ();
					MyDB_PageHandle twoHandle = myMgr.getPinnedPage ();
					writeNums (bytes, 64, i);
					twoHandle->wroteBytes ();
				}

				// make a second table
				cout << "get unpinned page 0~99 of second table..." << flush;
				MyDB_TablePtr table2 = make_shared <MyDB_Table> ("tempTable2", "barfoo");
				for (int i = 0; i < 100; i++) {
					MyDB_PageHandle temp = myMgr.getPage (table2, i);
					char *bytes = (char *) temp->getBytes ();
					writeLetters (bytes, 64, i);
					temp->wroteBytes ();
				}
				cout << "shutdown manager..." << flush;
			}

			// remove the temp file
			cout << "delete tempfile..." << flush;
			unlink ("tempFile");

			int countCorrect = 0;
			{
				cout << "create manager..." << flush;
				MyDB_BufferManager myMgr (64, 16, "tempDSFSD");
				MyDB_TablePtr table1 = make_shared <MyDB_Table> ("tempTable", "foobar");

				// look up all of the pages, and make sure they have the correct numbers
				cout << "get and verify unpinned page 0~99..." << flush;
				for (int i = 0; i < 100; i++) {
					MyDB_PageHandle temp = myMgr.getPage (table1, i);
					char answer[64];
					if (i < 8)
						writeSymbols (answer, 64, i);
					else
						writeNums (answer, 64, i);
					char *bytes = (char *) temp->getBytes ();
					if (string (answer) == string (bytes))
						countCorrect++;
				}
				cout << "shutdown manager..." << flush;
			}
			string temp = to_string(countCorrect) + string (" test cases correct");
			cout << temp << "..." << flush;
			if (temp == string("100 test cases correct")) cout << "CORRECT" << endl << flush;
			else cout << "***FAIL***" << endl << flush;
			QUNIT_IS_EQUAL (string("100 test cases correct"), temp);
		}
		FALLTHROUGH_INTENDED;

	default:
		break;
	}
}

#endif


#ifndef BUFFER_MGR_H
#define BUFFER_MGR_H

#include "MyDB_PageHandle.h"
#include "MyDB_Table.h"
#include "MyDB_Page.h"
#include "LRU.h"
#include <fcntl.h>
#include <vector>
#include <unordered_map>
#include <stdio.h>
#include <iostream>

using namespace std;

class MyDB_PageHandleBase;
class LRU;
typedef shared_ptr <MyDB_PageHandleBase> MyDB_PageHandle;

class MyDB_BufferManager {

public:

	// THESE METHODS MUST APPEAR AND THE PROTOTYPES CANNOT CHANGE!

	// gets the i^th page in the table whichTable... note that if the page
	// is currently being used (that is, the page is current buffered) a handle 
	// to that already-buffered page should be returned
	MyDB_PageHandle getPage (MyDB_TablePtr whichTable, long i);

	// gets a temporary page that will no longer exist (1) after the buffer manager
	// has been destroyed, or (2) there are no more references to it anywhere in the
	// program.  Typically such a temporary page will be used as buffer memory.
	// since it is just a temp page, it is not associated with any particular 
	// table
	MyDB_PageHandle getPage ();

	// gets the i^th page in the table whichTable... the only difference 
	// between this method and getPage (whichTable, i) is that the page will be
	// pinned in RAM; it cannot be written out to the file
	MyDB_PageHandle getPinnedPage (MyDB_TablePtr whichTable, long i);

	// gets a temporary page, like getPage (), except that this one is pinned
	MyDB_PageHandle getPinnedPage ();

	// un-pins the specified page
	void unpin (MyDB_PageHandle unpinMe);


	// If buffer manager has no available space for new page.
	// LRU needs to pop the first node of LRU and write back to disk.
	char* evict();  // 包含update；

	// If page has already existed in LRU, the page needs to be updated in LRU.
	// Index the node according to existed page and put it back of LRU.
	void update(Page* page);

	// 1. Read from disk and set the information into buffer address.
	// 2. Insert a new node into back of LRU
	void insert(Page* page);

	// Write anonymous and non-anonymous page back to table or temp file.
	void readFromDisk(Page *page);

	// Read from table or temp file.
	void writeToDisk(Page* page);

	// creates an LRU buffer manager... params are as follows:
	// 1) the size of each page is pageSize
	// 2) the number of pages managed by the buffer manager is numPages;
	// 3) temporary pages are written to the file tempFile
	MyDB_BufferManager (size_t pageSize, size_t numPages, string tempFile);


	// when the buffer manager is destroyed, all of the dirty pages need to be
	// written back to disk, any necessary data needs to be written to the catalog,
	// and any temporary files need to be deleted
	~MyDB_BufferManager ();

	// FEEL FREE TO ADD ADDITIONAL PUBLIC METHODS

	vector<char* > space;
	vector<int> anonymousSpace;


private:
	size_t pageSize;
	size_t numPage;
	string tempFile;
	char* buffer;

	// Use for checking the page whether it used to exist in buffer
	// key: tableName_i
	// value: Page pointer
	unordered_map<string, Page*> map;

	// Decide which slot for temp file.
	int slotId = 0;

	LRU* lru;

};


#endif



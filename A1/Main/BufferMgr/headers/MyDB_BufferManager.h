
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

	// get a temporary/anonymous page, like getPage (), except that this one is pinned
	MyDB_PageHandle getPinnedPage ();

	// un-pin the pinned page
	void unpin (MyDB_PageHandle unpinMe);


	// If there is no available space for uffer manager to add new.
	// LRU needs to pop the tail node and write it back to disk.
	char* evict();

	// If the page already exists in LRU, it should be updated in LRU.
	// Index the node according to existed page and put it back of LRU.
	void update(Page* page);

	//  Insert a new node into head of LRU
	void insert(Page* page);

	void readFromDisk(Page *page);

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

	// checke the page whether exists in buffer
	// key: tableName(i th)
	// value: Page pointer
	unordered_map<string, Page*> map;

	// the temp file index
	int slotId = 0;

	LRU* lru;

};


#endif



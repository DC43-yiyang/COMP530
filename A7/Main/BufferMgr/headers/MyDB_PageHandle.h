

/****************************************************
** COPYRIGHT 2016, Chris Jermaine, Rice University **
**                                                 **
** The MyDB Database System, COMP 530              **
** Note that this file contains SOLUTION CODE for  **
** A1.  You should not be looking at this file     **
** unless you have completed A1!                   **
****************************************************/


#ifndef PAGE_HANDLE_H
#define PAGE_HANDLE_H

#include <memory>
#include "MyDB_Page.h"
#include "MyDB_Table.h"
#include <string>

// page handles are basically smart pointers
using namespace std;
class MyDB_PageHandleBase;
typedef shared_ptr <MyDB_PageHandleBase> MyDB_PageHandle;

class MyDB_PageHandleBase {

public:

	// access the raw bytes in this page
	void *getBytes () {
		return page->getBytes (page);
	}

	// let the page know that we have written to the bytes.  Must always
	// be called once the page's bytes have been written.  If this is not
	// called, then the page will never be marked as dirty, and the page
	// will never be written to disk. 
	void wroteBytes () {
		page->wroteBytes ();
	}

	// There are no more references to the handle when this is called...
	// this should decrmeent a reference count to the number of handles
	// to the particular page that it references.  If the number of 
	// references to a pinned page goes down to zero, then the page should
	// become unpinned.  
	~MyDB_PageHandleBase () {
		page->decRefCount (page);
	}

	// sets up the page...
	MyDB_PageHandleBase (MyDB_PagePtr useMe) {
		page = useMe;
		page->incRefCount ();
	}

private:

	friend class MyDB_PageReaderWriter;

	// get the buffer manager
	MyDB_BufferManager &getParent () {
		return page->getParent ();
	}

	friend class CheckLRU;
	friend class MyDB_BufferManager;
	MyDB_PagePtr page;
};

#endif


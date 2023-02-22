

/****************************************************
** COPYRIGHT 2016, Chris Jermaine, Rice University **
**                                                 **
** The MyDB Database System, COMP 530              **
** Note that this file contains SOLUTION CODE for  **
** A2.  You should not be looking at this file     **
** unless you have completed A2!                   **
****************************************************/


#ifndef PAGE_RW_H
#define PAGE_RW_H

#include <memory>
#include "MyDB_PageType.h"
#include "MyDB_RecordIterator.h"
#include "MyDB_RecordIteratorAlt.h"
#include "MyDB_TableReaderWriter.h"

using namespace std;
class MyDB_PageReaderWriter;
typedef shared_ptr <MyDB_PageReaderWriter> MyDB_PageReaderWriterPtr;

class MyDB_PageReaderWriter {

public:

	// constructor for a page in the same file as the parent
	MyDB_PageReaderWriter (MyDB_TableReaderWriter &parent, int whichPage);

	// constructor for a page that can be pinned, if desired
	MyDB_PageReaderWriter (bool pinned, MyDB_TableReaderWriter &parent, int whichPage);

	// constructor for an anonymous page
	MyDB_PageReaderWriter (MyDB_BufferManager &parent);

	// constructor for an anonymous page that can be pinned, if desired
	MyDB_PageReaderWriter (bool pinned, MyDB_BufferManager &parent);

	// empties out the contents of this page, so that it has no records in it
	// the type of the page is set to MyDB_PageType :: RegularPage
	void clear ();	

	// return an itrator over this page... each time returnVal->next () is
	// called, the resulting record will be placed into the record pointed to
	// by iterateIntoMe
	MyDB_RecordIteratorPtr getIterator (MyDB_RecordPtr iterateIntoMe);

	// gets an instance of an alternate iterator over the page... this is an
	// iterator that has the alternate getCurrent ()/advance () interface
	MyDB_RecordIteratorAltPtr getIteratorAlt ();

	// gets an instance of an alternatie iterator over a list of pages
	friend MyDB_RecordIteratorAltPtr getIteratorAlt (vector <MyDB_PageReaderWriter> &forUs);

	// appends a record to this page... return false is the append fails because
	// there is not enough space on the page; otherwise, return true
	bool append (MyDB_RecordPtr appendMe);

	// appends a record to this page... return a pointer to the location of where
	// the record is written if there is enough space on the page; otherwise, return
	// a nullptr
	void *appendAndReturnLocation (MyDB_RecordPtr appendMe);

	// gets the type of this page... this is just a value from an ennumeration
	// that is stored within the page
	MyDB_PageType getType ();

	// sets the type of the page
	void setType (MyDB_PageType toMe);
	
	// sorts the contents of the page... the boolean lambda that is sent into
	// this function must check to see if the contents of the record pointed to
	// by lhs are less than the contens of the record pointed to by rhs... typically,
	// this lambda would have been created via a call to buildRecordComparator
	MyDB_PageReaderWriterPtr sort (function <bool ()> comparator, MyDB_RecordPtr lhs,  MyDB_RecordPtr rhs);

	// like the above, except that the sorting is done in place, on the page
	void sortInPlace (function <bool ()> comparator, MyDB_RecordPtr lhs,  MyDB_RecordPtr rhs);

	// returns the page size
	size_t getPageSize ();

	// returns the actual bytes
	void *getBytes ();

private:

	// this is the page that we are messing with
	MyDB_PageHandle myPage;	
	
	// this is our buffer manager
	size_t pageSize;
};

// gets an instance of an alternatie iterator over a list of pages
MyDB_RecordIteratorAltPtr getIteratorAlt (vector <MyDB_PageReaderWriter> &forUs);

#endif

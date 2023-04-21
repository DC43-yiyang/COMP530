

/****************************************************
** COPYRIGHT 2016, Chris Jermaine, Rice University **
**                                                 **
** The MyDB Database System, COMP 530              **
** Note that this file contains SOLUTION CODE for  **
** A2.  You should not be looking at this file     **
** unless you have completed A2!                   **
****************************************************/


#ifndef PAGE_RW_C
#define PAGE_RW_C

#include <algorithm>
#include "MyDB_PageReaderWriter.h"
#include "MyDB_PageRecIterator.h"
#include "MyDB_PageRecIteratorAlt.h"
#include "MyDB_PageListIteratorAlt.h"
#include "RecordComparator.h"

#define PAGE_TYPE *((MyDB_PageType *) ((char *) myPage->getBytes ()))
#define NUM_BYTES_USED *((size_t *) (((char *) myPage->getBytes ()) + sizeof (size_t)))
#define NUM_BYTES_LEFT (pageSize - NUM_BYTES_USED)

MyDB_PageReaderWriter :: MyDB_PageReaderWriter () {
	myPage = nullptr;
	pageSize = 0;
}

MyDB_PageReaderWriter :: MyDB_PageReaderWriter (MyDB_TableReaderWriter &parent, int whichPage) {

	// get the actual page
	myPage = parent.getBufferMgr ()->getPage (parent.getTable (), whichPage);
	pageSize = parent.getBufferMgr ()->getPageSize ();
}

MyDB_PageReaderWriter :: MyDB_PageReaderWriter (bool pinned, MyDB_TableReaderWriter &parent, int whichPage) {

	// get the actual page
	if (pinned) {
		myPage = parent.getBufferMgr ()->getPinnedPage (parent.getTable (), whichPage);
	} else {
		myPage = parent.getBufferMgr ()->getPage (parent.getTable (), whichPage);
	}
	pageSize = parent.getBufferMgr ()->getPageSize ();
}

MyDB_PageReaderWriter :: MyDB_PageReaderWriter (MyDB_BufferManager &parent) {
	myPage = parent.getPage ();	
	pageSize = parent.getPageSize ();
	clear ();
}

MyDB_PageReaderWriter :: MyDB_PageReaderWriter (bool pinned, MyDB_BufferManager &parent) {

	if (pinned) {
		myPage = parent.getPinnedPage ();
	} else {
		myPage = parent.getPage ();	
	}
	pageSize = parent.getPageSize ();
	clear ();
}

void MyDB_PageReaderWriter :: clear () {
	NUM_BYTES_USED = 2 * sizeof (size_t);
	PAGE_TYPE = MyDB_PageType :: RegularPage;
	myPage->wroteBytes ();	
}

MyDB_PageType MyDB_PageReaderWriter :: getType () {
	return PAGE_TYPE;
}

MyDB_RecordIteratorAltPtr getIteratorAlt (vector <MyDB_PageReaderWriter> &forUs) {
	return make_shared <MyDB_PageListIteratorAlt> (forUs);
}

MyDB_RecordIteratorPtr MyDB_PageReaderWriter :: getIterator (MyDB_RecordPtr iterateIntoMe) {
	return make_shared <MyDB_PageRecIterator> (myPage, iterateIntoMe);
}

MyDB_RecordIteratorAltPtr MyDB_PageReaderWriter :: getIteratorAlt () {
	return make_shared <MyDB_PageRecIteratorAlt> (myPage);
}

void MyDB_PageReaderWriter :: setType (MyDB_PageType toMe) {
	PAGE_TYPE = toMe;
	myPage->wroteBytes ();	
}

void *MyDB_PageReaderWriter :: appendAndReturnLocation (MyDB_RecordPtr appendMe) {
	void *recLocation = NUM_BYTES_USED + (char *)  myPage->getBytes ();
	if (append (appendMe))
		return recLocation;
	else
		return nullptr;
}

bool MyDB_PageReaderWriter :: append (MyDB_RecordPtr appendMe) {
	
	size_t recSize = appendMe->getBinarySize ();
	if (recSize > NUM_BYTES_LEFT)
		return false;

	// write at the end
	void *address = myPage->getBytes ();
	appendMe->toBinary (NUM_BYTES_USED + (char *) address);
	NUM_BYTES_USED += recSize;
	myPage->wroteBytes ();
	return true;
}

void MyDB_PageReaderWriter :: 
	sortInPlace (function <bool ()> comparator, MyDB_RecordPtr lhs,  MyDB_RecordPtr rhs) {

	void *temp = malloc (pageSize);
	memcpy (temp, myPage->getBytes (), pageSize);

	// first, read in the positions of all of the records
	vector <void *> positions;
	
	// this basically iterates through all of the records on the page
	int bytesConsumed = sizeof (size_t) * 2;
	while (bytesConsumed != NUM_BYTES_USED) {
		void *pos = bytesConsumed + (char *) temp;
		positions.push_back (pos);
		void *nextPos = lhs->fromBinary (pos);
		bytesConsumed += ((char *) nextPos) - ((char *) pos);
	}

	// and now we sort the vector of positions, using the record contents to build a comparator
	RecordComparator myComparator (comparator, lhs, rhs);
	std::stable_sort (positions.begin (), positions.end (), myComparator);

	// and write the guys back
	NUM_BYTES_USED = 2 * sizeof (size_t);
	myPage->wroteBytes ();	
	for (void *pos : positions) {
		lhs->fromBinary (pos);
		append (lhs);
	}

	free (temp);
}

MyDB_PageReaderWriterPtr MyDB_PageReaderWriter :: 
	sort (function <bool ()> comparator, MyDB_RecordPtr lhs,  MyDB_RecordPtr rhs) {

	// first, read in the positions of all of the records
	vector <void *> positions;
	
	// this basically iterates through all of the records on the page
	int bytesConsumed = sizeof (size_t) * 2;
	while (bytesConsumed != NUM_BYTES_USED) {
		void *pos = bytesConsumed + (char *) myPage->getBytes ();
		positions.push_back (pos);
		void *nextPos = lhs->fromBinary (pos);
		bytesConsumed += ((char *) nextPos) - ((char *) pos);
	}

	// and now we sort the vector of positions, using the record contents to build a comparator
	RecordComparator myComparator (comparator, lhs, rhs);
	std::stable_sort (positions.begin (), positions.end (), myComparator);

	// and now create the page to return
	MyDB_PageReaderWriterPtr returnVal = make_shared <MyDB_PageReaderWriter> (myPage->getParent ());
	returnVal->clear ();
	
	// loop through all of the sorted records and write them out
	for (void *pos : positions) {
		lhs->fromBinary (pos);
		returnVal->append (lhs);
	}

	return returnVal;
}

size_t MyDB_PageReaderWriter :: getPageSize () {
	return pageSize;
}

void *MyDB_PageReaderWriter :: getBytes () {
	return myPage->getBytes ();
}

#endif

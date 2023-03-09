

/****************************************************
** COPYRIGHT 2016, Chris Jermaine, Rice University **
**                                                 **
** The MyDB Database System, COMP 530              **
** Note that this file contains SOLUTION CODE for  **
** A2.  You should not be looking at this file     **
** unless you have completed A2!                   **
****************************************************/


#ifndef PAGE_REC_ITER_C
#define PAGE_REC_ITER_C

#include "MyDB_PageRecIterator.h"
#include "MyDB_PageType.h"

#define NUM_BYTES_USED *((size_t *) (((char *) myPage->getBytes ()) + sizeof (size_t)))

void MyDB_PageRecIterator :: getNext () {
	void *pos = bytesConsumed + (char *) myPage->getBytes ();
 	void *nextPos = myRec->fromBinary (pos);
	bytesConsumed += ((char *) nextPos) - ((char *) pos);	
}

void *MyDB_PageRecIterator :: getCurrentPointer () {
	return bytesConsumed + (char *) myPage->getBytes ();
}

bool MyDB_PageRecIterator :: hasNext () {
	return bytesConsumed != NUM_BYTES_USED;
}

MyDB_PageRecIterator :: MyDB_PageRecIterator (MyDB_PageHandle myPageIn, MyDB_RecordPtr myRecIn) {
	bytesConsumed = sizeof (size_t) * 2;
	myPage = myPageIn;
	myRec = myRecIn;
}

MyDB_PageRecIterator :: ~MyDB_PageRecIterator () {}

#endif

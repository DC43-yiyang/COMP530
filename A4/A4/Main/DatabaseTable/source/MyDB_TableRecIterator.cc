

/****************************************************
** COPYRIGHT 2016, Chris Jermaine, Rice University **
**                                                 **
** The MyDB Database System, COMP 530              **
** Note that this file contains SOLUTION CODE for  **
** A2.  You should not be looking at this file     **
** unless you have completed A2!                   **
****************************************************/


#ifndef TABLE_REC_ITER_C
#define TABLE_REC_ITER_C

#include "MyDB_PageReaderWriter.h"
#include "MyDB_TableRecIterator.h"

void MyDB_TableRecIterator :: getNext () {
	myIter->getNext ();
}

void *MyDB_TableRecIterator :: getCurrentPointer () {
	return myIter->getCurrentPointer ();
}

bool MyDB_TableRecIterator :: hasNext () {
	if (myParent[curPage].getType () == MyDB_PageType :: RegularPage && myIter->hasNext ())
		return true;

	if (curPage == myTable->lastPage ())
		return false;

	curPage++;
	myIter = myParent[curPage].getIterator (myRec);
	return hasNext ();
}

MyDB_TableRecIterator :: MyDB_TableRecIterator (MyDB_TableReaderWriter &myParent, MyDB_TablePtr myTableIn,
	MyDB_RecordPtr myRecIn) : myParent (myParent) {
	myTable = myTableIn;
	myRec = myRecIn;
	curPage = 0;
	myIter = myParent[curPage].getIterator (myRec);		
}

MyDB_TableRecIterator :: ~MyDB_TableRecIterator () {}

#endif

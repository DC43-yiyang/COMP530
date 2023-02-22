
/****************************************************
** COPYRIGHT 2016, Chris Jermaine, Rice University **
**                                                 **
** The MyDB Database System, COMP 530              **
** Note that this file contains SOLUTION CODE for  **
** A2.  You should not be looking at this file     **
** unless you have completed A2!                   **
****************************************************/

#ifndef RUN_QITER_ALT_C
#define RUN_QITER_ALT_C

#include "IteratorComparator.h"
#include "MyDB_PageListIteratorAlt.h"
#include "MyDB_PageRecIteratorAlt.h"
#include "MyDB_RunQueueIteratorAlt.h"

using namespace std;

void MyDB_RunQueueIteratorAlt :: getCurrent (MyDB_RecordPtr intoMe) {
	auto myIter = pq.top ();
	myIter->getCurrent (intoMe);
}

MyDB_RunQueueIteratorAlt :: MyDB_RunQueueIteratorAlt (function <bool ()> comparator, MyDB_RecordPtr lhs, MyDB_RecordPtr rhs) : 
	pq (IteratorComparator (comparator, lhs, rhs)) {
	firstTime = true;
}
	
bool MyDB_RunQueueIteratorAlt :: advance () {

	if (firstTime) {
		firstTime = false;
		return true;
	}

	if (pq.size () == 0)
		return false;

	// remove from the q
	auto myIter = pq.top ();
	pq.pop ();
	
	// re-insert
	if (myIter->advance ()) 
		pq.push (myIter);

	return (pq.size () != 0);
}

void *MyDB_RunQueueIteratorAlt :: getCurrentPointer () {
	return pq.top ()->getCurrentPointer ();
}

MyDB_RunQueueIteratorAlt :: ~MyDB_RunQueueIteratorAlt () {}

#endif

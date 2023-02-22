

/****************************************************
** COPYRIGHT 2016, Chris Jermaine, Rice University **
**                                                 **
** The MyDB Database System, COMP 530              **
** Note that this file contains SOLUTION CODE for  **
** A2.  You should not be looking at this file     **
** unless you have completed A2!                   **
****************************************************/


#ifndef TABLE_RW_C
#define TABLE_RW_C

#include <fstream>
#include <limits>
#include <queue>
#include "MyDB_PageReaderWriter.h"
#include "MyDB_TableRecIterator.h"
#include "MyDB_TableRecIteratorAlt.h"
#include "MyDB_TableReaderWriter.h"
#include <set>
#include <vector>
#include "Sorting.h"

using namespace std;

MyDB_TableReaderWriter :: MyDB_TableReaderWriter (MyDB_TablePtr forMeIn, MyDB_BufferManagerPtr myBufferIn) {
	forMe = forMeIn;
	myBuffer = myBufferIn;

	if (forMe->lastPage () == -1) {
		forMe->setLastPage (0);
		lastPage = make_shared <MyDB_PageReaderWriter> (*this, forMe->lastPage ());
		lastPage->clear ();
	} else {
		lastPage = make_shared <MyDB_PageReaderWriter> (*this, forMe->lastPage ());	
	}
}

MyDB_BufferManagerPtr MyDB_TableReaderWriter :: getBufferMgr () {
	return myBuffer;
}

MyDB_TablePtr MyDB_TableReaderWriter :: getTable () {
	return forMe;
}

int MyDB_TableReaderWriter :: getNumPages () {
	return forMe->lastPage () + 1;
}

MyDB_PageReaderWriter MyDB_TableReaderWriter :: getPinned (size_t i) {
	return MyDB_PageReaderWriter (true, *this, i);
}

MyDB_PageReaderWriter MyDB_TableReaderWriter :: operator [] (size_t i) {
	
	// see if we are going off of the end of the file... if so, then clear those pages
	while (i > forMe->lastPage ()) {
		forMe->setLastPage (forMe->lastPage () + 1);
		lastPage = make_shared <MyDB_PageReaderWriter> (*this, forMe->lastPage ());
		lastPage->clear ();	
	}

	// now get the page
	MyDB_PageReaderWriter arrayAccessBuffer (*this, i);
	return arrayAccessBuffer;
}

MyDB_RecordPtr MyDB_TableReaderWriter :: getEmptyRecord () {

	// use the schema to produce an empty record
	return make_shared <MyDB_Record> (forMe->getSchema ());
}

MyDB_PageReaderWriter MyDB_TableReaderWriter :: last () {
	MyDB_PageReaderWriter arrayAccessBuffer (*this, forMe->lastPage ());
	return arrayAccessBuffer;
}

void MyDB_TableReaderWriter :: append (MyDB_RecordPtr appendMe) {

	// try to append the record on the current page...
	if (!lastPage->append (appendMe)) {

		// if we cannot, then get a new last page and append
		forMe->setLastPage (forMe->lastPage () + 1);
		lastPage = make_shared <MyDB_PageReaderWriter> (*this, forMe->lastPage ());
		lastPage->clear ();
		lastPage->append (appendMe);
	}
}

pair <vector <size_t>, size_t>  MyDB_TableReaderWriter :: loadFromTextFile (string fName) {

	// empty out the database file
	forMe->setLastPage (0);
	lastPage = make_shared <MyDB_PageReaderWriter> (*this, forMe->lastPage ());
	lastPage->clear ();

	// try to open the file
	string line;
	ifstream myfile (fName);

	MyDB_RecordPtr tempRec = getEmptyRecord ();

	// this data structure is used for apporoximate counting of the number of distinct
	// values of each attribute
	vector <pair <set <size_t>, int>> allHashes;
	for (int i = 0; i < tempRec->getSchema ()->getAtts ().size (); i++) {
		set <size_t> temp1;
		allHashes.push_back (make_pair (temp1, 1));
	}

	// if we opened it, read the contents
	size_t counter = 0;
	if (myfile.is_open()) {

		// loop through all of the lines
		while (getline (myfile,line)) {
			tempRec->fromString (line);		
			counter++;
			// hash all of the attributes... this is used for counting
			int i = 0;
			for (auto &a : allHashes) {

				// insert the hash
				size_t hash = tempRec->getAtt (i)->hash ();
				i++;
				if (hash % a.second != 0)
					continue;

				a.first.insert (hash);

				// if we have too many items, compact them
				#define MAX_SIZE 1000
				if (a.first.size () > MAX_SIZE) {
					a.second *= 2;
					set <size_t> newSet;
					for (auto &num : a.first) {
						if (num % a.second == 0)
							newSet.insert (num);
					}
					a.first = newSet;	
				}
			}
			append (tempRec);
		}
		myfile.close ();
	}
	cout << "Loaded " << counter << " records.\n";

	// finally, compute the vector of estimates
	vector <size_t> returnVal;
	for (auto &a : allHashes) {
		size_t est = ((size_t) a.first.size ()) * a.second;
		returnVal.push_back (est);
	}
	return make_pair (returnVal, counter);
}

MyDB_RecordIteratorPtr MyDB_TableReaderWriter :: getIterator (MyDB_RecordPtr iterateIntoMe) {
	return make_shared <MyDB_TableRecIterator> (*this, forMe, iterateIntoMe);
}

MyDB_RecordIteratorAltPtr MyDB_TableReaderWriter :: getIteratorAlt () {
	return make_shared <MyDB_TableRecIteratorAlt> (*this, forMe);
}

MyDB_RecordIteratorAltPtr MyDB_TableReaderWriter :: getIteratorAlt (int lowPage, int highPage) {
	return make_shared <MyDB_TableRecIteratorAlt> (*this, forMe, lowPage, highPage);
}

void MyDB_TableReaderWriter :: writeIntoTextFile (string fName) {
	
	// open up the output file
	ofstream output;
	output.open (fName);

	// get an empty record
	MyDB_RecordPtr tempRec = getEmptyRecord ();;		

	// and write out all of the records
	MyDB_RecordIteratorPtr myIter = getIterator (tempRec);
	while (myIter->hasNext ()) {
		myIter->getNext ();
                output << tempRec << "\n";
	}
	output.close ();
}

#endif


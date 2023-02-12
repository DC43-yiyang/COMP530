

/****************************************************
** COPYRIGHT 2016, Chris Jermaine, Rice University **
**                                                 **
** The MyDB Database System, COMP 530              **
** Note that this file contains SOLUTION CODE for  **
** A2.  You should not be looking at this file     **
** unless you have completed A2!                   **
****************************************************/


#ifndef TABLE_RW_H
#define TABLE_RW_H

#include <memory>
#include "MyDB_BufferManager.h"
#include "MyDB_Record.h"
#include "MyDB_RecordIterator.h"
#include "MyDB_RecordIteratorAlt.h"
#include "MyDB_Table.h"
#include <set>
#include <vector>

// create a smart pointer for the table reader writer
using namespace std;
class MyDB_PageReaderWriter;
class MyDB_TableReaderWriter;
typedef shared_ptr <MyDB_TableReaderWriter> MyDB_TableReaderWriterPtr;

class MyDB_TableReaderWriter {

public:

	// create a table reader/writer
	MyDB_TableReaderWriter (MyDB_TablePtr forMe, MyDB_BufferManagerPtr myBuffer);

	// gets an empty record from this table
	MyDB_RecordPtr getEmptyRecord ();

	// append a record to the table
	virtual void append (MyDB_RecordPtr appendMe);

	// return an itrator over this table... each time returnVal->next () is
	// called, the resulting record will be placed into the record pointed to
	// by iterateIntoMe
	MyDB_RecordIteratorPtr getIterator (MyDB_RecordPtr iterateIntoMe);

        // gets an instance of an alternate iterator over the table... this is an
        // iterator that has the alternate getCurrent ()/advance () interface
        MyDB_RecordIteratorAltPtr getIteratorAlt ();

	// gets an instance of an alternate iterator over the page; this iterator
	// works on a range of pages in the file, and iterates from lowPage through
	// highPage inclusive
	MyDB_RecordIteratorAltPtr getIteratorAlt (int lowPage, int highPage);

	// load a text file into this table... this returns a pair where the first
	// entry is a list of (approximate) distinct value counts for each of the
	// attributes in the table, and the second entry is the number of tuples that
	// have been loaded into the table
	pair <vector <size_t>, size_t> loadFromTextFile (string fromMe);

	// dump the contents of this table into a text file
	void writeIntoTextFile (string toMe);

	// access the i^th page in this file
	MyDB_PageReaderWriter operator [] (size_t i);

	// access the i^th page in this file... getting a pinned version of the page
	MyDB_PageReaderWriter getPinned (size_t i);

	// access the last page in the file
	MyDB_PageReaderWriter last ();

	// get the number of pages in the file
	int getNumPages ();

	// get access to the buffer manager	
	MyDB_BufferManagerPtr getBufferMgr ();

	// gets the physical file for this guy
	string getFileName ();
	
	// gets the table object for this guy
	MyDB_TablePtr getTable ();

private:

	friend class MyDB_PageReaderWriter;
	friend class MyDB_BPlusTreeReaderWriter;
	MyDB_TablePtr forMe;
	MyDB_BufferManagerPtr myBuffer;
	shared_ptr <MyDB_PageReaderWriter> lastPage;
	
};

#endif


#ifndef REC_ITER_H
#define REC_ITER_H

#include <memory>
using namespace std;

// This pure virtual class is used to iterate through the records in a page or file
// Instances of this class will be created via calls to MyDB_PageReaderWriter.getIterator ()
// or MyDB_FileReaderWriter.getIterator ().
//
class MyDB_RecordIterator;
typedef shared_ptr <MyDB_RecordIterator> MyDB_RecordIteratorPtr;

// DO NOT MODIFY!

class MyDB_RecordIterator {

public:

	// put the contents of the next record in the file/page into the iterator record
	// this should be called BEFORE the iterator record is first examined
	virtual void getNext () = 0;

	// return true iff there is another record in the file/page
	virtual bool hasNext () = 0;

	// destructor and contructor
	MyDB_RecordIterator () {};
	virtual ~MyDB_RecordIterator () {};

};

#endif


#ifndef REC_ITER_ALT_H
#define REC_ITER_ALT_H

#include <memory>
#include "MyDB_Record.h"
using namespace std;

// This pure virtual class is used to iterate through the records in a page or file
// Instances of this class will be created via calls to MyDB_PageReaderWriter.getIteratorAlt ()
// or MyDB_FileReaderWriter.getIteratorAlt ().  
//
class MyDB_RecordIteratorAlt;
typedef shared_ptr <MyDB_RecordIteratorAlt> MyDB_RecordIteratorAltPtr;

class MyDB_RecordIteratorAlt {

public:

	// load the current record into the parameter
	virtual void getCurrent (MyDB_RecordPtr intoMe) = 0;

        // after a call to advance (), a call to getCurrentPointer () will get the address
        // of the record.  At a later time, it is then possible to reconstitute the record
        // by calling MyDB_Record.fromBinary (obtainedPointer)... ASSUMING that the page that
	// the record is located on has not been swapped out
        virtual void *getCurrentPointer () = 0;
	
	// advance to the next record... returns true if there is a next record, and 
	// false if there are no more records to iterate over.  Not that this cannot
	// be called until after getCurrent () has been called
	virtual bool advance () = 0;

	// destructor and contructor
	MyDB_RecordIteratorAlt () {};
	virtual ~MyDB_RecordIteratorAlt () {};

};

#endif

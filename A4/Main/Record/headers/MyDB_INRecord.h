
#ifndef IN_RECORD_H
#define IN_RECORD_H

#include "MyDB_Record.h"

// create a smart pointer for records
class MyDB_INRecord;
typedef shared_ptr <MyDB_INRecord> MyDB_INRecordPtr;

// this class stores a special type of record that lives in the internal node of a B+-Tree...
class MyDB_INRecord : public MyDB_Record  {

public:

	MyDB_INRecord (MyDB_AttValPtr myAtt) : MyDB_Record (nullptr) {
		values.push_back (myAtt);
		values.push_back (make_shared <MyDB_IntAttVal> ());	
		bufferOld = true;
	}

	int getPtr () {
		return values[1]->toInt ();
	}

	void setPtr (int fromMe) {
		values[1]->fromInt (fromMe);
		bufferOld = true;
	}

	void setKey (MyDB_AttValPtr toMe) {
		values[0] = toMe;
		bufferOld = true;
	}

	MyDB_AttValPtr getKey () {
		return values[0];
	}
};

#endif

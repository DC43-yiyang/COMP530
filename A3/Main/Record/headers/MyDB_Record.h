
#ifndef RECORD_H
#define RECORD_H

#include <functional>
#include "MyDB_AttVal.h"
#include "MyDB_Schema.h"
#include <memory>
#include <string>
#include <vector>

using namespace std;

// create a smart pointer for records
class MyDB_Record;
typedef shared_ptr <MyDB_Record> MyDB_RecordPtr;

// a lambda function over the record... computes an attribute value
typedef function <MyDB_AttValPtr ()> func;

class MyDB_Record {

public:

	// constructs a record that can hold data for the given schema
	MyDB_Record (MyDB_SchemaPtr mySchema);

	// read the record from the text string
	void fromText (string fromMe);

	// get the number of bytes required to store the record as a binary string
	size_t getBinarySize ();

	// makes it so that this record is a composite of the two input records
	void buildFrom (MyDB_RecordPtr left, MyDB_RecordPtr right);

	// write the contents of the record in binary to the specifed location; return
	// the location of the next byte that was not writen... thus, the code:
	//
	// void *loc = startLoc;
	// for (int i = 0; i < 10; i++) {
	// 	loc = myRec.toBinary (loc);
	// }
	// 	
	// would write 10 copies of the record to the location pointed to by startLoc
	void *toBinary (void *toHere);

	// this method should be called whenever we want to write the record to a page, but
	// the record may have had its contents changed since the last call to fromBinary
	// or fromString (for example, if an attribute value was changed).  Why?  For speed,
	// the record buffers its serailzed binary representation.  If the data changes, this
	// needs to be recomputed, and calling this method lets the record know this.  After
	// calling recordContentHasChanged (), toBinary () is more expensive.
	void recordContentHasChanged ();

	// reverse of the above; the following code would read the next 10 records
	//
	// void *loc = startLoc;
	// for (int i = 0; i < 10; i++) {
	// 	loc = myRec.fromBinary (loc);
	// }
	// 	
	void *fromBinary (void *startPos);

	// parse the contents of this record from the given string
	void fromString (string fromMe);

	// write the record to an output string
	friend std::ostream& operator<<(std::ostream& os, const MyDB_Record printMe);
	friend std::ostream& operator<<(std::ostream& os, const MyDB_RecordPtr printMe);
	
	// destructor
	~MyDB_Record ();

	// builds a function over this record, as specified in the given string...
	// the string is an infix notation specification of a computation over
	// the record.  For example:
	//
	// myRecord.compileComputation ("+ ( [firstAtt], / ([secAtt], [secAtt])) ");
	// 
	// returns a function equivalent to the SQL: firstAtt + (secAtt / secAtt)
	//
	// myRecord.compileComputation ("< (+ (double [34.45], + ( [firstAtt], / ([secAtt], [secAtt]))), int [20]) ");
	//
	// returns a function equivalent to the SQL: (34.45 + firstAtt + (secAtt / secAtt)) < 20
	//
	// myRecord.compileComputation ("!= ( string [here is one], string[here is another] ) ");
	//
	// returns a function equivalent to the SQL: 'here is one' <> 'here is another'
	//
	// note that when the resulting lambda expression is called, it is called
	// by computing the function over the CURRENT version of the record.  So one
	// can create the function over the record, then load the record, and invoke
	// the lambda---this will evaluate the function over the current contents of the
	// record.  Then, if one loads different contents into the record and executes
	// the lambda again, the function can return a different value the second time
	// around.  This is useful, because one can use a single record to iterate through
	// the entire file, computing the function after each new record is loaded, without
	// recompiling the function.
	//
	func compileComputation (string fromMe);

	// builds a function that returns true if lhs < rhs; the comparison is done by running whatever computation is 
	// encoded by the string "computation" on both lhs and rhs, and then compariing the results obtained using this
	// computation over both.  If the result from lhs is < the result from rhs, then the function returned from
	// buildRecordComparator returns a true; otherwise, it returns a false
	//
	// Note that the encoding of the computation in the string "computation" is exactly the same as the encoding
	// used by the method compileComputation above
	friend function <bool ()> buildRecordComparator (MyDB_RecordPtr lhs,  MyDB_RecordPtr rhs, string computation);

	// access the schema
	MyDB_SchemaPtr &getSchema ();

	// access a particular attribute
	MyDB_AttValPtr &getAtt (int whichAtt);

private:

	// for fast reading from a page; the contents of the record are simply copied into this buffer
	char *buffer;

	// the amount of space allocated for the buffer
	size_t allocatedSize;

	// the amount of data in the record buffer
	size_t recSize;

	// helper function for the compilation
	pair <func, MyDB_AttTypePtr> compileHelper (char * &vals);

	// helper function for the compilation
	char *findsymbol (char val, char *input);
	
	// these functions are all used to build up computations over the record
	pair <func, MyDB_AttTypePtr> fromData (string attName);
	pair <func, MyDB_AttTypePtr> plus (pair <func, MyDB_AttTypePtr> lhs, pair <func, MyDB_AttTypePtr> rhs);
	pair <func, MyDB_AttTypePtr> minus (pair <func, MyDB_AttTypePtr> lhs, pair <func, MyDB_AttTypePtr> rhs);
	pair <func, MyDB_AttTypePtr> times (pair <func, MyDB_AttTypePtr> lhs, pair <func, MyDB_AttTypePtr> rhs);
	pair <func, MyDB_AttTypePtr> divide (pair <func, MyDB_AttTypePtr> lhs, pair <func, MyDB_AttTypePtr> rhs);
	pair <func, MyDB_AttTypePtr> gt (pair <func, MyDB_AttTypePtr> lhs, pair <func, MyDB_AttTypePtr> rhs);
	pair <func, MyDB_AttTypePtr> lt (pair <func, MyDB_AttTypePtr> lhs, pair <func, MyDB_AttTypePtr> rhs);
	pair <func, MyDB_AttTypePtr> eq (pair <func, MyDB_AttTypePtr> lhs, pair <func, MyDB_AttTypePtr> rhs);
	pair <func, MyDB_AttTypePtr> neq (pair <func, MyDB_AttTypePtr> lhs, pair <func, MyDB_AttTypePtr> rhs);
	pair <func, MyDB_AttTypePtr> andd (pair <func, MyDB_AttTypePtr> lhs, pair <func, MyDB_AttTypePtr> rhs);
	pair <func, MyDB_AttTypePtr> orr (pair <func, MyDB_AttTypePtr> lhs, pair <func, MyDB_AttTypePtr> rhs);
	pair <func, MyDB_AttTypePtr> unaryMinus (pair <func, MyDB_AttTypePtr> lhs);
	pair <func, MyDB_AttTypePtr> nott (pair <func, MyDB_AttTypePtr> lhs);

	// write the current attribute values into the buffer
	void writeAttsToBuffer ();

	// true when the set of attributes don't match the attribute buffer
	bool bufferOld;

	// this is a subtype
	friend class MyDB_INRecord;

	MyDB_SchemaPtr mySchema;
	vector <MyDB_AttValPtr> values;	
	vector <MyDB_AttValPtr> scratch;

};

#endif

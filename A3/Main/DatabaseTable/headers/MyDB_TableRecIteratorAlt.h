

/****************************************************
** COPYRIGHT 2016, Chris Jermaine, Rice University **
**                                                 **
** The MyDB Database System, COMP 530              **
** Note that this file contains SOLUTION CODE for  **
** A2.  You should not be looking at this file     **
** unless you have completed A2!                   **
****************************************************/


#ifndef TABLE_REC_ITER_ALT_H
#define TABLE_REC_ITER_ALT_H

#include "MyDB_RecordIteratorAlt.h"
#include "MyDB_Record.h"
#include "MyDB_TableReaderWriter.h"
#include "MyDB_Table.h"

class MyDB_TableRecIteratorAlt : public MyDB_RecordIteratorAlt {

public:

        // load the current record into the parameter
        void getCurrent (MyDB_RecordPtr intoMe) override;

        // after a call to advance (), a call to getCurrentPointer () will get the address
        // of the record.  At a later time, it is then possible to reconstitute the record
        // by calling MyDB_Record.fromBinary (obtainedPointer)... ASSUMING that the page that
        // the record is located on has not been swapped out
        void *getCurrentPointer ();

        // advance to the next record... returns true if there is a next record, and
        // false if there are no more records to iterate over.  Not that this cannot
        // be called until after getCurrent () has been called
        bool advance () override;

	// destructor and contructor
	MyDB_TableRecIteratorAlt (MyDB_TableReaderWriter &myParent, MyDB_TablePtr myTableIn);
	~MyDB_TableRecIteratorAlt ();
	MyDB_TableRecIteratorAlt (MyDB_TableReaderWriter &myParent, MyDB_TablePtr myTableIn, int lowPage, int highPage);

private:

	MyDB_RecordIteratorAltPtr myIter;
	int curPage;
	int highPage;	
	MyDB_TableReaderWriter &myParent;
	MyDB_TablePtr myTable;
};

#endif

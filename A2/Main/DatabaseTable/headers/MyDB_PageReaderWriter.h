
#ifndef PAGE_RW_H
#define PAGE_RW_H

#include "MyDB_PageType.h"
#include "MyDB_TableReaderWriter.h"
#include "MyDB_RecordIterator.h"


class MyDB_PageReaderWriter {
public:

	// ANY OTHER METHODS YOU WANT HERE
	// constructor

	// MyDB_PageReaderWriter(MyDB_BufferManagerPtr myBufferIn, MyDB_PageHandle pageIn);
	MyDB_PageReaderWriter(size_t pageSize, MyDB_PageHandle pageIn);

	~MyDB_PageReaderWriter ();

	// empties out the contents of this page, so that it has no records in it
	// the type of the page is set to MyDB_PageType :: RegularPage
	void clear ();	

	// return an itrator over this page... each time returnVal->next () is
	// called, the resulting record will be placed into the record pointed to
	// by iterateIntoMe
	MyDB_RecordIteratorPtr getIterator (MyDB_RecordPtr iterateIntoMe);

	// appends a record to this page... return false is the append fails because
	// there is not enough space on the page; otherwise, return true
	bool append (MyDB_RecordPtr appendMe);

	// gets the type of this page... this is just a value from an ennumeration
	// that is stored within the page
	MyDB_PageType getType ();

	// sets the type of the page
	void setType (MyDB_PageType toMe);

	
	// nyytodo needs a deconstructor
	
private:
	// ANYTHING ELSE YOU WANT HERE
	MyDB_PageHandle pageHandle;
	
	// size_t index;
	// the page's index, and we can also combine the index and tablePtr to get the handle
	// the reanson why I do not treat the handle as the input parament is in funciton "append"
	// at first I do not know the specific location that recordPtr to insert
	// if each time I call the getPage funcition it's a big waste
	
	// It is a little bit awkward. two ways are same... above all, I am just talking...
	
	size_t pageSize;
	// we can get the size by call bufferManager
};

#endif

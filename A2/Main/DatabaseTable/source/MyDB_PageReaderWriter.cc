
#ifndef PAGE_RW_C
#define PAGE_RW_C

#include "MyDB_PageReaderWriter.h"
#include "MyDB_PageRecIterator.h"

void MyDB_PageReaderWriter :: clear () {
	// empties out the contents of this page, so that it has no records in it
	// the type of the page is set to MyDB_PageType :: RegularPage
	size_t offset = sizeof(size_t) + sizeof(MyDB_PageType);
	(* ((size_t *)(((char *)(this->pageHandle->getBytes())) + sizeof(MyDB_PageType)))) = offset;
	this->pageHandle->wroteBytes();
}

MyDB_PageType MyDB_PageReaderWriter :: getType () {
	return (*(MyDB_PageType *) (this -> pageHandle -> getBytes()));
}

MyDB_RecordIteratorPtr MyDB_PageReaderWriter :: getIterator (MyDB_RecordPtr recordPtr) {
	MyDB_RecordIteratorPtr returnIter;
	returnIter = make_shared<MyDB_PageRecIterator>(this->pageHandle,recordPtr);
	return returnIter;
}

void MyDB_PageReaderWriter :: setType (MyDB_PageType pageType) {
	(*(MyDB_PageType *) (this -> pageHandle -> getBytes())) = pageType; 
	pageHandle -> wroteBytes();
}

bool MyDB_PageReaderWriter :: append (MyDB_RecordPtr record) {
	// cout << "here is page append" << endl;
	// First get the size of the record
	size_t recordSize = record -> getBinarySize();
	size_t usedSize = (*((size_t *)( ((char *)(this -> pageHandle -> getBytes())) + sizeof(MyDB_PageType))));
	// Calculate remainning page size
	size_t remainSize = this -> pageSize - usedSize;

	// cout << "record size is: " << recordSize << endl;
	// cout << "remainSize is: " << remainSize << endl;

	char *myBytes = ((char *) this -> pageHandle -> getBytes());

	// If there is not enough space on the page, return false
	if (remainSize < recordSize) {
		return false;
	}
	else {
		// Append the record at the end of the page space
		record -> toBinary (usedSize + myBytes);
		// Augment the usedSize
		(*((size_t *)( ((char *)(this -> pageHandle -> getBytes())) + sizeof(MyDB_PageType)))) += recordSize;
		// cout << "after + record size, the used size is: " << usedSize << endl;
		this -> pageHandle -> wroteBytes();
	}
	
	return true;
}

MyDB_PageReaderWriter :: MyDB_PageReaderWriter(size_t pageSizeIn,  MyDB_PageHandle pageIn) {
	this->pageHandle = pageIn; 
	this->pageSize = pageSizeIn;
}

MyDB_PageReaderWriter :: ~MyDB_PageReaderWriter () { }
#endif

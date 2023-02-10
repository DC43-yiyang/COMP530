
#ifndef PAGE_RW_C
#define PAGE_RW_C

#include "MyDB_PageReaderWriter.h"
#include "MyDB_PageRecIterator.h"

void MyDB_PageReaderWriter :: clear () {
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
	size_t recordSize = record -> getBinarySize();
	size_t usedSize = (*((size_t *)( ((char *)(this -> pageHandle -> getBytes())) + sizeof(MyDB_PageType))));
	size_t remainSize = this -> pageSize - usedSize;

	char *myBytes = ((char *) this -> pageHandle -> getBytes());

	if (remainSize < recordSize) {
		return false;
	}
	else {
		record -> toBinary (usedSize + myBytes);
		(*((size_t *)( ((char *)(this -> pageHandle -> getBytes())) + sizeof(MyDB_PageType)))) += recordSize;
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

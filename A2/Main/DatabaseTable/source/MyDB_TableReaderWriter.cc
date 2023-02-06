
#ifndef TABLE_RW_C
#define TABLE_RW_C

#include <fstream>
#include "MyDB_PageReaderWriter.h"
#include "MyDB_TableReaderWriter.h"

using namespace std;

MyDB_TableReaderWriter :: MyDB_TableReaderWriter (MyDB_TablePtr, MyDB_BufferManagerPtr) {
}

MyDB_PageReaderWriter MyDB_TableReaderWriter :: operator [] (size_t) {
	MyDB_PageReaderWriter temp;
	return temp;	
}

MyDB_RecordPtr MyDB_TableReaderWriter :: getEmptyRecord () {
	return nullptr;
}

MyDB_PageReaderWriter MyDB_TableReaderWriter :: last () {
	MyDB_PageReaderWriter temp;
	return temp;	
}


void MyDB_TableReaderWriter :: append (MyDB_RecordPtr) {
}

void MyDB_TableReaderWriter :: loadFromTextFile (string) {
}

MyDB_RecordIteratorPtr MyDB_TableReaderWriter :: getIterator (MyDB_RecordPtr) {
	return nullptr;
}

void MyDB_TableReaderWriter :: writeIntoTextFile (string) {
}

#endif


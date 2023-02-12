
#ifndef SORT_C
#define SORT_C

#include "MyDB_PageReaderWriter.h"
#include "MyDB_TableRecIterator.h"
#include "MyDB_TableRecIteratorAlt.h"
#include "MyDB_TableReaderWriter.h"
#include "Sorting.h"

using namespace std;

void mergeIntoFile (MyDB_TableReaderWriter &, vector <MyDB_RecordIteratorAltPtr> &, function <bool ()>, MyDB_RecordPtr, MyDB_RecordPtr) {}

vector <MyDB_PageReaderWriter> mergeIntoList (MyDB_BufferManagerPtr, MyDB_RecordIteratorAltPtr, MyDB_RecordIteratorAltPtr, function <bool ()>, 
	MyDB_RecordPtr, MyDB_RecordPtr) {return vector <MyDB_PageReaderWriter> (); } 
	
void sort (int, MyDB_TableReaderWriter &, MyDB_TableReaderWriter &, function <bool ()>, MyDB_RecordPtr, MyDB_RecordPtr) {} 

#endif

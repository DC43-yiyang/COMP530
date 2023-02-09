
#ifndef TABLE_RW_C
#define TABLE_RW_C

#include <fstream>
#include <sstream>
#include "MyDB_PageReaderWriter.h"
#include "MyDB_TableReaderWriter.h"
#include "MyDB_TableRecIterator.h"

using namespace std;

MyDB_TableReaderWriter :: MyDB_TableReaderWriter (MyDB_TablePtr table, MyDB_BufferManagerPtr buffer) {
	this->table = table;
	this->buffer = buffer;
	this->pageSize = buffer->getPageSize();
	
	if(table->lastPage() == -1){
		// there is nothing written to table
		table->setLastPage(0);
		(*this)[0].clear();
	}
}

MyDB_PageReaderWriter MyDB_TableReaderWriter :: operator [] (size_t i) {
	MyDB_PageHandle myPageH = this->buffer->getPage(this->table,i);
	while (i > this->table->lastPage()) 
	{
		//the index i is out of the bounds, I need to loop through the page to up to the input index
		//reference:https://piazza.com/class/lcp293khb9x406/post/102
		this->table->setLastPage(this->table->lastPage() + 1);
		//after loop you need to clear and return the last one
		shared_ptr<MyDB_PageReaderWriter> temp = make_shared<MyDB_PageReaderWriter> (this->buffer, myPageH);  
		temp->clear();
	}
	
	this->pageMap[i] = make_shared<MyDB_PageReaderWriter> (this->buffer, myPageH);
	return *this->pageMap[i];	
}

MyDB_RecordPtr MyDB_TableReaderWriter :: getEmptyRecord () {
	MyDB_RecordPtr emptyRecordPtr = make_shared<MyDB_Record>(this->table->getSchema());
	return emptyRecordPtr;
}

MyDB_PageReaderWriter MyDB_TableReaderWriter :: last () {
	return (*this)[this->table->lastPage()];
}


void MyDB_TableReaderWriter :: append (MyDB_RecordPtr appendMe) {
	while (!((*this)[this->table->lastPage()]).append(appendMe))
	{
		// when add the "appendMe" to the index "lastPage()" failed
		int last = this->table->lastPage() + 1;
		this->table->setLastPage(last);
		// update the index
		(*this)[last].clear();
		// clear it and try add again
		
	}
}

MyDB_RecordIteratorPtr MyDB_TableReaderWriter :: getIterator (MyDB_RecordPtr recordPtr) {
	MyDB_RecordIteratorPtr temp;
	temp = make_shared<MyDB_TableRecIterator>(*this, this->table, recordPtr);
	return temp;
}

void MyDB_TableReaderWriter :: loadFromTextFile (string fileName) {
	this->table->setLastPage(0);

	MyDB_RecordPtr tmpRecord = getEmptyRecord();
	ifstream fileStream;
	fileStream.open(fileName);
	string line;
	while (getline(fileStream, line)) {
		tmpRecord -> fromString(line);
		append(tmpRecord);
	}
	fileStream.close();
}

void MyDB_TableReaderWriter :: writeIntoTextFile (string fileName) {
	ofstream fileStream;
	fileStream.open(fileName);
	MyDB_RecordPtr record = getEmptyRecord();
	MyDB_RecordIteratorPtr recIterator = getIterator(record);

	while (recIterator->hasNext())
	{
		recIterator->getNext(); 
	}
	fileStream.close();
}

#endif


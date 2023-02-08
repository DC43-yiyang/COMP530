#ifndef PAGE_REC_ITER_H
#define PAGE_REC_ITER_H


#include "MyDB_RecordIterator.h"
#include "MyDB_Table.h"
#include "MyDB_Record.h"
#include "MyDB_TableReaderWriter.h"



class MyDB_TableRecIterator: public MyDB_RecordIterator {

public:
    MyDB_TableRecIterator(MyDB_TableReaderWriter &parent, MyDB_TablePtr myTable, MyDB_RecordPtr myRecord);

    bool hasNext(); // whether there is a next page in table 

    void getNext();

private:
    MyDB_TableReaderWriter &myParent;
    MyDB_TablePtr table;
    MyDB_RecordPtr record;
    MyDB_RecordIteratorPtr pageRecIterator;
    long pageIndex;
};


#endif
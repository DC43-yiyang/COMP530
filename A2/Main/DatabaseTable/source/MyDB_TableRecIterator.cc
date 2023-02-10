#ifndef TABLE_REC_ITER_C
#define TABLE_REC_ITER_C
#include "MyDB_TableRecIterator.h"
#include "MyDB_PageReaderWriter.h"

bool MyDB_TableRecIterator :: hasNext(){
    if (this->pageRecIterator->hasNext()) {
        return true;
    }

    while (this->pageIndex < this->table->lastPage()) {
        this->pageIndex += 1;
        this->pageRecIterator = this->myParent[this->pageIndex].getIterator(record);
        // if there exists one page that has next record, return true 
        if (this->pageRecIterator->hasNext()) {
            return true;
        }
    }

    return false;
    
}

// for the table iterator, we need to use the iterator of the page 
// reference:https://piazza.com/class/lcp293khb9x406/post/103
void MyDB_TableRecIterator :: getNext(){
    if(this->hasNext()){
        this->pageRecIterator->getNext();
    }
}

MyDB_TableRecIterator :: MyDB_TableRecIterator(MyDB_TableReaderWriter& myParent, MyDB_TablePtr myTable, MyDB_RecordPtr myRecord) : myParent(myParent) {
    // this->tableRWptr = tableReaderWriter;
    this->table = myTable;
    this->record = myRecord;
    this->pageIndex = 0;
    this->pageRecIterator = this->myParent[this->pageIndex].getIterator(myRecord);
    // operator [] to get the index's MyDB_PageReaderWriter
}

#endif
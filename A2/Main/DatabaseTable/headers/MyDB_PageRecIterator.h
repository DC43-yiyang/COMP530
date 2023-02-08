#ifndef PAGE_REC_ITER_H
#define PAGE_REC_ITER_H

#include "MyDB_RecordIterator.h"
#include "MyDB_Record.h"
#include "MyDB_PageHandle.h"
// #include "MyDB_PageReaderWriter.h"


class MyDB_PageRecIterator;
typedef shared_ptr <MyDB_PageRecIterator> MyDB_PageRecIteratorPtr;

class MyDB_PageRecIterator : public MyDB_RecordIterator {

public:

    void getNext();

    bool hasNext();

    MyDB_PageRecIterator(MyDB_PageHandle pageHandle, MyDB_RecordPtr recordPtr);
    ~MyDB_PageRecIterator();

private:
    MyDB_PageHandle pageHandle;
    MyDB_RecordPtr recordPtr;
    size_t offset;
    // Note that we always do size by the number of pages, as opposed to the number of bytes.
    // the way to implement we can referened https://piazza.com/class/lcp293khb9x406/post/103 
    
};








#endif
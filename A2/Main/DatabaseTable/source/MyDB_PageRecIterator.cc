#ifndef TABLE_REC_ITER_H
#define TABLE_REC_ITER_H
#include "MyDB_PageRecIterator.h"
// #include "MyDB_PageReaderWriter.h"
#include "MyDB_PageType.h"
#include <iostream>
#include <string>

void MyDB_PageRecIterator :: getNext(){
    if (hasNext() == true) {
        char *currentPos = offset + ((char *)(this -> pageHandle -> getBytes()));
        char *nextPos = (char *) this -> recordPtr -> fromBinary (currentPos);
        // The new offset equals to
        offset = (nextPos - currentPos) + offset;
    }
}


bool MyDB_PageRecIterator :: hasNext(){
    if ((*((size_t *) ( ((char *)(this -> pageHandle -> getBytes())) + sizeof(MyDB_PageType)))) > offset) {
        return true;
    }
    else {
        return false;
    }
}

MyDB_PageRecIterator :: MyDB_PageRecIterator(MyDB_PageHandle pageHandle, MyDB_RecordPtr recordPtr) {
    this->pageHandle = pageHandle;
    this->recordPtr = recordPtr;
    // Offset is the initial used bytes in a page
    this -> offset = sizeof(MyDB_PageType) + sizeof(size_t);
}

MyDB_PageRecIterator :: ~MyDB_PageRecIterator(){}


#endif
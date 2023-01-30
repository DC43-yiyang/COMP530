#ifndef A1_MYDB_PAGE_H
#define A1_MYDB_PAGE_H

#include <iostream>


#include "MyDB_Page.h"
#include "MyDB_BufferManager.h"
#include "MyDB_Table.h"
#include "LRU.h"
class Node;

class Page {
private:
    char* bufferAddr = nullptr;
    // that's the place that address of page's bytes 
    int slotId = 0;
    // use for the anoanymous page
    int handleNum = 0;
    // record the page-handle num
    bool isAnonymous;
    bool dirty = false;
    bool isPin = false;

    pair<MyDB_TablePtr, long> PageId;
    Node* node;

public:
    // Initialize anonymous page
    Page();

    // Initialize non-anonymous page
    Page(MyDB_TablePtr whichTable, long i);


    int getSlotId() const;

    void setSlotId(int slotId);


    Node *getNode() const;

    void setNode(Node *node);


    bool isIsPin() const;

    void setPin();

    void undoPin();


    bool isIsAnonymous() const;


    void setBufferAddr(char *bufferAddr);

    char *getBufferAddr() const;


    void setDirty(bool dirty);

    bool isDirty() const;



    int getHandleNum() const;

    void addHandleNum();

    void deHandleNum();


    pair<MyDB_TablePtr, long> getPageId();


};


#endif 

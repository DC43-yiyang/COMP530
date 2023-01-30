#ifndef A1_LRU_H
#define A1_LRU_H

#include "MyDB_Page.h"
#include "MyDB_BufferManager.h"
#include "MyDB_Table.h"

class Page;

class Node {
public:
    Page *getPage() const;

    Node *getPre() const;

    void setPre(Node *pre);

    void setNext(Node *next);

    Node *getNext() const;

    Node(Page* page);



private:
    Page *page;
    Node *pre;
    Node *next;

};


class LRU {
public:
    LRU(int fullSize);

    Node* evict();

    void insert(Node* node);

    void update(Node* node);

    bool isEmpty();

    Node* finalEvict();


private:
    int fullSize;
    int curSize = 0;
    Node* tail = nullptr;
    Node* head = nullptr;

};




#endif //A1_LRU_H

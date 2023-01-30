#ifndef PAGE_C
#define PAGE_C
#include "MyDB_Page.h"

char *Page::getBufferAddr() const {
    return bufferAddr;
}

bool Page::isDirty() const {
    return dirty;
}

void Page::setBufferAddr(char *bufferAddr) {
    this->bufferAddr = bufferAddr;
}

void Page::setDirty(bool dirty) {
    this->dirty = dirty;
}

void Page::setPin() {
    this->isPin = true;
}

void Page::undoPin() {
    this->isPin = false;

}

void Page::addHandleNum() {
    this->handleNum++;
}

void Page::deHandleNum() {
    this->handleNum--;
}

Page::Page(MyDB_TablePtr whichTable, long i) {
    PageId.first = whichTable;
    PageId.second = i;
    isAnonymous = false;
}

Page::Page() {
    this->isAnonymous= true;
}

Node *Page::getNode() const {
    return node;
}

void Page::setNode(Node *node) {
    this->node = node;
}

bool Page::isIsPin() const {
    return isPin;
}

bool Page::isIsAnonymous() const {
    return isAnonymous;
}

int Page::getHandleNum() const {
    return handleNum;
}

int Page::getSlotId() const {
    return slotId;
}

void Page::setSlotId(int slotId) {
    this->slotId = slotId;
}

pair<MyDB_TablePtr, long> Page::getPageId() {
    return PageId;
}


#endif


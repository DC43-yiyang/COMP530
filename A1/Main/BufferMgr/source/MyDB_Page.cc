//
// Created by Wangjj on 2019-01-24.
//


#include "MyDB_Page.h"

char *Page::getBufferAddr() const {
    return bufferAddr;
}

bool Page::isDirty() const {
    return dirty;
}

void Page::setBufferAddr(char *bufferAddr) {
    Page::bufferAddr = bufferAddr;
}

void Page::setDirty(bool dirty) {
    Page::dirty = dirty;
}

void Page::setPin() {
    Page::isPin = true;
}

void Page::undoPin() {
    Page::isPin = false;

}

void Page::addHandleNum() {
    Page::handleNum++;
}

void Page::deHandleNum() {
    Page::handleNum--;
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
    Page::node = node;
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
    Page::slotId = slotId;
}

pair<MyDB_TablePtr, long> Page::getPageId() {
    return PageId;
}





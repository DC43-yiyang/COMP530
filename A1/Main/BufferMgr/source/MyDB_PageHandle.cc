
#ifndef PAGE_HANDLE_C
#define PAGE_HANDLE_C

#include <memory>
#include "MyDB_PageHandle.h"

void *MyDB_PageHandleBase :: getBytes () {
    if(page->getBufferAddr() != nullptr) {
		// std::cout << "###########NYY########### this page is already in the LRU list FILE: " __FILE__ "， LINE: " << __LINE__ << " ，Function: " << __FUNCTION__ << std::endl;

		// which indicates that this page is already in the LRU list
		// we need to update the position of node in LRU, we can assume that node is the most recent node
	    bufferManager->update(page);
        return page->getBufferAddr();
	} 
	else {

		// If there is no available space for new page, buffer needs
		// to evict one page and assign this space to new page.
		if(bufferManager->space.empty()) {
			// std::cout << "###########NYY########### space is empty FILE:" __FILE__ "， LINE: " << __LINE__ << " ，Function: " << __FUNCTION__ << std::endl;

			// LRU need to evict the first one
			char* availableAddr = bufferManager->evict();
			page->setBufferAddr(availableAddr);
			bufferManager->insert(page);
		} 
		else {
			// std::cout << "###########NYY########### space available  FILE:" __FILE__ "， LINE: " << __LINE__ << " ，Function: " << __FUNCTION__ << std::endl;
			// If buffer's space is still available, assign the available
			// return the last space element to add the new page.
		    char* addr = bufferManager->space.back();
			// remove the last element is the space
		    bufferManager->space.pop_back();
		    page->setBufferAddr(addr);
			bufferManager->insert(page);
		}
        return page->getBufferAddr();
	}
}

void MyDB_PageHandleBase :: wroteBytes () {
	page->setDirty(true);
}

MyDB_PageHandleBase :: ~MyDB_PageHandleBase () {
	page->deHandleNum();
	if(page->getHandleNum() == 0){
		page->undoPin();
		if(page->isIsAnonymous()){
			this->bufferManager->anonymousSpace.push_back(page->getSlotId());
		}
	}
}

Page *MyDB_PageHandleBase::getPage() const {
	return page;
}

MyDB_PageHandleBase::MyDB_PageHandleBase(Page *page, MyDB_BufferManager *buffer) : page(page), bufferManager(buffer) {
	this->page = page;
	this->bufferManager = buffer;
}

#endif


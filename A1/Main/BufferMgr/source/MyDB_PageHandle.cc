
#ifndef PAGE_HANDLE_C
#define PAGE_HANDLE_C

#include <memory>
#include "MyDB_PageHandle.h"

void *MyDB_PageHandleBase :: getBytes () {
	// if page's buffer address is not nullptr, the page exists in buffer.
    if(page->getBufferAddr() != nullptr) {

		// Update the position of node in LRU
	    buffer->update(page);
        return page->getBufferAddr();
	} 
	else {

		// If there is no available space for new page, buffer needs
		// to evict one page and assign this space to new page.
		if(buffer->space.empty()) {
			// LRU need to evict the first one
			char* availableAddr = buffer->evict(page);
			page->setBufferAddr(availableAddr);
			buffer->insert(page);
		} 
		else {

			// If buffer's space is still available, assign the available
			// space to new page.
		    char* addr = buffer->space.back();
		    buffer->space.pop_back();
		    page->setBufferAddr(addr);
			buffer->insert(page);
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
			this->buffer->anonymousSpace.push_back(page->getSlotId());
		}
	}
}

Page *MyDB_PageHandleBase::getPage() const {
	return page;
}

MyDB_PageHandleBase::MyDB_PageHandleBase(Page *page, MyDB_BufferManager *buffer) : page(page), buffer(buffer) {
	this->page = page;
	this->buffer = buffer;
}

#endif


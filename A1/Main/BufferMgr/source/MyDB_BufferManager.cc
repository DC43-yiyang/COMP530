
#ifndef BUFFER_MGR_C
#define BUFFER_MGR_C

#include "MyDB_BufferManager.h"
#include <string>
#include <zconf.h>
#include <iostream>

using namespace std;

MyDB_PageHandle MyDB_BufferManager :: getPage (MyDB_TablePtr whichTable, long i) {
	Page* curPage;
	string str = whichTable->getName();
	string key = str + "_"+ to_string(i);

	// to check whether the page already eists
	if(this->map.find(key) == map.end()) {
		// find it
		curPage = new Page(whichTable, i);
		this->map[key] = curPage;
	}
	// it doesn't exist, so add it to the map
	curPage = map[key];
	curPage->addHandleNum();
	MyDB_PageHandle ph = make_shared<MyDB_PageHandleBase>(curPage, this);

	return ph;
}

MyDB_PageHandle MyDB_BufferManager :: getPage () {
	Page *curPage = new Page();
	if(anonymousSpace.empty()) {
	    curPage->setSlotId(slotId);
		slotId++;
	} else {
		curPage->setSlotId(anonymousSpace.back());
		anonymousSpace.pop_back();
	}
	MyDB_PageHandle ph = make_shared<MyDB_PageHandleBase>(curPage, this);
	curPage->addHandleNum();

	return ph;
}

MyDB_PageHandle MyDB_BufferManager :: getPinnedPage (MyDB_TablePtr whichTable, long i) {
	// Get a non-anonymous page
	MyDB_PageHandle ph = getPage(whichTable, i);

	// Set pin
	ph->getPage()->setPin();

    return ph;
}

MyDB_PageHandle MyDB_BufferManager :: getPinnedPage () {

	// Get an anonymous page
	MyDB_PageHandle ph = getPage();

	// Set Pin
	ph->getPage()->setPin();

	return ph;
}

void MyDB_BufferManager :: unpin (MyDB_PageHandle pageHandle) {
	pageHandle->getPage()->undoPin();
}

MyDB_BufferManager :: MyDB_BufferManager (size_t pageSize, size_t numPage, string tempFile) {
	
	this->lru = new LRU(numPage);
	this->pageSize = pageSize;
	this->numPage = numPage;
	this->tempFile = tempFile;
	this->map = unordered_map<string, Page*> ();

	buffer = (char*) malloc(pageSize * numPage);

	// Split the space into Page size and put them into a vector
	for(int i = 0; i < numPage; i++) {
		space.push_back(addr);
		addr += pageSize;
	}

}

MyDB_BufferManager :: ~MyDB_BufferManager () {

	unordered_map<string, Page*>::iterator iter;

	for(iter=map.begin();iter!=map.end(); iter++){
		if(iter->second->getBufferAddr() == nullptr){
			delete(iter->second->getNode());
			delete(iter->second);
		}else{
			if(iter->second->isDirty()){
				writeToDisk(iter->second);
			}
			delete(iter->second->getNode());
			delete(iter->second);
		}
	}

	delete lru;

	remove(tempFile.c_str());
	free (buffer);
}

char* MyDB_BufferManager::evict() {

	// Evict one page from LRU
	Page* outPage = lru->evict()->getPage();

	// Write back to disk
	writeToDisk(outPage);

	// Get out page's buffer address
	char* addr = outPage->getBufferAddr();

	// Set out page's buffer address to nullptr
	outPage->setBufferAddr(nullptr);

    return addr;
}

void MyDB_BufferManager::update(Page *page) {

	// get the node
	Node* node = page->getNode();

	// during the LRU list to find the node and move it to the head
	lru->update(node); //需要去优化nyytodo
}

void MyDB_BufferManager::insert(Page *page) {
	// Create a new node to insert
	Node *node = new Node(page);

	// Set page's node
	page->setNode(node);

	// insert into LRU
	lru->insert(node);

	readFromDisk(page);


}

void MyDB_BufferManager::writeToDisk(Page* page) {
	if (page->isIsAnonymous()) {
		if (page->isDirty()) {
			int filedescriptor = open(tempFile.c_str(),O_CREAT | O_RDWR | O_SYNC, 0666);
			lseek(filedescriptor, page->getSlotId() * this->pageSize, SEEK_SET);
			write(filedescriptor, page->getBufferAddr(), this->pageSize);
			page->setDirty(false);
			close(filedescriptor);
		}
	} else {
		if (page->isDirty()) {
			int filedescriptor = open(page->getPageId().first->getStorageLoc().c_str(), O_CREAT | O_RDWR | O_SYNC, 0666);
			lseek(filedescriptor, page->getPageId().second * this->pageSize, SEEK_SET);
			write(filedescriptor, page->getBufferAddr(), this->pageSize);
			page->setDirty(false);
			close(filedescriptor);
		}
	}
}

void MyDB_BufferManager::readFromDisk(Page *page) {
	if (page->isIsAnonymous()) {
		int filedescriptor = open(tempFile.c_str(),O_CREAT | O_RDWR | O_SYNC, 0666);
		lseek(filedescriptor, page->getSlotId() * this->pageSize, SEEK_SET);
		read(filedescriptor, page->getBufferAddr(), this->pageSize);
		page->setDirty(false);
		 close(filedescriptor);

	} else {
		int filedescriptor = open(page->getPageId().first->getStorageLoc().c_str(), O_CREAT | O_RDWR | O_SYNC, 0666);
		lseek(filedescriptor, page->getPageId().second * this->pageSize, SEEK_SET);
		read(filedescriptor, page->getBufferAddr(), this->pageSize);
		page->setDirty(false);
		close(filedescriptor);
	}
}

#endif



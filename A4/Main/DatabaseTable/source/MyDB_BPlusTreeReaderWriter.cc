
#ifndef BPLUS_C
#define BPLUS_C

#include "MyDB_INRecord.h"
#include "MyDB_BPlusTreeReaderWriter.h"
#include "MyDB_PageReaderWriter.h"
#include "MyDB_PageListIteratorSelfSortingAlt.h"
#include "RecordComparator.h"

MyDB_BPlusTreeReaderWriter :: MyDB_BPlusTreeReaderWriter (string orderOnAttName, MyDB_TablePtr forMe, 
	MyDB_BufferManagerPtr myBuffer) : MyDB_TableReaderWriter (forMe, myBuffer) {

	// find the ordering attribute
	auto res = forMe->getSchema ()->getAttByName (orderOnAttName);

	// remember information about the ordering attribute
	orderingAttType = res.second;
	whichAttIsOrdering = res.first;

	// and the root location
	rootLocation = getTable ()->getRootLocation ();
}

MyDB_RecordIteratorAltPtr MyDB_BPlusTreeReaderWriter :: getSortedRangeIteratorAlt (MyDB_AttValPtr low, MyDB_AttValPtr high) {
	vector<MyDB_PageReaderWriter> rangePages;
	MyDB_INRecordPtr lowPtr = getINRecord();
	MyDB_INRecordPtr highPtr = getINRecord();
	MyDB_RecordPtr tempPtr = getEmptyRecord();
	lowPtr->setKey(low);
	highPtr->setKey(high);
	function<bool()> lowBound = buildComparator(tempPtr, lowPtr);
    function<bool()> highBound = buildComparator(highPtr, tempPtr);

    MyDB_RecordPtr lhs = getEmptyRecord();
    MyDB_RecordPtr rhs = getEmptyRecord();
	function<bool()> comparator = buildComparator(lhs, rhs);
	
	return make_shared<MyDB_PageListIteratorSelfSortingAlt>(rangePages, lhs, rhs, comparator, tempPtr, lowBound, highBound, true);
}

MyDB_RecordIteratorAltPtr MyDB_BPlusTreeReaderWriter :: getRangeIteratorAlt (MyDB_AttValPtr low, MyDB_AttValPtr high) {
	vector<MyDB_PageReaderWriter> rangePages;
	MyDB_INRecordPtr lowPtr = getINRecord();
	MyDB_INRecordPtr highPtr = getINRecord();
	MyDB_RecordPtr tempPtr = getEmptyRecord();

	lowPtr->setKey(low);
	highPtr->setKey(high);
	function<bool()> lowBound = buildComparator(tempPtr, lowPtr);
    function<bool()> highBound = buildComparator(highPtr, tempPtr);

    MyDB_RecordPtr lhs = getEmptyRecord();
    MyDB_RecordPtr rhs = getEmptyRecord();
	function<bool()> comparator = buildComparator(lhs, rhs);
	
	return make_shared<MyDB_PageListIteratorSelfSortingAlt>(rangePages, lhs, rhs, comparator, tempPtr, lowBound, highBound, false);
}


bool MyDB_BPlusTreeReaderWriter :: discoverPages (int whichPage, vector <MyDB_PageReaderWriter> &list, MyDB_AttValPtr low, MyDB_AttValPtr high) {
	// Any pages found are then returned to the caller by putting them in the parameter list
	// whichPage is the identity of a page in the file 
	queue<int> pageQueue;
	pageQueue.push(whichPage);
	while (!pageQueue.empty())
	{
		// judge whether there is element in the queue
		// the BPlusTreeReaderWriter is the subclass of the TableReaderWriter so I can directly use the [index] to get the index^th page(pageReadWriter)
		MyDB_PageReaderWriter curPage = (*this)[pageQueue.front()];
		if (curPage.getType() == MyDB_PageType::RegularPage)
		{
			// leaf nodes
			// what we need to know is if the page is regular, just put it in the list and return true.
			list.push_back(curPage);
		}
		pageQueue.pop();
		// after judging the leaf node or not you need to pop it, when we need to add it see below
		// there is about the internal nodes
		MyDB_INRecordPtr lowPtr = getINRecord();
		MyDB_INRecordPtr highPtr = getINRecord();
		MyDB_INRecordPtr tempPtr = getINRecord();
		// initialize the three ptr

		MyDB_RecordIteratorAltPtr tempIter = curPage.getIteratorAlt();

		lowPtr->setKey(low);
		highPtr->setKey(high);

		while (tempIter->advance())
		{
			tempIter->getCurrent(tempPtr);
			if(buildComparator(tempPtr,lowPtr)){
				continue;
			}
			pageQueue.push(tempPtr->getPtr());

			if (buildComparator(highPtr,tempPtr))
			{
				// over boundary
				break;
			}

		}
	}
	return !list.empty();
	// the return boolean value indicating whether the page pointed to by whichPage was at the leaf level
}

void MyDB_BPlusTreeReaderWriter :: append (MyDB_RecordPtr appendMe) {
}

MyDB_RecordPtr MyDB_BPlusTreeReaderWriter :: split (MyDB_PageReaderWriter splitRW, MyDB_RecordPtr splitRPtr) {
	return nullptr;
}

MyDB_RecordPtr MyDB_BPlusTreeReaderWriter :: append (int whichPage, MyDB_RecordPtr appendMe) {
	return nullptr;
}

MyDB_INRecordPtr MyDB_BPlusTreeReaderWriter :: getINRecord () {
	return make_shared <MyDB_INRecord> (orderingAttType->createAttMax ());
}

void MyDB_BPlusTreeReaderWriter :: printTree () {
}

MyDB_AttValPtr MyDB_BPlusTreeReaderWriter :: getKey (MyDB_RecordPtr fromMe) {

	// in this case, got an IN record
	if (fromMe->getSchema () == nullptr) 
		return fromMe->getAtt (0)->getCopy ();

	// in this case, got a data record
	else 
		return fromMe->getAtt (whichAttIsOrdering)->getCopy ();
}

function <bool ()>  MyDB_BPlusTreeReaderWriter :: buildComparator (MyDB_RecordPtr lhs, MyDB_RecordPtr rhs) {

	MyDB_AttValPtr lhAtt, rhAtt;

	// in this case, the LHS is an IN record
	if (lhs->getSchema () == nullptr) {
		lhAtt = lhs->getAtt (0);	

	// here, it is a regular data record
	} else {
		lhAtt = lhs->getAtt (whichAttIsOrdering);
	}

	// in this case, the LHS is an IN record
	if (rhs->getSchema () == nullptr) {
		rhAtt = rhs->getAtt (0);	

	// here, it is a regular data record
	} else {
		rhAtt = rhs->getAtt (whichAttIsOrdering);
	}
	
	// now, build the comparison lambda and return
	if (orderingAttType->promotableToInt ()) {
		return [lhAtt, rhAtt] {return lhAtt->toInt () < rhAtt->toInt ();};
	} else if (orderingAttType->promotableToDouble ()) {
		return [lhAtt, rhAtt] {return lhAtt->toDouble () < rhAtt->toDouble ();};
	} else if (orderingAttType->promotableToString ()) {
		return [lhAtt, rhAtt] {return lhAtt->toString () < rhAtt->toString ();};
	} else {
		cout << "This is bad... cannot do anything with the >.\n";
		exit (1);
	}
}


#endif

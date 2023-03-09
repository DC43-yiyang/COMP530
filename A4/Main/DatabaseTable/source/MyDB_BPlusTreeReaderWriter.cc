
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
	this->rootLocation = getTable()->getRootLocation();
}

MyDB_RecordIteratorAltPtr MyDB_BPlusTreeReaderWriter :: getSortedRangeIteratorAlt (MyDB_AttValPtr low, MyDB_AttValPtr high) {
	vector<MyDB_PageReaderWriter> rangePages;
	this->discoverPages(this->rootLocation, rangePages, low, high);

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
	this->discoverPages(this->rootLocation, rangePages, low, high);
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
// Another solution to discoverPage
// bool MyDB_BPlusTreeReaderWriter :: discoverPages (int whichPage, vector <MyDB_PageReaderWriter> &list, MyDB_AttValPtr low, MyDB_AttValPtr high) {
// 	MyDB_PageReaderWriter curPage = (*this)[whichPage];
// 	// Return it if it is a leaf page
// 	if (curPage.getType() == MyDB_PageType::RegularPage) {
// 		list.push_back(curPage);
// 		return true;
// 	}
// 	// iterate the rest to find the page
// 	MyDB_RecordIteratorAltPtr iter = curPage.getIteratorAlt();

// 	//build comparator
// 	MyDB_INRecordPtr lhs = getINRecord();
// 	MyDB_INRecordPtr rhs = getINRecord();
// 	lhs -> setKey(low);
// 	rhs -> setKey(high);

// 	MyDB_INRecordPtr curRec = getINRecord();

// 	bool isLeaf = false;
// 	while(iter->advance()) {
// 		iter->getCurrent(curRec);
// 		// curRec should >= lhs
// 		if (!buildComparator(curRec, lhs)()) {
// 			int curId = curRec -> getPtr();
// 			if (isLeaf) {
// 				list.push_back((*this)[curId]);
// 			}
// 			else {
// 				isLeaf = discoverPages(curId, list, low, high);
// 			}
// 		}
// 		if (buildComparator(rhs, curRec)()) {
// 			return false;
// 		} 
// 	}
// 	return false;	
// }

// nyytodo
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
		pageQueue.pop();

		if (curPage.getType() == MyDB_PageType::RegularPage)
		{
			// leaf nodes
			// what we need to know is if the page is regular, just put it in the list and return true.
			list.push_back(curPage);
		}
		// after judging the leaf node or not you need to pop it, when we need to add it see below
		// there is about the internal nodes
		if (curPage.getType() != RegularPage)
		{
			MyDB_INRecordPtr lowPtr = getINRecord();
			MyDB_INRecordPtr highPtr = getINRecord();
			MyDB_INRecordPtr tempPtr = getINRecord();
			// initialize the three ptr

			MyDB_RecordIteratorAltPtr tempIter = curPage.getIteratorAlt();

			lowPtr->setKey(low);
			function<bool()> lowBound = buildComparator(tempPtr, lowPtr);
			highPtr->setKey(high);
			function<bool()> highBound = buildComparator(highPtr, tempPtr);

			while (tempIter->advance())
			{
				tempIter->getCurrent(tempPtr);
				if (lowBound())
				{
					continue;
				}
				pageQueue.push(tempPtr->getPtr());

				if (highBound())
				{
					// over boundary
					break;
				}
			}
		}
	}
	return !list.empty();
	// the return boolean value indicating whether the page pointed to by whichPage was at the leaf level
}

void MyDB_BPlusTreeReaderWriter :: append (MyDB_RecordPtr appendMe) {
	// first we need to check if there is a valid B+ tree. Follow the A4 pdf says.
	// the empty B+ tree is an empty tree with only one node, which is the root node. so we got
	if(getNumPages() <= 1)
	{
		MyDB_PageReaderWriter root = (*this)[0];
		this->rootLocation = 0;
		getTable()->setRootLocation(0);
		// complete the root
		root.clear();
		root.setType(DirectoryPage);

		// initial the internal node first, and add it to the root
		MyDB_INRecordPtr newInNode = getINRecord();
		newInNode->setPtr(1);
		getTable()->setLastPage(1);
		root.append(newInNode);

		// leaf node
		MyDB_PageReaderWriter newLeaf = (*this)[1];
		newLeaf.setType(RegularPage);
		newLeaf.clear();
		newLeaf.append(appendMe);
	}
	else
	{
		// valid B+ tree
		MyDB_RecordPtr newRecInRoot = append(this->rootLocation, appendMe);
		if(nullptr != newRecInRoot){
			// add successfully
			int newRootLoc = getTable()->lastPage()+1;
			MyDB_PageReaderWriter newRoot = (*this)[newRootLoc];
			getTable()->setLastPage(newRootLoc);

			// insert the record points to the corresponding page
			newRoot.clear();
			newRoot.setType(DirectoryPage);
			newRoot.append(newRecInRoot);

			MyDB_INRecordPtr newRec = getINRecord();
			newRec->setPtr(this->rootLocation);
			newRoot.append(newRec);

            this->rootLocation = getTable()->lastPage();
            getTable()->setRootLocation(this->rootLocation);
		}
	}
}

// splits the given page (plus the record andMe) around the median.  A MyDB_INRecordPtr is returned that
// points to the record holding the (key, ptr) pair pointing to the new page.  Note that the new page
// always holds the lower 1/2 of the records on the page; the upper 1/2 remains in the original page

MyDB_RecordPtr MyDB_BPlusTreeReaderWriter :: split (MyDB_PageReaderWriter splitRW, MyDB_RecordPtr splitRPtr) {
	int newRootLoc = getTable()->lastPage() + 1;
    MyDB_PageReaderWriter newPage = (*this)[newRootLoc];
    MyDB_PageType currentType = splitRW.getType();
    MyDB_RecordPtr lhs, rhs;
    function<bool ()> comparator, insertionComp;
	// using different record for different nodes
    if (currentType == RegularPage) {
        lhs = getEmptyRecord();
        rhs = getEmptyRecord();
    } else if (currentType == DirectoryPage) {
        lhs = getINRecord();
        rhs = getINRecord();
    }

    MyDB_INRecordPtr newInterRec = getINRecord();
    newPage.setType(currentType);
    comparator = buildComparator(lhs, rhs);

    if (currentType == RegularPage) {
        splitRW.sortInPlace(comparator, lhs, rhs);
    }
    bool flag = false;
    vector<MyDB_RecordPtr> listToSplit;
    MyDB_RecordIteratorAltPtr iter = splitRW.getIteratorAlt();
	
    while (iter->advance()) {
        MyDB_RecordPtr temp;
        if (currentType == RegularPage) {
            temp = getEmptyRecord();
        } else if (currentType == DirectoryPage) {
            temp = getINRecord();
        }
        iter->getCurrent(temp);
        if (!flag) {
            insertionComp = buildComparator(splitRPtr, temp);
            if (insertionComp()) {
                flag = true;
                listToSplit.push_back(splitRPtr);
            }
        }
        listToSplit.push_back(temp);
    }

    if (!flag) {
        listToSplit.push_back(splitRPtr);
    }
    splitRW.clear();
    if (currentType == DirectoryPage) {
        splitRW.setType(DirectoryPage);
    }
    int i;
    int size = listToSplit.size();
    int mid = size / 2;
    for (i = 0; i < mid; i++) {
        newPage.append(listToSplit[i]);
    }
    newInterRec->setPtr(getTable()->lastPage());
    newInterRec->setKey(getKey(listToSplit[i - 1]));
    for (; i < size; i++) {
        splitRW.append(listToSplit[i]);
    }
	return newInterRec;
}


MyDB_RecordPtr MyDB_BPlusTreeReaderWriter :: append (int whichPage, MyDB_RecordPtr appendMe) {
	MyDB_PageReaderWriter pageAddMe = (*this)[whichPage];
	// this pageReaderWriter need to add
	if (pageAddMe.getType() == RegularPage)
	{
		// this page is a leaf page, directly add 
		if(pageAddMe.append(appendMe)){
			return nullptr;
		}
		else{
			// if failed, we need to split the page
			return split(pageAddMe, appendMe);
		}
		
	}
	else{
		// it is an internal node, we need to find the subroot to add
		MyDB_RecordIteratorAltPtr recIter = pageAddMe.getIteratorAlt();

		MyDB_INRecordPtr tempInRec = getINRecord();
		function <bool ()> comparator = buildComparator(appendMe, tempInRec);
		while (recIter->advance())
		{
			recIter->getCurrent(tempInRec);
			// if we find the record that is larger than appendMe record.
			// what we do is to recursively find the final insertion position
			if(comparator()){
				MyDB_RecordPtr recPtr = append(tempInRec->getPtr(), appendMe);
				// recursive
				if (recPtr != nullptr)
				{
					// there is not nullptr, we need a split
					if (pageAddMe.append(recPtr))
					{
						MyDB_INRecordPtr curRec = getINRecord();
						function <bool ()> comparator = buildComparator(recPtr, curRec);
						pageAddMe.sortInPlace(comparator, recPtr, curRec);
						return nullptr;
					}

					 return this->split(pageAddMe, recPtr);
				}
				return nullptr;
				
			}
		}
	}
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

#ifndef SORT_C
#define SORT_C

#include "MyDB_PageReaderWriter.h"
#include "MyDB_TableRecIterator.h"
#include "MyDB_TableRecIteratorAlt.h"
#include "MyDB_TableReaderWriter.h"
#include "Sorting.h"
#include "RecordComparator.h"
#include <memory>
#include <vector>


using namespace std;

class comparePQ {
private:
    RecordComparator* rc;
public:
    comparePQ(function <bool ()> comparator, MyDB_RecordPtr lhs, MyDB_RecordPtr rhs) {
        rc = new RecordComparator(comparator, lhs, rhs);
    }

    bool operator() (MyDB_RecordIteratorAltPtr l, MyDB_RecordIteratorAltPtr r) {
        return !rc->operator()(l->getCurrentPointer(), r->getCurrentPointer());
    }

    void release() {
        delete(rc);
    }
};

void appendToPage(MyDB_BufferManagerPtr parent, MyDB_RecordPtr appendMe, vector <MyDB_PageReaderWriter> &curList) {
    if (curList.empty()) {
        MyDB_PageReaderWriter newPage(*parent);
        curList.push_back(newPage);
    }

    MyDB_PageReaderWriter &curPage = curList.back();
    if (!curPage.append(appendMe)) {
        MyDB_PageReaderWriter nextPage(*parent);
        nextPage.append(appendMe);
        curList.push_back(nextPage);
    }
}


void mergeIntoFile (MyDB_TableReaderWriter &sortIntoMe, vector <MyDB_RecordIteratorAltPtr> &mergeUs, function <bool ()> comparator, MyDB_RecordPtr lhs, MyDB_RecordPtr rhs) {
    comparePQ comp(comparator, lhs, rhs);
    priority_queue<MyDB_RecordIteratorAltPtr, vector<MyDB_RecordIteratorAltPtr>, comparePQ> pq (comp, mergeUs);

    MyDB_RecordIteratorAltPtr tempPtr;
    while (!pq.empty()) {
        tempPtr = pq.top();
        pq.pop();

        tempPtr -> getCurrent(lhs);
        sortIntoMe.append(lhs);

        if (tempPtr->advance()) {
            pq.push(tempPtr);
        }
    }

    comp.release();
}

vector <MyDB_PageReaderWriter> mergeIntoList (MyDB_BufferManagerPtr parent, MyDB_RecordIteratorAltPtr leftIter, MyDB_RecordIteratorAltPtr rightIter, function <bool ()> comparator,
	MyDB_RecordPtr lhs, MyDB_RecordPtr rhs) {

    vector <MyDB_PageReaderWriter> sortedPageList;

    bool leftFlag= leftIter->advance();
    bool rightFlag = rightIter->advance();

    while (!leftFlag && !rightFlag) {
        leftIter->getCurrent(lhs);
        rightIter->getCurrent(rhs);
        if (comparator()) {
            appendToPage(parent, lhs, sortedPageList);
            leftFlag = !leftIter->advance();
        }
        else {
            appendToPage(parent, rhs, sortedPageList);
            rightFlag = !rightIter->advance();
        }
    }
    while (!leftFlag) {
        leftIter->getCurrent(lhs);
        appendToPage(parent, lhs, sortedPageList);
        rightFlag = !leftIter->advance();
    }

    while (!rightFlag) {
        rightIter->getCurrent(rhs);
        appendToPage(parent, rhs, sortedPageList);
        rightFlag = !rightIter->advance();
    }

    return sortedPageList;
}

void sort (int runSize, MyDB_TableReaderWriter &sortMe, MyDB_TableReaderWriter &sortIntoMe, function <bool ()> comparator, MyDB_RecordPtr lhs, MyDB_RecordPtr rhs) {
    MyDB_BufferManagerPtr BufferPtr = sortMe.getBufferMgr();
    vector<MyDB_RecordIteratorAltPtr> mergeUs;

    for (int i = 0; i < sortMe.getNumPages();) {
        vector< vector<MyDB_PageReaderWriter>> mergeHelper;
        vector< MyDB_PageReaderWriter> tempVector;

        // Record the progress
        int j;
        for (j = i; j < sortMe.getNumPages() && j < i + runSize; j++) {
            tempVector.clear();
            tempVector.push_back(*sortMe[j].sort(comparator, lhs, rhs));
            mergeHelper.push_back(tempVector);
        }

        while (mergeHelper.size() != 1) {
            mergeHelper.push_back(mergeIntoList(BufferPtr, getIteratorAlt(mergeHelper[0]), getIteratorAlt(mergeHelper[1]), comparator, lhs, rhs));
            mergeHelper.erase(mergeHelper.begin());
            mergeHelper.erase(mergeHelper.begin());
        }

        mergeUs.push_back(getIteratorAlt(mergeHelper[0]));

        //update i
        i = j;
    }

    mergeIntoFile(sortIntoMe, mergeUs, comparator, lhs, rhs);
}

#endif

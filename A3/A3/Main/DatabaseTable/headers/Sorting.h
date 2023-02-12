
#ifndef SORTING_H
#define SORTING_H

#include "MyDB_PageReaderWriter.h"
#include "MyDB_TableRecIterator.h"
#include "MyDB_TableRecIteratorAlt.h"
#include "MyDB_TableReaderWriter.h"

// performs a TPMMS of the table sortMe.  The results are written to sortIntoMe.  The run 
// size for the first phase of the TPMMS is given by runSize.  Comarisons are performed 
// using comparator, lhs, rhs
void sort (int runSize, MyDB_TableReaderWriter &sortMe, MyDB_TableReaderWriter &sortIntoMe,
        function <bool ()> comparator, MyDB_RecordPtr lhs, MyDB_RecordPtr rhs);

// helper function.  Gets two iterators, leftIter and rightIter.  It is assumed that these are iterators over
// sorted lists of records.  This function then merges all of those records into a list of anonymous pages,
// and returns the list of anonymous pages to the caller.  The resulting list of anonymous pages is sorted.
// Comparisons are performed using comparator, lhs, rhs
vector <MyDB_PageReaderWriter> mergeIntoList (MyDB_BufferManagerPtr parent, MyDB_RecordIteratorAltPtr leftIter,
        MyDB_RecordIteratorAltPtr rightIter, function <bool ()> comparator, MyDB_RecordPtr lhs, MyDB_RecordPtr rhs);

// accepts a list of iterators called mergeUs.  It is assumed that these are all iterators over sorted lists
// of records.  This function then merges all of those records and appends them to the file sortIntoMe.  If
// all of the iterators are over sorted lists of records, then all of the records appended onto the end of
// sortIntoMe will be sorted.  Comparisons are performed using comparator, lhs, rhs
void mergeIntoFile (MyDB_TableReaderWriter &sortIntoMe, vector <MyDB_RecordIteratorAltPtr> &mergeUs,
        function <bool ()> comparator, MyDB_RecordPtr lhs, MyDB_RecordPtr rhs);

#endif

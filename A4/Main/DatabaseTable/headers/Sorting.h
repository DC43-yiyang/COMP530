
#ifndef SORTING_H
#define SORTING_H

#include "MyDB_PageReaderWriter.h"
#include "MyDB_TableRecIterator.h"
#include "MyDB_TableReaderWriter.h"
#include "IteratorComparator.h"

// performs a TPMMS of the table sortMe.  The results are written to sortIntoMe.  The run 
// size for the first phase of the TPMMS is given by runSize.  Comparisons are performed 
// using comparator, lhs, rhs
void sort (int runSize, MyDB_TableReaderWriter &sortMe, MyDB_TableReaderWriter &sortIntoMe,
        function <bool ()> comparator, MyDB_RecordPtr lhs, MyDB_RecordPtr rhs);

// Accepts the input file sortMe, and then uses the specified comparator over the records lhs 
// and rhs to sort the file into a set of sorted runs of length at most runSize.  It then
// constructs an iterator over those runs, that can be used to scan the data in sorted order
// in the input file.
MyDB_RecordIteratorAltPtr buildItertorOverSortedRuns (int runSize, MyDB_TableReaderWriter &sortMe,
        function <bool ()> comparator, MyDB_RecordPtr lhs, MyDB_RecordPtr rhs);

// just like the above, except that in addition, the specified selection predicates are run
// over the input records so that only the records matching the selection predicates is sorted
MyDB_RecordIteratorAltPtr buildItertorOverSortedRuns (int runSize, MyDB_TableReaderWriter &sortMe,
        function <bool ()> comparator, MyDB_RecordPtr lhs, MyDB_RecordPtr rhs, string pred);

// helper function.  Gets two iterators, leftIter and rightIter.  It is assumed that these are iterators over
// sorted lists of records.  This function then merges all of those records into a list of anonymous pages,
// and returns the list of anonymous pages to the caller.  The resulting list of anonymous pages is sorted.
// Comparisons are performed using comparator, lhs, rhs
vector <MyDB_PageReaderWriter> mergeIntoList (MyDB_BufferManagerPtr parent, MyDB_RecordIteratorAltPtr leftIter,
        MyDB_RecordIteratorAltPtr rightIter, function <bool ()> comparator, MyDB_RecordPtr lhs, MyDB_RecordPtr rhs);

#endif

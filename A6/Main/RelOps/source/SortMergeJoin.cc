
#ifndef SORTMERGE_CC
#define SORTMERGE_CC

#include "Aggregate.h"
#include "MyDB_Record.h"
#include "MyDB_PageReaderWriter.h"
#include "MyDB_TableReaderWriter.h"
#include "SortMergeJoin.h"
#include "Sorting.h"

SortMergeJoin :: SortMergeJoin (MyDB_TableReaderWriterPtr, MyDB_TableReaderWriterPtr,
                MyDB_TableReaderWriterPtr, string, 
                vector <string>,
                pair <string, string>, string,
                string) {}

void SortMergeJoin :: run () {}

#endif

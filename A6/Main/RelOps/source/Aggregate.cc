
#ifndef AGG_CC
#define AGG_CC

#include "MyDB_Record.h"
#include "MyDB_PageReaderWriter.h"
#include "MyDB_TableReaderWriter.h"
#include "Aggregate.h"
#include <unordered_map>

using namespace std;

Aggregate :: Aggregate (MyDB_TableReaderWriterPtr, MyDB_TableReaderWriterPtr,
                vector <pair <MyDB_AggType, string>>,
                vector <string>, string) {}

void Aggregate :: run () {}

#endif



#ifndef TABLE_H
#define TABLE_H

#include <iostream>
#include "MyDB_Catalog.h"
#include "MyDB_Schema.h"
#include "MyDB_Table.h"
#include <memory>
#include <string>

// create a smart pointer for database tables
using namespace std;
class MyDB_Table;
typedef shared_ptr <MyDB_Table> MyDB_TablePtr;

// this class encapsulates the notion of a database table
class MyDB_Table {

public:

	// creates a new table with the given name, at the given storage location; the schema is empty
	MyDB_Table (string tableName, string storageLoc);

	// creates a table with the given name, at the given storage location, and the given schema
	MyDB_Table (string tableName, string storageLoc, MyDB_SchemaPtr mySchema);

	// creates a table with the given name, at the given storage location, and the given schema,
	// of the given type, with the given sort att
	MyDB_Table (string tableName, string storageLoc, MyDB_SchemaPtr mySchema, string fileType, string sortAtt);
	
	// creates an empty table object
	MyDB_Table ();

	// load the table information from the catalog; return false iff not in the catalog
	bool fromCatalog (string tableName, MyDB_CatalogPtr catalog);

	// write the dude to a catalog
	void putInCatalog (MyDB_CatalogPtr catalog);

	// get the name of the table
	string &getName ();

	// get the storage location of the table
	string &getStorageLoc ();

	// gete the schema for this table
	MyDB_SchemaPtr getSchema ();

	// kill the dude
	~MyDB_Table ();

	// print to the screen
	void print ();

	// return the last page in the table; -1 if there has never been anything
	// written to the table (the table has just been initialized and setLastPage
	// has never been called)
	int lastPage ();

	// set the last page
	void setLastPage (size_t lastPage);

	// get the list of all of the tables from the catalog
	static map <string, MyDB_TablePtr> getAllTables (MyDB_CatalogPtr fromMe);

	// to print out a schema to the screen
	friend std::ostream& operator<<(std::ostream& os, const MyDB_TablePtr printMe);
	friend std::ostream& operator<<(std::ostream& os, const MyDB_Table printMe);

	// the sort att
	string &getSortAtt ();

	// the file type (ex: "heap" or "bplustree")
	string &getFileType ();

	// get/set the root location
	void setRootLocation (int toMe);
	int getRootLocation ();

        // get the distinct value count for an attribute
        size_t getDistinctValues (string forMe);
        size_t getDistinctValues (int forMe);

        // set the distinct value count for all attributes
        void setDistinctValues (vector <size_t> &toMe);

        // get/set the number of tuples in the relation
        void setTupleCount (size_t toMe);
        size_t getTupleCount ();

private:

	// the distinct value counts
	vector <size_t> allCounts;

	// the number of tuples
	int count;

	// the name of the sort att
	string sortAtt;

	// the type of the file
	string fileType;
	
	// the last used page in the table
	int last;

	// the name of the table
	string tableName;

	// the location where it is stored
	string storageLoc;

	// the schema for this table
	MyDB_SchemaPtr mySchema;

	// location of the root node
	int rootLocation;
};

#endif

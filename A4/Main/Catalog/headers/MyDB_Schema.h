
#ifndef SCHEMA_H
#define SCHEMA_H

#include <memory>
#include <iostream>
#include "MyDB_AttType.h"
#include "MyDB_Catalog.h"
#include <vector>

using namespace std;

// create a smart pointer for records
class MyDB_Schema;
typedef shared_ptr <MyDB_Schema> MyDB_SchemaPtr;

class MyDB_Schema {

public:

	// get a particular attribute... the pair is the index (first, second, third, etc.) and the type
	pair <int, MyDB_AttTypePtr> getAttByName (string findMe);

	// get the list of all of the attributes... the pair is the name and the type
	vector <pair <string, MyDB_AttTypePtr>> &getAtts ();

	// append another attribute to the schema
	void appendAtt (pair <string, MyDB_AttTypePtr> addAtt);

	// create this schema by loading from the catalog
	void fromCatalog (string tableName, MyDB_CatalogPtr catalog);

	// add to the catalog
	void putInCatalog (string TableName, MyDB_CatalogPtr toMe);

	// to print out the the screen
	friend std::ostream& operator<<(std::ostream& os, const MyDB_Schema printMe);
	friend std::ostream& operator<<(std::ostream& os, const MyDB_SchemaPtr printMe);

private:

	// add an attribute for the given table to the catalog
	static void addAtt (string tableName, pair <string, MyDB_AttTypePtr>, MyDB_CatalogPtr catalog);

	// this is a list, in order, of the attributes in the schema
	// the string is the name of the attribute, and we also know the types
	vector <pair <string, MyDB_AttTypePtr>> allAtts;
};

#endif

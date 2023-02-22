
#ifndef SCHEMA_C
#define SCHEMA_C

#include <iostream>
#include "MyDB_Schema.h"

using namespace std;

pair <int, MyDB_AttTypePtr> MyDB_Schema :: getAttByName (string findMe) {

	// search for the information on a particular attributes
	int counter = 0;
	for (auto &entry : allAtts) {
		if (entry.first == findMe) {
			return make_pair (counter, entry.second);
		} else {
			counter++;
		}
	}	
	cout << "Could not find attribute " << findMe << "\n";
	cout << "Candidates were: \n";
	for (auto entry : allAtts) {
		cout << "\t" << entry.first << "\n";
	}
	return make_pair (-1, nullptr);
}

void MyDB_Schema :: addAtt (string tableName, pair <string, MyDB_AttTypePtr> attToAdd, MyDB_CatalogPtr catalog) {

	vector <string> myAtts;	
	catalog->getStringList (tableName + ".attList", myAtts);
	bool foundIt = false;
	for (string s : myAtts) {
		if (s == attToAdd.first) {
			foundIt = true;
		}
	}
	if (!foundIt) {
		myAtts.push_back (attToAdd.first);
	}
	catalog->putStringList (tableName + ".attList", myAtts);
	catalog->putString (tableName + "." + attToAdd.first + ".type", attToAdd.second->toString ());
}


void MyDB_Schema :: fromCatalog (string tableName, MyDB_CatalogPtr catalog) {
	
	// get the list of attribtes
	vector <string> myAtts;
	if (!catalog->getStringList (tableName + ".attList", myAtts))
		return;
	
	// get the type of each attribute
	for (string s : myAtts) {
		string attType;
		catalog->getString (tableName + "." + s + ".type", attType);
		if (attType == "int") {
			allAtts.push_back (make_pair (s, make_shared <MyDB_IntAttType> ()));
		} else if (attType == "double") {
			allAtts.push_back (make_pair (s, make_shared <MyDB_DoubleAttType> ()));
		} else if (attType == "string") {
			allAtts.push_back (make_pair (s, make_shared <MyDB_StringAttType> ()));
		} else if (attType == "bool") {
			allAtts.push_back (make_pair (s, make_shared <MyDB_BoolAttType> ()));
		} else {
			cout << "Bad att type for attribute " << s << ": " << attType << "\n";
			exit (1);
		}
	}
}

void MyDB_Schema :: appendAtt (pair <string, MyDB_AttTypePtr> addAtt) {
	allAtts.push_back (addAtt);
}

void MyDB_Schema :: putInCatalog (string tableName, MyDB_CatalogPtr catalog) {

	// write out the attributes
	for (auto entry : allAtts) {
		addAtt (tableName, entry, catalog);
	}	
}

vector <pair <string, MyDB_AttTypePtr>> &MyDB_Schema :: getAtts () {
	return allAtts;
}

std::ostream& operator<<(std::ostream& os, const MyDB_Schema printMe) {
	int first = true;
	for (auto entry : printMe.allAtts) {
		os << "[";
		if (!first)
			os << ", ";
		os << "<" << entry.first << ", " << entry.second->toString () << ">";	
		first = false;
		os << "]";
	}
        return os;
}

std::ostream& operator<<(std::ostream& os, const MyDB_SchemaPtr printMe) {
        if (printMe == nullptr) {
                os << "<null>";
        } else {
		int first = true;
		os << "[";
		for (auto entry : printMe->allAtts) {
			if (!first)
				os << ", ";
			os << "<" << entry.first << ", " << entry.second->toString () << ">";	
			first = false;
		}
		os << "]";
        }
        return os;
}

#endif


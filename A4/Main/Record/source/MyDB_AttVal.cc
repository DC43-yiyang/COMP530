
#ifndef ATT_VAL_C
#define ATT_VAL_C

#include <iostream>
#include "MyDB_AttVal.h"
#include <string>
#include <string.h>

using namespace std;

MyDB_AttVal :: ~MyDB_AttVal () {}

int MyDB_IntAttVal :: toInt () {
	void *dataPtr = getDataPointer ();
	if (dataPtr == nullptr) 
		return value;
	else
		return *((int *) dataPtr);
}

void MyDB_IntAttVal :: fromInt (int fromMe) {
	value = fromMe;
	setNotBuffered ();
}

double MyDB_IntAttVal :: toDouble () {
	void *dataPtr = getDataPointer ();
	if (dataPtr == nullptr) 
		return (double) value;
	else
		return (double) *((int *) dataPtr);
}

string MyDB_IntAttVal :: toString () {
	void *dataPtr = getDataPointer ();
	if (dataPtr == nullptr) 
		return to_string (value);
	else
		return to_string (*((int *) dataPtr));
}

void MyDB_IntAttVal :: set (MyDB_AttValPtr fromMe) {
	value = fromMe->toInt ();
	setNotBuffered ();
}

void MyDB_BoolAttVal :: set (MyDB_AttValPtr fromMe) {
	value = fromMe->toBool ();
	setNotBuffered ();
}

void MyDB_StringAttVal :: set (MyDB_AttValPtr fromMe) {
	value = fromMe->toString ();
	setNotBuffered ();
}

void MyDB_DoubleAttVal :: set (MyDB_AttValPtr fromMe) {
	value = fromMe->toDouble ();
	setNotBuffered ();
}

void MyDB_IntAttVal :: fromString (string &fromMe) {
	value = stoi (fromMe);
	setNotBuffered ();
}

size_t MyDB_IntAttVal :: hash () {
	return std :: hash <int> () (toInt ());
}

size_t MyDB_DoubleAttVal :: hash () {
	return std :: hash <int> () (toDouble ());
}

size_t MyDB_BoolAttVal :: hash () {
	return std :: hash <int> () (toBool ());
}

size_t MyDB_StringAttVal :: hash () {
	return std :: hash <string> () (toString ());
}

bool MyDB_IntAttVal :: toBool () {
	cout << "Oops!  Can't convert int to bool";
	exit (1);
}

void MyDB_IntAttVal :: serialize (char *&buffer, size_t &allocatedSize, size_t &totSize) {

	extendBuffer (buffer, allocatedSize, totSize, sizeof (int) + sizeof (short));

	*((short *) (buffer + totSize)) = (short) (sizeof (short) + sizeof (int));
	totSize += sizeof (short);
	*((int *) (buffer + totSize)) = toInt ();
	totSize += sizeof (int);
}

void MyDB_IntAttVal :: set (int val) {
	value = val;
	setNotBuffered ();
}

MyDB_IntAttVal :: MyDB_IntAttVal () {
	value = 0;
	setNotBuffered ();
}

MyDB_IntAttVal :: ~MyDB_IntAttVal () {}

int MyDB_DoubleAttVal :: toInt () {
	void *dataPtr = getDataPointer ();
	if (dataPtr == nullptr) 
		return (int) value;
	else
		return (int) *((double *) dataPtr);
}

void MyDB_DoubleAttVal :: fromInt (int fromMe) {
	value = (double) fromMe;
	setNotBuffered ();
}

void MyDB_DoubleAttVal :: fromString (string &fromMe) {
	value = stod (fromMe);
	setNotBuffered ();
}

double MyDB_DoubleAttVal :: toDouble () {
	void *dataPtr = getDataPointer ();
	if (dataPtr == nullptr) 
		return value;
	else
		return *((double *) dataPtr);
}

string MyDB_DoubleAttVal :: toString () {
	void *dataPtr = getDataPointer ();
	if (dataPtr == nullptr) 
		return to_string (value);
	else
		return to_string (*((double *) dataPtr));
}

bool MyDB_DoubleAttVal :: toBool () {
	cout << "Oops!  Can't convert int to bool";
	exit (1);
}

void MyDB_DoubleAttVal :: serialize (char *&buffer, size_t &allocatedSize, size_t &totSize) {

	extendBuffer (buffer, allocatedSize, totSize, sizeof (double) + sizeof (short));

	*((short *) (buffer + totSize)) = (short) (sizeof (short) + sizeof (double));
	totSize += sizeof (short);
	*((double *) (buffer + totSize)) = toDouble ();
	totSize += sizeof (double);
}

void MyDB_DoubleAttVal :: set (double val) {
	value = val;
	setNotBuffered ();
}

MyDB_DoubleAttVal :: MyDB_DoubleAttVal () {
	value = 0;
	setNotBuffered ();
}

MyDB_DoubleAttVal :: ~MyDB_DoubleAttVal () {}


MyDB_StringAttVal :: ~MyDB_StringAttVal () {}

int MyDB_StringAttVal :: toInt () {
        cout << "Oops!  Can't convert string to int";
        exit (1);
}

void MyDB_StringAttVal :: fromString (string &fromMe) {
        value = fromMe;
	setNotBuffered ();
}

double MyDB_StringAttVal :: toDouble () {
        cout << "Oops!  Can't convert int to double";
        exit (1);
}

void MyDB_StringAttVal :: fromInt (int fromMe) {
	value = to_string (fromMe);
	setNotBuffered ();
}

string MyDB_StringAttVal :: toString () {
	void *dataPtr = getDataPointer ();
	if (dataPtr == nullptr) 
		return value;
	else
		return string ((char *) dataPtr);
}

bool MyDB_StringAttVal :: toBool () {
        cout << "Oops!  Can't convert int to bool";
        exit (1);
}

void MyDB_StringAttVal :: serialize (char *&buffer, size_t &allocatedSize, size_t &totSize) {

	string value = toString ();

	extendBuffer (buffer, allocatedSize, totSize, strlen (value.c_str ()) + 1 + sizeof (short));

	*((short *) (buffer + totSize)) = (short) (sizeof (short) + strlen (value.c_str ()) + 1);
	totSize += sizeof (short);
	memcpy (buffer + totSize, value.c_str (), strlen (value.c_str ()) + 1);
	totSize += strlen (value.c_str ()) + 1;
}

void MyDB_StringAttVal :: set (string val) {
        value = val;
	setNotBuffered ();
}

MyDB_StringAttVal :: MyDB_StringAttVal () {
        value = "";
	setNotBuffered ();
}

int MyDB_BoolAttVal :: toInt () {
	cout << "Oops!  Can't convert bool to int";
	exit (1);
}

double MyDB_BoolAttVal :: toDouble () {
	cout << "Oops!  Can't convert bool to double";
	exit (1);
}

string MyDB_BoolAttVal :: toString () {
	bool val;
	void *dataPtr = getDataPointer ();
	if (dataPtr == nullptr) 
		val = value;
	else 
		val = (*((char *) dataPtr) == 1);
	if (val) {
		return "true";
	} else {
		return "false";
	}
}

void MyDB_BoolAttVal :: fromString (string &fromMe) {
	if (fromMe == "false") {
		value = false;
	} else if (fromMe == "true") {
		value = true;
	} else {
		cout << "Oops!  Bad string for boolean\n";
		exit (1);
	}
	setNotBuffered ();
}

void MyDB_BoolAttVal :: fromInt (int fromMe) {
	value = (fromMe == 1);
	setNotBuffered ();
}

bool MyDB_BoolAttVal :: toBool () {
	void *dataPtr = getDataPointer ();
	if (dataPtr == nullptr) 
		return value;
	else 
		return (*((char *) dataPtr) == 1);
}

void MyDB_BoolAttVal :: serialize (char *&buffer, size_t &allocatedSize, size_t &totSize) {

	bool value = toBool ();

	extendBuffer (buffer, allocatedSize, totSize, sizeof (char) + sizeof (short));

	*((short *) (buffer + totSize)) = (short) (sizeof (short) + sizeof (char));
	totSize += sizeof (short);
	if (value) {
		*(buffer + totSize) = 1;
	} else {
		*(buffer + totSize) = 0;
	}
	totSize += sizeof (char);
}

void MyDB_BoolAttVal :: set (bool val) {
	value = val;
	setNotBuffered ();
}

MyDB_AttValPtr MyDB_IntAttVal :: getCopy () {
	MyDB_IntAttValPtr retVal = make_shared <MyDB_IntAttVal> ();
	retVal->set (toInt ());
	return retVal;	
}

MyDB_AttValPtr MyDB_DoubleAttVal :: getCopy () {
	MyDB_DoubleAttValPtr retVal = make_shared <MyDB_DoubleAttVal> ();
	retVal->set (toDouble ());
	return retVal;	
}

MyDB_AttValPtr MyDB_StringAttVal :: getCopy () {
	MyDB_StringAttValPtr retVal = make_shared <MyDB_StringAttVal> ();
	retVal->set (toString ());
	return retVal;	
}

MyDB_AttValPtr MyDB_BoolAttVal :: getCopy () {
	MyDB_BoolAttValPtr retVal = make_shared <MyDB_BoolAttVal> ();
	retVal->set (toBool ());
	return retVal;	
}

MyDB_BoolAttVal :: MyDB_BoolAttVal () {
	value = false;
	setNotBuffered ();
}

MyDB_BoolAttVal :: ~MyDB_BoolAttVal () {}

#endif

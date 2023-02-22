
#ifndef ATT_TYPE_H
#define ATT_TYPE_H

#include "MyDB_AttVal.h"
#include <float.h>
#include <climits>
#include <memory>
#include <string>

// create a smart pointer for database tables
using namespace std;
class MyDB_AttType;
typedef shared_ptr <MyDB_AttType> MyDB_AttTypePtr;

class MyDB_AttType {

public:

	virtual bool promotableToInt () = 0;
	virtual bool promotableToDouble () = 0;
	virtual bool promotableToString () = 0;
	virtual MyDB_AttValPtr createAtt () = 0;
	virtual MyDB_AttValPtr createAttMax () = 0;
	virtual string toString () = 0;
	virtual bool isBool () = 0;
};

class MyDB_IntAttType : public MyDB_AttType {

public: 
	
	bool promotableToInt () {
		return true;
	}

	bool promotableToDouble () {
		return true;
	}

	string toString () {
		return "int";
	}

	bool promotableToString () {
		return true;
	}

	bool isBool () {
		return false;
	}

	MyDB_AttValPtr createAtt () {
		return make_shared <MyDB_IntAttVal> ();
	}	

	MyDB_AttValPtr createAttMax () {
		MyDB_IntAttValPtr retVal = make_shared <MyDB_IntAttVal> ();
		retVal->set (INT_MAX);
		return retVal;	
	}	
	
};

class MyDB_DoubleAttType : public MyDB_AttType {

public: 
	
	bool promotableToInt () {
		return false;
	}

	bool promotableToDouble () {
		return true;
	}

	bool promotableToString () {
		return true;
	}

	string toString () {
		return "double";
	}

	bool isBool () {
		return false;
	}

	MyDB_AttValPtr createAtt () {
		return make_shared <MyDB_DoubleAttVal> ();
	}	

	MyDB_AttValPtr createAttMax () {
		MyDB_DoubleAttValPtr retVal = make_shared <MyDB_DoubleAttVal> ();
		retVal->set (1.79769e+308);
		return retVal;	
	}	
};

class MyDB_StringAttType : public MyDB_AttType {

public: 
	
	bool promotableToInt () {
		return false;
	}

	bool promotableToDouble () {
		return false;
	}

	bool promotableToString () {
		return true;
	}

	bool isBool () {
		return false;
	}

	string toString () {
		return "string";
	}

	MyDB_AttValPtr createAtt () {
		return make_shared <MyDB_StringAttVal> ();
	}	

	MyDB_AttValPtr createAttMax () {
		MyDB_StringAttValPtr retVal = make_shared <MyDB_StringAttVal> ();
		retVal->set ("~~~~~~~~~");
		return retVal;	
	}	
};

class MyDB_BoolAttType : public MyDB_AttType {

public: 
	
	bool promotableToInt () {
		return false;
	}

	bool promotableToDouble () {
		return false;
	}

	bool promotableToString () {
		return true;
	}

	bool isBool () {
		return true;
	}

	string toString () {
		return "bool";
	}

	MyDB_AttValPtr createAtt () {
		return make_shared <MyDB_BoolAttVal> ();
	}	

	MyDB_AttValPtr createAttMax () {
		MyDB_BoolAttValPtr retVal = make_shared <MyDB_BoolAttVal> ();
		retVal->set (true);
		return retVal;	
	}	
};

#endif

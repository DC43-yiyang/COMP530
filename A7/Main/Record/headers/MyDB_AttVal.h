
#ifndef ATT_VAL_H
#define ATT_VAL_H

#include <memory>
#include <string>
#include <string.h>

// create a smart pointer for the catalog
using namespace std;
class MyDB_AttVal;
typedef shared_ptr <MyDB_AttVal> MyDB_AttValPtr;

class MyDB_AttVal {

private:

	// this is a pointer into a buffer
	void *myData;

	// this tells us whether we are using the buffer
	bool usingBuffer;

public:

	virtual int toInt () = 0;
	virtual void fromInt (int fromMe) = 0;
	virtual double toDouble () = 0;
	virtual string toString () = 0;
	virtual bool toBool () = 0;
	virtual void set (MyDB_AttValPtr toMe) = 0;
	virtual size_t hash () = 0;
	virtual MyDB_AttValPtr getCopy () = 0;
	virtual void fromString (string &fromMe) = 0;
	virtual void serialize (char *&buffer, size_t &allocatedSize, size_t &totSize) = 0;
	virtual ~MyDB_AttVal ();

	// this gets a pointer to our data... useful because we can avoid deserializing the record
	inline void *getDataPointer () {
		return myData;
	}

	inline void extendBuffer (char *&buffer, size_t &allocatedSize, size_t &totSize, int extraSpaceNeeded) {
		if (totSize + extraSpaceNeeded > allocatedSize) {
			size_t newSize = (totSize + extraSpaceNeeded) * 2;
			char *newBuff = new char[newSize];
			memcpy (newBuff, buffer, allocatedSize);
			delete [] buffer;
			buffer = newBuff;
			allocatedSize = newSize;
		}
	}

	inline void setBuffered (char *where) {
		myData = where;
		usingBuffer = true;
	}

	inline void setNotBuffered () {
		myData = nullptr;
		usingBuffer = false;
	}

	MyDB_AttVal () {
		setNotBuffered ();	
	}

	inline char *fromBinary (char *fromHere) {

		// this is the length
		int myLen = *((short *) fromHere);

		// remember our data
		setBuffered (fromHere + sizeof (short));

		// and return a pointer to the next guy
		return fromHere + myLen; 
	}

};

class MyDB_IntAttVal;
typedef shared_ptr <MyDB_IntAttVal> MyDB_IntAttValPtr;

class MyDB_IntAttVal : public MyDB_AttVal {

public:

	int toInt () override;
	double toDouble () override;
	string toString () override;
	void fromInt (int fromMe) override;
	bool toBool () override;
	void fromString (string &fromMe) override;
	void set (MyDB_AttValPtr toMe) override;
	size_t hash () override;
	MyDB_AttValPtr getCopy () override;
	void serialize (char *&buffer, size_t &allocatedSize, size_t &totSize) override;
	void set (int val);
	MyDB_IntAttVal ();
	~MyDB_IntAttVal ();

private:

	int value;
};

class MyDB_DoubleAttVal;
typedef shared_ptr <MyDB_DoubleAttVal> MyDB_DoubleAttValPtr;

class MyDB_DoubleAttVal : public MyDB_AttVal {

public:

	int toInt () override;
	double toDouble () override;
	string toString () override;
	bool toBool () override;
	void fromInt (int fromMe) override;
	MyDB_AttValPtr getCopy () override;
	size_t hash () override;
	void set (MyDB_AttValPtr toMe) override;
	void fromString (string &fromMe) override;
	void serialize (char *&buffer, size_t &allocatedSize, size_t &totSize) override;
	void set (double val);
	MyDB_DoubleAttVal ();
	~MyDB_DoubleAttVal ();

private:

	double value;
};

class MyDB_StringAttVal;
typedef shared_ptr <MyDB_StringAttVal> MyDB_StringAttValPtr;

class MyDB_StringAttVal : public MyDB_AttVal {

public:

	int toInt () override;
	double toDouble () override;
	string toString () override;
	bool toBool () override;
	void fromString (string &fromMe) override;
	MyDB_AttValPtr getCopy () override;
	size_t hash () override;
	void set (MyDB_AttValPtr toMe) override;
	void serialize (char *&buffer, size_t &allocatedSize, size_t &totSize) override;
	void fromInt (int fromMe) override;
	void set (string val);
	MyDB_StringAttVal ();
	~MyDB_StringAttVal ();

private:

	string value;
};

class MyDB_BoolAttVal;
typedef shared_ptr <MyDB_BoolAttVal> MyDB_BoolAttValPtr;

class MyDB_BoolAttVal : public MyDB_AttVal {

public:

	int toInt () override;
	double toDouble () override;
	string toString () override;
	bool toBool () override;
	void fromString (string &fromMe) override;
	void set (MyDB_AttValPtr toMe) override;
	MyDB_AttValPtr getCopy () override;
	size_t hash () override;
	void fromInt (int fromMe) override;
	void serialize (char *&buffer, size_t &allocatedSize, size_t &totSize) override;
	void set (bool val);
	MyDB_BoolAttVal ();
	~MyDB_BoolAttVal ();

private:

	bool value;
};



#endif

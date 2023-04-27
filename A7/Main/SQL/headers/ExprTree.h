
#ifndef SQL_EXPRESSIONS
#define SQL_EXPRESSIONS

#include "MyDB_AttType.h"
#include <string>
#include <vector>

// create a smart pointer for database tables
using namespace std;
class ExprTree;
typedef shared_ptr<ExprTree> ExprTreePtr;

// this class encapsules a parsed SQL expression (such as "this.that > 34.5 AND 4 = 5")

// class ExprTree is a pure virtual class... the various classes that implement it are below
class ExprTree : public enable_shared_from_this<ExprTree> {

public:
  // here are a bunch of self-explanatory operations that allow manipulation of expressions
  virtual string toString() = 0;
  virtual ~ExprTree() {}
  virtual bool isEq() { return false; }
  virtual bool isId() { return false; }
  virtual bool isOr() { return false; }
  virtual bool isComp() { return false; }
  virtual bool isLTGT() { return false; }
  virtual bool isNotEq() { return false; }
  virtual bool isSum() { return false; }
  virtual bool isAvg() { return false; }

  virtual MyDB_AttValPtr getLiteral() { return nullptr; }

  virtual string getId() { return ""; }
  virtual ExprTreePtr getLHS() { return nullptr; }
  virtual ExprTreePtr getRHS() { return nullptr; }
  virtual ExprTreePtr getChild() { return nullptr; }
  virtual bool referencesTable(string alias) { return false; }
  virtual bool referencesAtt(string alias, string attName) { return false; }
  virtual bool hasAgg() {
    if (isSum() || isAvg())
      return true;
    ExprTreePtr lhs = getLHS();
    ExprTreePtr rhs = getRHS();
    ExprTreePtr child = getChild();
    bool res = false;
    if (lhs != nullptr)
      res = res || lhs->hasAgg();
    if (rhs != nullptr)
      res = res || rhs->hasAgg();
    if (child != nullptr)
      res = res || child->hasAgg();
    return res;
  }

  virtual vector<ExprTreePtr> getAggExprs() {
    if (isSum() || isAvg())
      return {shared_from_this()};
    if (getChild())
      return getChild()->getAggExprs();
    if (getLHS() && getRHS()) {
      vector<ExprTreePtr> vec1 = getLHS()->getAggExprs();
      vector<ExprTreePtr> vec2 = getRHS()->getAggExprs();
      vec1.insert(vec1.end(), make_move_iterator(vec2.begin()), make_move_iterator(vec2.end()));
      return vec1;
    }
    return {};
  }

  virtual vector<ExprTreePtr> getIdentifiers() {
    if (isId())
      return {shared_from_this()};
    if (getChild())
      return getChild()->getIdentifiers();
    if (getLHS() && getRHS()) {
      vector<ExprTreePtr> vec1 = getLHS()->getIdentifiers();
      vector<ExprTreePtr> vec2 = getRHS()->getIdentifiers();
      vec1.insert(vec1.end(), make_move_iterator(vec2.begin()), make_move_iterator(vec2.end()));
      return vec1;
    }
    return {};
  }
};

class BoolLiteral : public ExprTree {

private:
  bool myVal;

public:
  BoolLiteral(bool fromMe) {
    myVal = fromMe;
  }

  string toString() {
    if (myVal) {
      return "bool[true]";
    } else {
      return "bool[false]";
    }
  }

  MyDB_AttValPtr getLiteral() override {
    auto value = make_shared<MyDB_BoolAttVal>();
    value->set(myVal);
    return value;
  }
};

class DoubleLiteral : public ExprTree {

private:
  double myVal;

public:
  DoubleLiteral(double fromMe) {
    myVal = fromMe;
  }

  string toString() {
    return "double[" + to_string(myVal) + "]";
  }

  ~DoubleLiteral() {}

  MyDB_AttValPtr getLiteral() override {
    auto value = make_shared<MyDB_DoubleAttVal>();
    value->set(myVal);
    return value;
  }
};

// this implement class ExprTree
class IntLiteral : public ExprTree {

private:
  int myVal;

public:
  IntLiteral(int fromMe) {
    myVal = fromMe;
  }

  string toString() {
    return "int[" + to_string(myVal) + "]";
  }

  ~IntLiteral() {}

  MyDB_AttValPtr getLiteral() override {
    auto value = make_shared<MyDB_IntAttVal>();
    value->set(myVal);
    return value;
  }
};

class StringLiteral : public ExprTree {

private:
  string myVal;

public:
  StringLiteral(char *fromMe) {
    fromMe[strlen(fromMe) - 1] = 0;
    myVal = string(fromMe + 1);
  }

  string toString() {
    return "string[" + myVal + "]";
  }

  ~StringLiteral() {}

  MyDB_AttValPtr getLiteral() override {
    auto value = make_shared<MyDB_StringAttVal>();
    value->set(myVal);
    return value;
  }
};

class Identifier : public ExprTree {

private:
  string tableName;
  string attName;

public:
  Identifier(char *tableNameIn, char *attNameIn) {
    tableName = string(tableNameIn);
    attName = string(attNameIn);
  }

  string toString() {
    return "[" + tableName + "_" + attName + "]";
  }

  string getId() {
    return tableName + "_" + attName;
  }

  bool isId() {
    return true;
  }

  bool referencesTable(string alias) {
    return alias == tableName;
  }

  bool referencesAtt(string alias, string attNameToFind) {
    return alias == tableName && attName == attNameToFind;
  }

  ~Identifier() {}
};

class MinusOp : public ExprTree {

private:
  ExprTreePtr lhs;
  ExprTreePtr rhs;

public:
  MinusOp(ExprTreePtr lhsIn, ExprTreePtr rhsIn) {
    lhs = lhsIn;
    rhs = rhsIn;
  }

  string toString() {
    return "- (" + lhs->toString() + ", " + rhs->toString() + ")";
  }

  string getId() {
    string res = lhs->getId();
    if (res != "")
      return res;
    else
      return rhs->getId();
  }

  ExprTreePtr getLHS() {
    return lhs;
  }

  ExprTreePtr getRHS() {
    return rhs;
  }

  bool referencesTable(string alias) {
    return lhs->referencesTable(alias) || rhs->referencesTable(alias);
  }

  bool referencesAtt(string alias, string attNameToFind) {
    return lhs->referencesAtt(alias, attNameToFind) || rhs->referencesAtt(alias, attNameToFind);
  }

  ~MinusOp() {}
};

class PlusOp : public ExprTree {

private:
  ExprTreePtr lhs;
  ExprTreePtr rhs;

public:
  PlusOp(ExprTreePtr lhsIn, ExprTreePtr rhsIn) {
    lhs = lhsIn;
    rhs = rhsIn;
  }

  string getId() {
    string res = lhs->getId();
    if (res != "")
      return res;
    else
      return rhs->getId();
  }

  ExprTreePtr getLHS() {
    return lhs;
  }

  ExprTreePtr getRHS() {
    return rhs;
  }

  string toString() {
    return "+ (" + lhs->toString() + ", " + rhs->toString() + ")";
  }

  bool referencesTable(string alias) {
    return lhs->referencesTable(alias) || rhs->referencesTable(alias);
  }

  bool referencesAtt(string alias, string attNameToFind) {
    return lhs->referencesAtt(alias, attNameToFind) || rhs->referencesAtt(alias, attNameToFind);
  }

  ~PlusOp() {}
};

class TimesOp : public ExprTree {

private:
  ExprTreePtr lhs;
  ExprTreePtr rhs;

public:
  TimesOp(ExprTreePtr lhsIn, ExprTreePtr rhsIn) {
    lhs = lhsIn;
    rhs = rhsIn;
  }

  string getId() {
    string res = lhs->getId();
    if (res != "")
      return res;
    else
      return rhs->getId();
  }

  ExprTreePtr getLHS() {
    return lhs;
  }

  ExprTreePtr getRHS() {
    return rhs;
  }

  string toString() {
    return "* (" + lhs->toString() + ", " + rhs->toString() + ")";
  }

  bool referencesTable(string alias) {
    return lhs->referencesTable(alias) || rhs->referencesTable(alias);
  }

  bool referencesAtt(string alias, string attNameToFind) {
    return lhs->referencesAtt(alias, attNameToFind) || rhs->referencesAtt(alias, attNameToFind);
  }

  ~TimesOp() {}
};

class DivideOp : public ExprTree {

private:
  ExprTreePtr lhs;
  ExprTreePtr rhs;

public:
  DivideOp(ExprTreePtr lhsIn, ExprTreePtr rhsIn) {
    lhs = lhsIn;
    rhs = rhsIn;
  }

  string toString() {
    return "/ (" + lhs->toString() + ", " + rhs->toString() + ")";
  }

  string getId() {
    string res = lhs->getId();
    if (res != "")
      return res;
    else
      return rhs->getId();
  }

  ExprTreePtr getLHS() {
    return lhs;
  }

  ExprTreePtr getRHS() {
    return rhs;
  }

  bool referencesTable(string alias) {
    return lhs->referencesTable(alias) || rhs->referencesTable(alias);
  }

  bool referencesAtt(string alias, string attNameToFind) {
    return lhs->referencesAtt(alias, attNameToFind) || rhs->referencesAtt(alias, attNameToFind);
  }

  ~DivideOp() {}
};

class GtOp : public ExprTree {

private:
  ExprTreePtr lhs;
  ExprTreePtr rhs;

public:
  GtOp(ExprTreePtr lhsIn, ExprTreePtr rhsIn) {
    lhs = lhsIn;
    rhs = rhsIn;
  }

  string toString() {
    return "> (" + lhs->toString() + ", " + rhs->toString() + ")";
  }

  bool isLTGT() {
    return true;
  }

  bool isComp() {
    return true;
  }

  string getId() {
    string res = lhs->getId();
    if (res != "")
      return res;
    else
      return rhs->getId();
  }

  ExprTreePtr getLHS() {
    return lhs;
  }

  ExprTreePtr getRHS() {
    return rhs;
  }

  bool referencesTable(string alias) {
    return lhs->referencesTable(alias) || rhs->referencesTable(alias);
  }

  bool referencesAtt(string alias, string attNameToFind) {
    return lhs->referencesAtt(alias, attNameToFind) || rhs->referencesAtt(alias, attNameToFind);
  }

  ~GtOp() {}
};

class LtOp : public ExprTree {

private:
  ExprTreePtr lhs;
  ExprTreePtr rhs;

public:
  LtOp(ExprTreePtr lhsIn, ExprTreePtr rhsIn) {
    lhs = lhsIn;
    rhs = rhsIn;
  }

  string toString() {
    return "< (" + lhs->toString() + ", " + rhs->toString() + ")";
  }

  bool isLTGT() {
    return true;
  }

  bool isComp() {
    return true;
  }

  string getId() {
    string res = lhs->getId();
    if (res != "")
      return res;
    else
      return rhs->getId();
  }

  ExprTreePtr getLHS() {
    return lhs;
  }

  ExprTreePtr getRHS() {
    return rhs;
  }

  bool referencesTable(string alias) {
    return lhs->referencesTable(alias) || rhs->referencesTable(alias);
  }

  bool referencesAtt(string alias, string attNameToFind) {
    return lhs->referencesAtt(alias, attNameToFind) || rhs->referencesAtt(alias, attNameToFind);
  }

  ~LtOp() {}
};

class NeqOp : public ExprTree {

private:
  ExprTreePtr lhs;
  ExprTreePtr rhs;

public:
  NeqOp(ExprTreePtr lhsIn, ExprTreePtr rhsIn) {
    lhs = lhsIn;
    rhs = rhsIn;
  }

  bool isNotEq() {
    return true;
  }

  bool isComp() {
    return true;
  }

  string getId() {
    string res = lhs->getId();
    if (res != "")
      return res;
    else
      return rhs->getId();
  }

  ExprTreePtr getLHS() {
    return lhs;
  }

  ExprTreePtr getRHS() {
    return rhs;
  }

  string toString() {
    return "!= (" + lhs->toString() + ", " + rhs->toString() + ")";
  }

  ~NeqOp() {}
};

class OrOp : public ExprTree {

private:
  ExprTreePtr lhs;
  ExprTreePtr rhs;

public:
  OrOp(ExprTreePtr lhsIn, ExprTreePtr rhsIn) {
    lhs = lhsIn;
    rhs = rhsIn;
  }

  string toString() {
    return "|| (" + lhs->toString() + ", " + rhs->toString() + ")";
  }

  bool isOr() {
    return true;
  }

  string getId() {
    string res = lhs->getId();
    if (res != "")
      return res;
    else
      return rhs->getId();
  }

  ExprTreePtr getLHS() {
    return lhs;
  }

  ExprTreePtr getRHS() {
    return rhs;
  }

  bool referencesTable(string alias) {
    return lhs->referencesTable(alias) || rhs->referencesTable(alias);
  }

  bool referencesAtt(string alias, string attNameToFind) {
    return lhs->referencesAtt(alias, attNameToFind) || rhs->referencesAtt(alias, attNameToFind);
  }

  ~OrOp() {}
};

class EqOp : public ExprTree {

private:
  ExprTreePtr lhs;
  ExprTreePtr rhs;

public:
  EqOp(ExprTreePtr lhsIn, ExprTreePtr rhsIn) {
    lhs = lhsIn;
    rhs = rhsIn;
  }

  bool isComp() {
    return true;
  }

  bool isEq() {
    return true;
  }

  string getId() {
    string res = lhs->getId();
    if (res != "")
      return res;
    else
      return rhs->getId();
  }

  ExprTreePtr getLHS() {
    return lhs;
  }

  ExprTreePtr getRHS() {
    return rhs;
  }

  string toString() {
    return "== (" + lhs->toString() + ", " + rhs->toString() + ")";
  }

  bool referencesTable(string alias) {
    return lhs->referencesTable(alias) || rhs->referencesTable(alias);
  }

  bool referencesAtt(string alias, string attNameToFind) {
    return lhs->referencesAtt(alias, attNameToFind) || rhs->referencesAtt(alias, attNameToFind);
  }

  ~EqOp() {}
};

class NotOp : public ExprTree {

private:
  ExprTreePtr child;

public:
  NotOp(ExprTreePtr childIn) {
    child = childIn;
  }

  bool isComp() {
    return true;
  }

  ExprTreePtr getChild() {
    return child;
  }

  string getId() {
    return child->getId();
  }

  string toString() {
    return "!(" + child->toString() + ")";
  }

  bool referencesTable(string alias) {
    return child->referencesTable(alias);
  }

  bool referencesAtt(string alias, string attNameToFind) {
    return child->referencesAtt(alias, attNameToFind);
  }

  ~NotOp() {}
};

class SumOp : public ExprTree {

private:
  ExprTreePtr child;

public:
  SumOp(ExprTreePtr childIn) {
    child = childIn;
  }

  string toString() {
    return "sum(" + child->toString() + ")";
  }

  bool isSum() {
    return true;
  }

  bool referencesTable(string alias) {
    return child->referencesTable(alias);
  }

  bool referencesAtt(string alias, string attNameToFind) {
    return child->referencesAtt(alias, attNameToFind);
  }

  ~SumOp() {}

  ExprTreePtr getChild() {
    return child;
  }
};

class AvgOp : public ExprTree {

private:
  ExprTreePtr child;

public:
  AvgOp(ExprTreePtr childIn) {
    child = childIn;
  }

  string toString() {
    return "avg(" + child->toString() + ")";
  }

  bool isAvg() {
    return true;
  }

  bool referencesTable(string alias) {
    return child->referencesTable(alias);
  }

  bool referencesAtt(string alias, string attNameToFind) {
    return child->referencesAtt(alias, attNameToFind);
  }

  ~AvgOp() {}

  ExprTreePtr getChild() {
    return child;
  }
};

#endif

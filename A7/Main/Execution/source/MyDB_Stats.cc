
#ifndef STATS_CC
#define STATS_CC

#include "MyDB_Stats.h"

MyDB_Stats :: MyDB_Stats () {
	tupleCount = 0;
}

double MyDB_Stats :: getTupleCount () {
	return tupleCount;
}

void MyDB_Stats :: print () {
	cout << "tuples: " << tupleCount << "\n";
	for (auto a : allAtts) {
		cout << "(" << a.first << ", " << a.second << ") ";
	}
	cout << "\n";
}

double MyDB_Stats :: getAttVals (string forMe) {
	for (auto a : allAtts) {
		if (a.second == forMe)
			return a.first;
	}
	return 0;
}

MyDB_Stats :: MyDB_Stats (MyDB_TablePtr inTable, string aliasName) {
	for (auto a : inTable->getSchema ()->getAtts ()) {
		allAtts.push_back (make_pair (inTable->getDistinctValues (a.first), aliasName + "_" + a.first));
	}
	tupleCount = inTable->getTupleCount ();
}
	
MyDB_StatsPtr MyDB_Stats :: costSelection (vector <ExprTreePtr> &allDisjunctions) {

	// if there are no expressions, we are done
	if (allDisjunctions.size () == 0) {
		MyDB_StatsPtr returnVal = make_shared <MyDB_Stats> ();
		*returnVal = *this;
		return returnVal;
	}

	// pop the last expression
	ExprTreePtr myExpr = allDisjunctions.back ();
	allDisjunctions.pop_back ();
	ExprTreePtr myActualExpr = myExpr;
	MyDB_StatsPtr temp = make_shared <MyDB_Stats> ();	

	// see if it's an OR
	if (myExpr->isOr ()) {
		// get the RHS, which is the comparison
		myActualExpr = myExpr->getRHS ();	
	} else {
		myActualExpr = myExpr;
	}

	cout << "expession is: " << myActualExpr->toString () << "\n";

	// just verify that it's a comparison
	if (!myActualExpr->isComp ())
		cout << "This is a problem.  I expected a comparison while costing,\n";

	// equality check
	if (myActualExpr->isEq ()) {

		if (myActualExpr->getLHS ()->isId () || myActualExpr->getRHS ()->isId ()) {
			string ID;
			if (myActualExpr->getLHS ()->isId ()) {
				ID = myActualExpr->getLHS ()->getId ();
			} else {
				ID = myActualExpr->getRHS ()->getId ();
			}
			temp->tupleCount = tupleCount / getAttVals (ID);
			for (auto &b: allAtts) {
				pair <double, string> newOne;
				if (b.second == ID) {
					newOne.first = 1;
				} else {
					newOne.first = b.first;
					if (newOne.first > temp->tupleCount) {
						newOne.first = temp->tupleCount;
					}
				}
				newOne.second = b.second;
				temp->allAtts.push_back (newOne);	
			}
		} else {
			temp->tupleCount = tupleCount;
			temp->allAtts = allAtts;
		}

	} else if (myActualExpr->isLTGT ()) {

		temp->tupleCount = tupleCount / 3;
		for (auto &b: allAtts) {
			pair <double, string> newOne;
			newOne.first = b.first;
			if (newOne.first > temp->tupleCount) {
				newOne.first = temp->tupleCount;
			}
			newOne.second = b.second;
			temp->allAtts.push_back (newOne);	
		}
			
	} else if (myActualExpr->isNotEq ()) {

		if (myActualExpr->getLHS ()->isId () || myActualExpr->getRHS ()->isId ()) {
			string ID;
			if (myActualExpr->getLHS ()->isId ()) {
				ID = myActualExpr->getLHS ()->getId ();
			} else {
				ID = myActualExpr->getRHS ()->getId ();
			}
			temp->tupleCount = tupleCount - tupleCount / getAttVals (ID);
			for (auto &b: allAtts) {
				pair <double, string> newOne;
				if (b.second == ID) {
					newOne.first = b.first - 1;
				} else {
					newOne.first = b.first;
					if (newOne.first > temp->tupleCount) {
						newOne.first = temp->tupleCount;
					}
				}
				newOne.second = b.second;
				temp->allAtts.push_back (newOne);	
			}	
		} else {
			temp->tupleCount = tupleCount;
			temp->allAtts = allAtts;
		}
	} else {
		temp->tupleCount = tupleCount;
		temp->allAtts = allAtts;
	}
	
	cout << "in tuples was " << tupleCount << " and out is " << temp->tupleCount << "\n";

	auto res = temp->costSelection (allDisjunctions);
	allDisjunctions.push_back (myExpr);
	cout << "output stats are ";
	res->print ();
	return res;

	
}


MyDB_StatsPtr MyDB_Stats ::  costJoin (vector <ExprTreePtr> &allDisjunctions, MyDB_StatsPtr RHS) {

	this->print ();
	RHS->print ();

	// search through the clauses to find an equality check
	int pos = 0;
	MyDB_StatsPtr temp = make_shared <MyDB_Stats> ();	
	for (auto a: allDisjunctions) {
		if (a->isEq ()) {
			// we found an equals! check that each side comes from a different table
			if (a->getLHS ()->isId () && a->getRHS ()->isId ()) {
			 	cout << a->getLHS ()->getId () << "\n";
			 	cout << a->getRHS ()->getId () << "\n";
				auto lhsAttCnt = getAttVals (a->getLHS ()->getId ());
				auto lhsTupleCnt = getTupleCount ();
				auto rhsAttCnt = RHS->getAttVals (a->getRHS ()->getId ());
				auto rhsTupleCnt = RHS->getTupleCount ();

				// we may have to switch LHS and RHS
				if (lhsAttCnt < 0.0000001) {
					lhsAttCnt = RHS->getAttVals (a->getLHS ()->getId ());
					lhsTupleCnt = RHS->getTupleCount ();
					rhsAttCnt = getAttVals (a->getRHS ()->getId ());
					rhsTupleCnt = getTupleCount ();
				}

				if (lhsAttCnt < 0.00000001)
					cout << "Bad error!!  I could not find att val " << a->getLHS ()->getId () << " during costing.\n";
				
				cout << "att counts were " << lhsAttCnt << ", " << rhsAttCnt << "\n";

				// see if we were able to get a join predicate
				if (lhsAttCnt > 0 && rhsAttCnt > 0) {

					// we did!!
					temp->tupleCount = (lhsTupleCnt / lhsAttCnt) * (rhsTupleCnt / rhsAttCnt);
					double minAttCnt = lhsAttCnt;
					if (minAttCnt > rhsAttCnt)
						minAttCnt = rhsAttCnt;
					temp->tupleCount *= minAttCnt;

					// and so now we create a new set of stats
					for (auto &b: allAtts) {
						pair <double, string> newOne;
						cout << "[" << a->getLHS ()->getId () << " " << a->getRHS ()->getId () << "] ";
						if (b.second == a->getLHS ()->getId () || b.second == a->getRHS ()->getId ()) {
							newOne.first = minAttCnt;
						} else {
							newOne.first = b.first;
							if (newOne.first > temp->tupleCount)
								newOne.first = temp->tupleCount;
						}
						newOne.second = b.second;
						cout << "(" << newOne.first << ", " << newOne.second << ") ";
						temp->allAtts.push_back (newOne);	
					}	
					for (auto &b: RHS->allAtts) {
						pair <double, string> newOne;
						cout << "[" << a->getLHS ()->getId () << " " << a->getRHS ()->getId () << "] ";
						if (b.second == a->getLHS ()->getId () || b.second == a->getRHS ()->getId ()) {
							newOne.first = minAttCnt;
							cout << "! ";
						} else {
							newOne.first = b.first;
							if (newOne.first > temp->tupleCount)
								newOne.first = temp->tupleCount;
						}
						newOne.second = b.second;
						cout << "(" << newOne.first << ", " << newOne.second << ") ";
						temp->allAtts.push_back (newOne);	
					}	

					cout << "\n";
				}
			}

			// now, create a new predicate
			vector <ExprTreePtr> allDisjunctionsNew;
			for (int i = 0; i < allDisjunctions.size (); i++) {
				if (i != pos)
					allDisjunctionsNew.push_back (allDisjunctions[i]);
			}

			// and cost it
			return temp->costSelection (allDisjunctionsNew);
		}
		pos++;
	}
	
	// if we got here, it's just a cross product (there is not a join predicate)
	// and so now we create a new set of stats
	temp->tupleCount = tupleCount * RHS->tupleCount;
	for (auto &b: allAtts) {
		pair <double, string> newOne;
		newOne.first = b.first;
		newOne.second = b.second;
		temp->allAtts.push_back (newOne);	
	}	
	for (auto &b: RHS->allAtts) {
		pair <double, string> newOne;
		newOne.first = b.first;
		newOne.second = b.second;
		temp->allAtts.push_back (newOne);	
	}	
	
	return temp->costSelection (allDisjunctions);

}

#endif

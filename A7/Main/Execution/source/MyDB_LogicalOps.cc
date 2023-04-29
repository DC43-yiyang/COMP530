
#ifndef LOG_OP_CC
#define LOG_OP_CC

#include "MyDB_LogicalOps.h"
#include "Aggregate.h"
#include "MyDB_BPlusTreeReaderWriter.h"
#include "RegularSelection.h"
#include "ScanJoin.h"
#include "SortMergeJoin.h"
#include <algorithm>
#include <cassert>
#include <sstream>

// fill this out!  This should actually run the aggregation via an appropriate RelOp, and then it is going to
// have to unscramble the output attributes and compute exprsToCompute using an execution of the RegularSelection
// operation (why?  Note that the aggregate always outputs all of the grouping atts followed by the agg atts.
// After, a selection is required to compute the final set of aggregate expressions)
//
// Note that after the left and right hand sides have been executed, the temporary tables associated with the two
// sides should be deleted (via a kill to killFile () on the buffer manager)
tuple<MyDB_TableReaderWriterPtr, size_t> LogicalAggregate::execute()
{
    MyDB_TableReaderWriterPtr input;
    size_t count;
    tie(input, count) = inputOp->execute();
    auto bufMgr = input->getBufferMgr();
    auto output = make_shared<MyDB_TableReaderWriter>(outputSpec, bufMgr);

    vector<pair<MyDB_AggType, string>> aggsToCompute;
    for (const auto &expr : exprsToCompute)
    {
        if (expr->isAvg())
        {
            aggsToCompute.emplace_back(MyDB_AggType::avg, expr->getChild()->toString());
        }
        else if (expr->isSum())
        {
            aggsToCompute.emplace_back(MyDB_AggType::sum, expr->getChild()->toString());
        }
        else
        {
            cout << "error" << endl;
            exit(1);
        }
    }

    vector<string> myGroupings;
    for (const auto &groupExpr : groupings)
    {
        myGroupings.push_back(groupExpr->toString());
    }

    Aggregate myPlan(input, output, aggsToCompute, myGroupings, "bool[true]");
    size_t cnt = myPlan.run();

    cout << output->getTable()->getName() << ": " << cnt << endl;
    bufMgr->killTable(input->getTable());

    return make_tuple(output, cnt);
}

// we don't really count the cost of the aggregate, so cost its subplan and return that
pair<double, MyDB_StatsPtr> LogicalAggregate ::cost()
{
    return inputOp->cost();
}

// this costs the entire query plan with the join at the top, returning the compute set of statistics for
// the output.  Note that it recursively costs the left and then the right, before using the statistics from
// the left and the right to cost the join itself
pair<double, MyDB_StatsPtr> LogicalJoin ::cost()
{
    auto left = leftInputOp->cost();
    auto right = rightInputOp->cost();
    MyDB_StatsPtr outputStats = left.second->costJoin(outputSelectionPredicate, right.second);
    return make_pair(left.first + right.first + outputStats->getTupleCount(), outputStats);
}

string joinExprs(vector<ExprTreePtr> exprs)
{
    int i = 0;
    string res{};
    for (auto &expr : exprs)
    {
        if (i == 0)
        {
            res = expr->toString();
        }
        else
        {
            res = "&& (" + res + ", " + expr->toString() + ")";
        }
        i++;
    }
    return res;
}

// Fill this out!  This should recursively execute the left hand side, and then the right hand side, and then
// it should heuristically choose whether to do a scan join or a sort-merge join (if it chooses a scan join, it
// should use a heuristic to choose which input is to be hashed and which is to be scanned), and execute the join.
// Note that after the left and right hand sides have been executed, the temporary tables associated with the two
// sides should be deleted (via a kill to killFile () on the buffer manager)
tuple<MyDB_TableReaderWriterPtr, size_t> LogicalJoin ::execute()
{
    // TODO: require thread-safe memory pool to enable concurrency
    //  or allocate memory per thread for better performance

    bool leftInSelect = any_of(exprsToCompute.begin(), exprsToCompute.end(), [this](const ExprTreePtr &expr)
                               { return any_of(leftTables.begin(), leftTables.end(), [&expr](const pair<string, string> &name)
                                               { return expr->referencesTable(name.second); }); });

    bool rightInSelect = any_of(exprsToCompute.begin(), exprsToCompute.end(), [this](const ExprTreePtr &expr)
                                { return any_of(rightTables.begin(), rightTables.end(), [&expr](const pair<string, string> &name)
                                                { return expr->referencesTable(name.second); }); });

    if (outputSelectionPredicate.empty())
    {
        // no join condition
        if (leftInSelect && !rightInSelect)
            return leftInputOp->execute();
        if (rightInSelect && !leftInSelect)
            return rightInputOp->execute();
        else
            outputSelectionPredicate.push_back(make_shared<EqOp>(make_shared<BoolLiteral>(true), make_shared<BoolLiteral>(true)));
    }

    MyDB_TableReaderWriterPtr leftInput, rightInput;
    size_t leftCount, rightCount;
    tie(leftInput, leftCount) = leftInputOp->execute();
    tie(rightInput, rightCount) = rightInputOp->execute();

    auto bufMgr = leftInput->getBufferMgr();

    auto output = make_shared<MyDB_TableReaderWriter>(outputSpec, bufMgr);

    string finalPreds = joinExprs(outputSelectionPredicate);
    vector<string> projections;
    for_each(exprsToCompute.begin(), exprsToCompute.end(), [&projections](const ExprTreePtr &expr)
             {
    // TODO: handle nested expr
    projections.push_back(expr->toString()); });

    size_t cnt;
    auto maxPossiblePages = bufMgr->getMaxPossiblePages();
    auto leftAtts = leftInput->getTable()->getSchema()->getAtts();
    
    pair<string, string> equalityCheck;
    for (const auto &expr : outputSelectionPredicate)
    {
        if (expr->getLHS() && expr->getRHS())
        {
            auto pred1 = expr->getLHS()->toString();
            auto pred2 = expr->getRHS()->toString();
            if (any_of(leftAtts.begin(), leftAtts.end(), [&pred1](const pair<string, MyDB_AttTypePtr> &p)
                        { return pred1.find(p.first) != string::npos; }))
                equalityCheck = make_pair(pred1, pred2);
            else
                equalityCheck = make_pair(pred2, pred1);
            break;
        }
    }
    auto pred1 = outputSelectionPredicate[0]->getLHS()->toString();
    auto pred2 = outputSelectionPredicate[0]->getRHS()->toString();

    SortMergeJoin joinPlan(leftInput, rightInput, output,
                            finalPreds, projections, equalityCheck,
                            "bool[true]", "bool[true]");

    cnt = joinPlan.run();

    cout << output->getTable()->getName() << ": " << cnt << endl;

    bufMgr->killTable(leftInput->getTable());
    bufMgr->killTable(rightInput->getTable());

    return make_tuple(output, cnt);
}

// this costs the table scan returning the compute set of statistics for the output
pair<double, MyDB_StatsPtr> LogicalTableScan ::cost()
{
    MyDB_StatsPtr returnVal = inputStats->costSelection(selectionPred);
    return make_pair(returnVal->getTupleCount(), returnVal);
}

void replaceAll(string &s, string const &toReplace, string const &replaceWith)
{
    ostringstream oss;
    size_t pos = 0;
    size_t prevPos = pos;

    while (true)
    {
        prevPos = pos;
        pos = s.find(toReplace, pos);
        if (pos == string::npos)
            break;
        oss << s.substr(prevPos, pos - prevPos);
        oss << replaceWith;
        pos += toReplace.size();
    }

    oss << s.substr(prevPos);
    s = oss.str();
}

// fill this out!  This should heuristically choose whether to use a B+-Tree (if appropriate) or just a regular
// table scan, and then execute the table scan using a relational selection.  Note that a desirable optimization
// is to somehow set things up so that if a B+-Tree is NOT used, that the table scan does not actually do anything,
// and the selection predicate is handled at the level of the parent (by filtering, for example, the data that is
// input into a join)

tuple<MyDB_TableReaderWriterPtr, size_t> LogicalTableScan ::execute() {
    auto output = make_shared<MyDB_TableReaderWriter>(outputSpec, inputSpec->getBufferMgr());
    auto temp = inputSpec->getEmptyRecord();
    auto outputRec = output->getEmptyRecord();

    MyDB_RecordIteratorAltPtr recIter = nullptr;
    bool useBPlusTree = inputSpec->getTable()->getFileType() == "bplustree";
    if (useBPlusTree) {
        auto inputTable = dynamic_pointer_cast<MyDB_BPlusTreeReaderWriter>(inputSpec);
        auto att = inputSpec->getTable()->getSortAtt();
        auto attExprStr = "[" + tableAlias + "_" + att + "]";

        auto iter = selectionPred.begin();
        for (; iter != selectionPred.end(); iter++) {
            auto expr = *iter;
            if (expr->isEq()) {
                auto lhs = expr->getLHS();
                auto rhs = expr->getRHS();

                if (lhs->isId() && lhs->toString() == attExprStr && rhs->getLiteral()) {
                    recIter = inputTable->getRangeIteratorAlt(rhs->getLiteral(), rhs->getLiteral());
                    break;
                } else if (rhs->isId() && rhs->toString() == attExprStr && lhs->getLiteral()) {
                    recIter = inputTable->getRangeIteratorAlt(lhs->getLiteral(), lhs->getLiteral());
                    break;
                }
            }
        }
        if (iter != selectionPred.end()) {
            selectionPred.erase(iter);
        }
    }

    auto pred = selectionPred.empty() ? "bool[true]" : joinExprs(selectionPred);
    for (const auto &att : inputSpec->getTable()->getSchema()->getAtts()) {
        replaceAll(pred, tableAlias + "_" + att.first, att.first);
    }
    auto predFunc = temp->compileComputation(pred);

    vector<func> finalComputations;
    for (string s : exprsToCompute) {
        finalComputations.push_back(temp->compileComputation(s));
    }

    size_t counter = 0;
    if (recIter) {
        while (recIter->advance()) {
            recIter->getCurrent(temp);
            if (!predFunc()->toBool()) {
                continue;
            }
            counter++;
            int i = 0;
            for (auto &f : finalComputations) {
                outputRec->getAtt(i++)->set(f());
            }

            outputRec->recordContentHasChanged();
            output->append(outputRec);
        }
    } else {
        RegularSelection myPlan(inputSpec, output, pred, exprsToCompute);
        counter = myPlan.run();
    }
    
    cout << output->getTable()->getName() << ": " << counter << endl;
    return make_tuple(output, counter);
}


// this costs the table scan returning the compute set of statistics for the output
pair<double, MyDB_StatsPtr> LogicalFinalScan::cost()
{
    return inputOp->cost();
}

tuple<MyDB_TableReaderWriterPtr, size_t> LogicalFinalScan::execute()
{
    MyDB_TableReaderWriterPtr input;
    size_t count;
    tie(input, count) = inputOp->execute();
    auto bufMgr = input->getBufferMgr();
    auto output = make_shared<MyDB_TableReaderWriter>(outputSpec, bufMgr);

    RegularSelection myPlan(input, output, "bool[true]", exprsToCompute);

    size_t cnt = myPlan.run();
    cout << output->getTable()->getName() << ": " << cnt << endl;

    bufMgr->killTable(input->getTable());

    return make_tuple(output, cnt);
}

#endif

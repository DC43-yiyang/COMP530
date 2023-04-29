
#ifndef SFW_QUERY_CC
#define SFW_QUERY_CC

#include "ParserTypes.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>

#define DEBUG_MSG(str)                                                                      \
    do                                                                                      \
    {                                                                                       \
        auto now = std::chrono::system_clock::now();                                        \
        auto now_c = std::chrono::system_clock::to_time_t(now);                             \
        auto now_tm = std::localtime(&now_c);                                               \
        std::cerr << std::put_time(now_tm, "%T") << " (Houston Time) ";                     \
        std::cerr << "DEBUG: " << __FILE__ << "(" << __LINE__ << "): " << str << std::endl; \
    } while (false)

vector<pair<unordered_set<int>, unordered_set<int>>> generateSubsets(int n)
{
    assert(n >= 2);
    int count = (int)pow(2, n);
    vector<pair<unordered_set<int>, unordered_set<int>>> allSubsets;
    for (int i = 0; i < count; i++)
    {
        unordered_set<int> leftSet;
        unordered_set<int> rightSet;

        for (int j = 0; j < n; j++)
            if (i & (1 << j))
                leftSet.insert(j);
            else
                rightSet.insert(j);
        if (leftSet.size() == rightSet.size() && find(allSubsets.begin(), allSubsets.end(), make_pair(rightSet, leftSet)) != allSubsets.end())
            continue;
        if (leftSet.size() >= rightSet.size() && (int)rightSet.size() > 0)
            allSubsets.emplace_back(leftSet, rightSet);
    }
    return allSubsets;
};

// TODO: memoization
LogicalOpPtr SFWQuery ::optimize(vector<ExprTreePtr> valuesToSelect,
                                 vector<pair<string, string>> tables,
                                 vector<ExprTreePtr> cnf,
                                 int totTables)
{
    LogicalOpPtr bestOp = nullptr;
    double bestCost = numeric_limits<double>::max();
    if (tables.size() == 1)
    {
        // no opt
        string tableName = tables[0].first;
        string tableAlias = tables[0].second;
        auto inputTable = allTables[tableName];
        auto inputTableRW = allTableReaderWriters[tableName];

        vector<string> myExprs;

        MyDB_SchemaPtr outputScheme = make_shared<MyDB_Schema>();
        for (const auto &expr : valuesToSelect)
        {
            for (const auto &inputAtt : inputTable->getSchema()->getAtts())
            {
                if (expr->referencesAtt(tableAlias, inputAtt.first))
                {
                    outputScheme->getAtts().emplace_back(tableAlias + "_" + inputAtt.first, inputAtt.second);
                    myExprs.emplace_back("[" + inputAtt.first + "]");
                }
            }
        }

        auto myOp = make_shared<LogicalTableScan>(
            inputTableRW,
            make_shared<MyDB_Table>(tableAlias + "_scan", tableAlias + "_scan.bin", outputScheme),
            make_shared<MyDB_Stats>(inputTable, tableAlias),
            cnf, myExprs, tableAlias);

        if (totTables == 1)
            finalScheme = outputScheme;
        return myOp;
    }
    else
    {

        auto allPartitions = generateSubsets((int)tables.size());
        for (const auto &partitions : allPartitions)
        {
            vector<pair<string, string>> leftTables;
            vector<pair<string, string>> rightTables;
            for (int i : partitions.first)
                leftTables.push_back(tables[i]);
            for (int i : partitions.second)
                rightTables.push_back(tables[i]);

            vector<ExprTreePtr> leftExprs, rightExprs, topExprs;
            vector<ExprTreePtr> leftCNF, rightCNF, topCNF;
            MyDB_SchemaPtr leftSchema = make_shared<MyDB_Schema>();
            MyDB_SchemaPtr rightSchema = make_shared<MyDB_Schema>();
            MyDB_SchemaPtr topSchema = make_shared<MyDB_Schema>();

            for (auto c : cnf)
            {
                bool inLeft = any_of(leftTables.begin(), leftTables.end(), [&c](const pair<string, string> &table)
                                     { return c->referencesTable(table.second); });
                bool inRight = any_of(rightTables.begin(), rightTables.end(), [&c](const pair<string, string> &table)
                                      { return c->referencesTable(table.second); });
                if (inLeft && inRight)
                    topCNF.push_back(c);
                else if (inLeft)
                    leftCNF.push_back(c);
                else
                    rightCNF.push_back(c);
            }

            for (auto leftTableName : leftTables)
            {
                auto leftTable = allTables[leftTableName.first];
                for (auto att : leftTable->getSchema()->getAtts())
                {

                    bool inSelect = any_of(valuesToSelect.begin(), valuesToSelect.end(), [&att, &leftTableName](const ExprTreePtr &exp)
                                           { return exp->referencesAtt(leftTableName.second, att.first); });
                    bool inCNF = any_of(cnf.begin(), cnf.end(), [&att, &leftTableName](const ExprTreePtr &exp)
                                        { return exp->referencesAtt(leftTableName.second, att.first); });

                    ExprTreePtr idExp = make_shared<Identifier>(strdup(leftTableName.second.c_str()), strdup(att.first.c_str()));

                    if (inSelect || inCNF)
                    {
                        leftSchema->getAtts().emplace_back(leftTableName.second + "_" + att.first, att.second);
                        leftExprs.push_back(idExp);
                    }

                    if (inSelect)
                    {
                        topSchema->getAtts().emplace_back(leftTableName.second + "_" + att.first, att.second);
                        topExprs.push_back(idExp);
                    }
                }
            }

            for (auto rightTableName : rightTables)
            {
                auto rightTable = allTables[rightTableName.first];
                for (auto att : rightTable->getSchema()->getAtts())
                {

                    bool inSelect = any_of(valuesToSelect.begin(), valuesToSelect.end(), [&att, &rightTableName](const ExprTreePtr &exp)
                                           { return exp->referencesAtt(rightTableName.second, att.first); });
                    bool inCNF = any_of(cnf.begin(), cnf.end(), [&att, &rightTableName](const ExprTreePtr &exp)
                                        { return exp->referencesAtt(rightTableName.second, att.first); });

                    ExprTreePtr idExp = make_shared<Identifier>(strdup(rightTableName.second.c_str()), strdup(att.first.c_str()));

                    if (inSelect || inCNF)
                    {
                        rightSchema->getAtts().emplace_back(rightTableName.second + "_" + att.first, att.second);
                        rightExprs.push_back(idExp);
                    }

                    if (inSelect)
                    {
                        topSchema->getAtts().emplace_back(rightTableName.second + "_" + att.first, att.second);
                        topExprs.push_back(idExp);
                    }
                }
            }

            LogicalOpPtr leftOp = optimize(leftExprs, leftTables, leftCNF, totTables);
            LogicalOpPtr rightOp = optimize(rightExprs, rightTables, rightCNF, totTables);

            string leftTableNames = leftTables[0].second;
            for_each(leftTables.begin() + 1, leftTables.end(), [&leftTableNames](const pair<string, string> &p)
                     { leftTableNames += ("_" + p.second); });

            string rightTableNames = rightTables[0].second;
            for_each(rightTables.begin() + 1, rightTables.end(), [&rightTableNames](const pair<string, string> &p)
                     { rightTableNames += ("_" + p.second); });

            string outputTableName = leftTableNames + "_JOIN_" + rightTableNames;

            LogicalOpPtr myOp = make_shared<LogicalJoin>(
                leftOp, rightOp,
                make_shared<MyDB_Table>(outputTableName, outputTableName + ".bin", topSchema),
                topCNF, topExprs, leftTables, rightTables);

            auto cost = myOp->cost();

            if (cost.first < bestCost)
            {
                bestCost = cost.first;
                bestOp = myOp;
                if (tables.size() == totTables)
                    finalScheme = topSchema;
            }
        }
        return bestOp;
    }
}

void replace(string &s, string const &toReplace, string const &replaceWith)
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

// builds and optimizes a logical query plan for a SFW query, returning the logical query plan
//
// note that this implementation only works for two-table queries that do not have an aggregation
//
LogicalOpPtr SFWQuery ::buildLogicalQueryPlan(map<string, MyDB_TablePtr> &allTables, map<string, MyDB_TableReaderWriterPtr> &allTableReaderWriters)
{

    cout << "groupingClauses: \n"
         << endl;
    for (auto expr : groupingClauses)
    {
        cout << expr->toString() << endl;
    }
    cout << "valuesToSelect: \n"
         << endl;
    for (auto expr : valuesToSelect)
    {
        cout << expr->toString() << endl;
    }
    cout << "allDisjunctions: \n"
         << endl;
    for (auto expr : allDisjunctions)
    {
        cout << expr->toString() << endl;
    }

    this->allTables = allTables;
    this->allTableReaderWriters = allTableReaderWriters;

    // remove unused tables
    for (auto iter = tablesToProcess.begin(); iter != tablesToProcess.end();)
    {
        auto tableAlias = (*iter).second;
        bool isUsed = any_of(allDisjunctions.begin(), allDisjunctions.end(), [&tableAlias](const ExprTreePtr &expr)
                             { return expr->referencesTable(tableAlias); }) ||
                      any_of(valuesToSelect.begin(), valuesToSelect.end(), [&tableAlias](const ExprTreePtr &expr)
                             { return expr->referencesTable(tableAlias); }) ||
                      any_of(groupingClauses.begin(), groupingClauses.end(), [&tableAlias](const ExprTreePtr &expr)
                             { return expr->referencesTable(tableAlias); });

        if (isUsed)
            ++iter;
        else
            tablesToProcess.erase(iter);
    }

    LogicalOpPtr myOp = optimize(valuesToSelect, tablesToProcess, allDisjunctions, tablesToProcess.size());
    MyDB_SchemaPtr myScheme = finalScheme;
    MyDB_Record myRecord(myScheme);

    vector<string> finalSelections;

    vector<ExprTreePtr> aggSelections;

    auto aggScheme = make_shared<MyDB_Schema>();

    bool needAgg = false;

    string aggTableAlias = "agg";

    // TODO: check if this is correct
    // extract expressions from group by clauses
    unordered_map<string, string> renameTable;
    int attId = 0;
    for (const auto &groupExpr : groupingClauses)
    {
        DEBUG_MSG("groupExpr: " << groupExpr->toString());
        needAgg = true;
        string newName = "group_" + to_string(attId);
        aggScheme->getAtts().emplace_back(newName, myRecord.getType(groupExpr->toString()));
        renameTable[groupExpr->toString()] = newName;
        attId++;
    }

    // extract identifier from group by clauses
    for (const auto &groupExpr : groupingClauses)
    {
        needAgg = true;
        for (const auto &idExpr : groupExpr->getIdentifiers())
        {
            string idExprStr = idExpr->toString();
            if (renameTable.find(idExprStr) == renameTable.end())
            {
                groupingClauses.push_back(idExpr);
                assert(idExprStr.size() >= 2);
                string newName = idExprStr.substr(1, idExprStr.size() - 2);
                aggScheme->getAtts().emplace_back(newName, myRecord.getType(idExpr->toString()));
                renameTable[idExprStr] = newName;
                attId++;
            }
        }
    }

    // extract aggregate expressions from group by select clauses
    for (auto &expr : valuesToSelect)
    {
        auto aggExprs = expr->getAggExprs();
        if (!aggExprs.empty())
        {
            needAgg = true;
            for (const auto &aggExpr : aggExprs)
            {
                string aggExprStr = aggExpr->toString();
                if (renameTable.find(aggExprStr) == renameTable.end())
                {
                    aggSelections.push_back(aggExpr);
                    string newName = aggExprStr.substr(0, 3) + "_" + to_string(attId);
                    aggScheme->getAtts().emplace_back(newName, myRecord.getType(aggExpr->toString()));
                    renameTable[aggExprStr] = newName;
                    attId++;
                }
            }
        }
    }

    vector<string> allRenamedKeys;
    for (const auto &p : renameTable)
        allRenamedKeys.push_back(p.first);

    for (auto &expr : valuesToSelect)
    {
        string exprStr = expr->toString();
        for (const auto &renamedKeys : allRenamedKeys)
        {
            replace(exprStr, renamedKeys, "[" + renameTable[renamedKeys] + "]");
        }
        finalSelections.push_back(exprStr);
        DEBUG_MSG("finalSelections: " << exprStr);
    }

    bool needFinalSelection = true;
    if (needAgg)
    {
        bool allIdentical = true;
        if (finalSelections.size() == aggScheme->getAtts().size())
        {
            for (int i = 0; i < finalSelections.size(); i++)
            {
                if (finalSelections[i] != "[" + aggScheme->getAtts()[i].first + "]")
                {
                    allIdentical = false;
                    break;
                }
            }
        }
        else
        {
            allIdentical = false;
        }

        needFinalSelection = !allIdentical;
    }
    else
    {

        bool allIdentical = true;
        if (valuesToSelect.size() == myScheme->getAtts().size())
        {
            for (int i = 0; i < valuesToSelect.size(); i++)
            {
                if (valuesToSelect[i]->toString() != "[" + myScheme->getAtts()[i].first + "]")
                {
                    allIdentical = false;
                    break;
                }
            }
        }
        else
        {
            allIdentical = false;
        }
        needFinalSelection = !allIdentical;
    }

    if (!needAgg && !needFinalSelection)
        return myOp;

    LogicalOpPtr myAgg;

    if (needAgg)
    {
        myAgg = make_shared<LogicalAggregate>(
            myOp,
            make_shared<MyDB_Table>("agg", "agg.bin", aggScheme),
            aggSelections,
            groupingClauses);
    }

    if (!needFinalSelection)
        return myAgg;

    LogicalOpPtr final;

    string finalTableAlias = "final";
    LogicalOpPtr finalInput = needAgg ? myAgg : myOp;
    MyDB_SchemaPtr finalScheme = make_shared<MyDB_Schema>();
    attId = 0;

    if (needAgg)
    {
        MyDB_Record intermediateRecord(aggScheme);
        for (const auto &exprStr : finalSelections)
        {
            finalScheme->getAtts().emplace_back(finalTableAlias + "_" + to_string(attId), intermediateRecord.getType(exprStr));
            attId++;
        }
    }
    else
    {
        for (const auto &expr : valuesToSelect)
        {
            finalScheme->getAtts().emplace_back(finalTableAlias + "_" + to_string(attId), myRecord.getType(expr->toString()));
            attId++;
        }
    }

    final = make_shared<LogicalFinalScan>(
        finalInput,
        make_shared<MyDB_Table>("final", "final.bin", finalScheme),
        finalSelections,
        finalTableAlias);
    return final;
}

void SFWQuery ::print()
{
    cout << "Selecting the following:\n";
    for (auto a : valuesToSelect)
    {
        cout << "\t" << a->toString() << "\n";
    }
    cout << "From the following:\n";
    for (auto a : tablesToProcess)
    {
        cout << "\t" << a.first << " AS " << a.second << "\n";
    }
    cout << "Where the following are true:\n";
    for (auto a : allDisjunctions)
    {
        cout << "\t" << a->toString() << "\n";
    }
    cout << "Group using:\n";
    for (auto a : groupingClauses)
    {
        cout << "\t" << a->toString() << "\n";
    }
}

SFWQuery ::SFWQuery(struct ValueList *selectClause, struct FromList *fromClause,
                    struct CNF *cnf, struct ValueList *grouping)
{
    valuesToSelect = selectClause->valuesToCompute;
    tablesToProcess = fromClause->aliases;
    allDisjunctions = cnf->disjunctions;
    groupingClauses = grouping->valuesToCompute;
}

SFWQuery ::SFWQuery(struct ValueList *selectClause, struct FromList *fromClause,
                    struct CNF *cnf)
{
    valuesToSelect = selectClause->valuesToCompute;
    tablesToProcess = fromClause->aliases;
    allDisjunctions = cnf->disjunctions;
}

SFWQuery ::SFWQuery(struct ValueList *selectClause, struct FromList *fromClause)
{
    valuesToSelect = selectClause->valuesToCompute;
    tablesToProcess = fromClause->aliases;
    allDisjunctions.push_back(make_shared<BoolLiteral>(true));
}

#endif

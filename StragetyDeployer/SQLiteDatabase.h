#pragma once

#include <string>
#include <map>
#include <vector>
#include "sqlite3.h"

using namespace std;

#define SQLITEDATABASE_ERROR -1
#define SQLITEDATABASE_OK 0

////SQLiteResultSet////
class SQLiteResultSet
{
private:
    sqlite3_stmt *pstmt = nullptr;

private:
    SQLiteResultSet();

public:
    SQLiteResultSet(const SQLiteResultSet &other) = delete;
    SQLiteResultSet(SQLiteResultSet &&other);
    ~SQLiteResultSet();
    bool next();
    int columnCount();
    ///get record attribute by column, nCol starts from 0////
    string columnName(int nCol);
    int getInteger(int nCol);
    long long getLonglong(int nCol);
    double getDouble(int nCol);
    string getString(int nCol);
    //////////////////////////////////////////////////////////
    SQLiteResultSet &operator=(const SQLiteResultSet &other) = delete;

    friend class SQLiteStatement;
};

////SQLiteStatement////
class SQLiteStatement
{
private:
    sqlite3_stmt *pstmt = nullptr;
    sqlite3 *pdb = nullptr;

private:
    SQLiteStatement();

public:
    ~SQLiteStatement();
    SQLiteStatement(const SQLiteStatement &stat) = delete;
    SQLiteStatement(SQLiteStatement &&stat);
    //drop/create table,view,triger,index etc,
    //return value: 0-success, -1-error
    int execute();
    //query, return SQLiteResultSet.
    SQLiteResultSet executeQuery();
    //insert, return row of inserted if success, else -1
    int executeInsert();
    //update/delete, return row of effected if success, else -1
    int executeUpdateDelete();
    ////bind parameter, return 0 if success, else -1//
    ////nIdx starts from 1////////////////////////////
    int bindString(int nIdx, const string &sVal);
    int bindDouble(int nIdx, double dVal);
    int bindInteger(int nIdx, int nVal);
    int bindLongLong(int nIdx, long long llVal);
    ////////////////////////////////////////////////
    //clear bindings
    int clearBindings();
    //release statement
    void release();
    //recompile with sql, return 0 if success, else -1
    int reCompile(const string &sql);

    SQLiteStatement &operator=(const SQLiteStatement &stat) = delete;

    friend class SQLiteDatabase;
};

////SQLiteDatabase////
class SQLiteDatabase
{
private:
    string m_strDbName;
    sqlite3 *m_pDb = nullptr;

public:
    SQLiteDatabase(const string &dbName);
    ~SQLiteDatabase();
    //open database, return 0 if success, else -1
    int open();
    //close database, return 0 if success, else -1
    int close();
    //return error message
    string errmsg();
    //return error code of last sqlitedb operation
    //return value is undefined when last sqlitedb operate success
    int errcode();
    //begin transaction, return 0 if success, else -1
    int beginTransaction();
    //commit transaction, return 0 if success, else -1
    int commitTransaction();
    //rollback transaction, return 0 if success, else -1
    int rollbackTransaction();
    //create SQLiteStatement using sql
    SQLiteStatement compileStatement(const string &sql);
};

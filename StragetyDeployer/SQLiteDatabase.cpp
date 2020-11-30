#include "SQLiteDatabase.h"
#include <iostream>

//////////////////////SQLiteResultSet//////////////////////////
SQLiteResultSet::SQLiteResultSet()
{
}

SQLiteResultSet::SQLiteResultSet(SQLiteResultSet &&other)
{
    pstmt = other.pstmt;
    other.pstmt = nullptr;
}

SQLiteResultSet::~SQLiteResultSet()
{
}

bool SQLiteResultSet::next()
{
    return sqlite3_step(pstmt) == SQLITE_ROW ? true : false;
}

int SQLiteResultSet::columnCount()
{
    return sqlite3_data_count(pstmt);
}

string SQLiteResultSet::columnName(int nCol)
{
    string res(sqlite3_column_name(pstmt, nCol));
    return std::move(res);
}

int SQLiteResultSet::getInteger(int nCol)
{
    return sqlite3_column_int(pstmt, nCol);
}

long long SQLiteResultSet::getLonglong(int nCol)
{
    return sqlite3_column_int64(pstmt, nCol);
}

double SQLiteResultSet::getDouble(int nCol)
{
    return sqlite3_column_double(pstmt, nCol);
}

string SQLiteResultSet::getString(int nCol)
{
    string res = (char *)sqlite3_column_text(pstmt, nCol);
    return std::move(res);
}

//////////////////////SQLiteStatement//////////////////////////
SQLiteStatement::SQLiteStatement()
{
}

SQLiteStatement::~SQLiteStatement()
{
    sqlite3_finalize(pstmt);
    pstmt = nullptr;
}

SQLiteStatement::SQLiteStatement(SQLiteStatement &&stat)
{
    pdb = stat.pdb;
    pstmt = stat.pstmt;
    stat.pdb = nullptr;
    stat.pstmt = nullptr;
}

int SQLiteStatement::execute()
{
    int ret = sqlite3_step(pstmt);
    if (ret == SQLITE_OK || ret == SQLITE_DONE)
    {
        return SQLITEDATABASE_OK;
    }
    return SQLITEDATABASE_ERROR;
}

SQLiteResultSet SQLiteStatement::executeQuery()
{
    SQLiteResultSet resultSet;
    resultSet.pstmt = pstmt;
    return std::move(resultSet);
}

int SQLiteStatement::executeInsert()
{
    int ret = sqlite3_step(pstmt);
    if (ret == SQLITE_OK || ret == SQLITE_DONE)
    {
        return sqlite3_changes(pdb);
    }
    return SQLITEDATABASE_ERROR;
}

int SQLiteStatement::executeUpdateDelete()
{
    int ret = sqlite3_step(pstmt);
    if (ret == SQLITE_OK || ret == SQLITE_DONE)
    {
        return sqlite3_changes(pdb);
    }
    return SQLITEDATABASE_ERROR;
}

int SQLiteStatement::bindString(int nIdx, const string &sVal)
{
    return sqlite3_bind_text(pstmt, nIdx + 1, sVal.c_str(), sVal.length(), SQLITE_TRANSIENT) == SQLITE_OK ? SQLITEDATABASE_OK : SQLITEDATABASE_ERROR;
}

int SQLiteStatement::bindDouble(int nIdx, double dVal)
{
    return sqlite3_bind_double(pstmt, nIdx + 1, dVal) == SQLITE_OK ? SQLITEDATABASE_OK : SQLITEDATABASE_ERROR;
}

int SQLiteStatement::bindInteger(int nIdx, int nVal)
{
    return sqlite3_bind_int(pstmt, nIdx + 1, nVal) == SQLITE_OK ? SQLITEDATABASE_OK : SQLITEDATABASE_ERROR;
}

int SQLiteStatement::bindLongLong(int nIdx, long long llVal)
{
    return sqlite3_bind_int64(pstmt, nIdx + 1, llVal) == SQLITE_OK ? SQLITEDATABASE_OK : SQLITEDATABASE_ERROR;
}

int SQLiteStatement::clearBindings()
{
    return sqlite3_reset(pstmt) == SQLITE_OK ? SQLITEDATABASE_OK : SQLITEDATABASE_ERROR;
}

void SQLiteStatement::release()
{
    if (pdb && pstmt)
    {
        sqlite3_finalize(pstmt);
        pstmt = nullptr;
    }
}

int SQLiteStatement::reCompile(const string &sql)
{
    return sqlite3_prepare_v2(pdb, sql.c_str(), -1, &pstmt, nullptr) == SQLITE_OK ? SQLITEDATABASE_OK : SQLITEDATABASE_ERROR;
}

///////////////////////SQLiteDatabase/////////////////////////
SQLiteDatabase::SQLiteDatabase(const string &dbName)
{
    m_strDbName = dbName;
}

SQLiteDatabase::~SQLiteDatabase()
{
    if (m_pDb)
        sqlite3_close(m_pDb);
}

int SQLiteDatabase::open()
{
    int ret = sqlite3_open(m_strDbName.c_str(), &m_pDb);
    return ret;
}

int SQLiteDatabase::close()
{
    int ret = SQLITE_OK;
    if (m_pDb)
        ret = sqlite3_close(m_pDb);
    return ret;
}

string SQLiteDatabase::errmsg()
{
    return sqlite3_errmsg(m_pDb);
}

int SQLiteDatabase::errcode()
{
    return sqlite3_errcode(m_pDb);
}

int SQLiteDatabase::beginTransaction()
{
    return sqlite3_exec(m_pDb, "begin;", nullptr, nullptr, nullptr);
}

int SQLiteDatabase::commitTransaction()
{
    return sqlite3_exec(m_pDb, "commit;", nullptr, nullptr, nullptr);
}

int SQLiteDatabase::rollbackTransaction()
{
    return sqlite3_exec(m_pDb, "rollback;", nullptr, nullptr, nullptr);
}

SQLiteStatement SQLiteDatabase::compileStatement(const string &sql)
{
    SQLiteStatement stat;
    stat.pdb = m_pDb;
    sqlite3_prepare_v2(m_pDb, sql.c_str(), -1, &stat.pstmt, nullptr);
    return std::move(stat);
}

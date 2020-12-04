#include <string>
#include <iostream>
#include "SQLiteDatabase.h"

using namespace std;

int main(int argc, char *argv[])
{
    SQLiteDatabase db("ATDConfig.db");
    db.open();
    std::cout << "open database." << std::endl;

    string sql;

    sql = "select * from ManageUser where ManagerName=?";
    // sql = "select * from ManageUser where ManagerID=?";
    SQLiteStatement stmt = db.compileStatement(sql);
    stmt.bindString(0, "admin");
    db.beginTransaction();
    SQLiteResultSet resultSet = stmt.executeQuery();
    if (resultSet.next())
    {
        int id = resultSet.getInteger(0);
        string name = resultSet.getString(1);
        string pass = resultSet.getString(2);
        char line[128] = {0};
        sprintf(line, "query ManageUser, column counts=%d, %s=%d, %s=%s, %s=%s\n", resultSet.columnCount(),
                resultSet.columnName(0).c_str(), id,
                resultSet.columnName(1).c_str(), name.c_str(),
                resultSet.columnName(2).c_str(), pass.c_str());
        cout << line;
    }
    db.commitTransaction();

    // ////create table////
    // sql = "CREATE TABLE [TestUser]("
    //       "[ManagerID] INTEGER NOT NULL UNIQUE, "
    //       "[ManagerName] CHAR(64) NOT NULL UNIQUE, "
    //       "[ManagerPasswd] CHAR(64));";

    // SQLiteStatement stmt = db.compileStatement(sql);
    // db.beginTransaction();
    // int nres = 0;
    // nres = stmt.execute();
    // if (nres < 0)
    // {
    //     std::cout << "Create table [TestUser] failed" << std::endl;
    //     std::cout << "Error: " << db.errcode() << ", " << db.errmsg() << std::endl;
    //     db.rollbackTransaction();
    //     std::cout << "rollback create transaction." << std::endl;
    // }
    // else
    // {
    //     std::cout << "Create table [TestUser] success" << std::endl;
    //     db.commitTransaction();
    //     std::cout << "commit create transaction." << std::endl;
    // }

    // ////insert////
    // sql = "insert into TestUser(ManagerID, ManagerName,ManagerPasswd) values(?,?,?)";
    // stmt.release();
    // stmt.reCompile(sql);
    // for (int i = 0; i < 10; i++)
    // {
    //     char name[32] = {0}, pass[32] = {0};
    //     sprintf(name, "admin%d", i);
    //     sprintf(pass, "pass%d", i);
    //     stmt.bindInteger(0, i);
    //     stmt.bindString(1, name);
    //     stmt.bindString(2, name);
    //     nres = stmt.executeInsert();
    //     if (nres < 0)
    //     {
    //         std::cout << "Insert into table [TestUser] failed, id=" << i << std::endl;
    //         std::cout << "Error: " << db.errcode() << ", " << db.errmsg() << std::endl;
    //         break;
    //     }
    //     else
    //     {
    //         std::cout << "Insert into table [TestUser] success, id=" << i << std::endl;
    //     }
    //     stmt.clearBindings();
    // }
    // if (nres < 0)
    // {
    //     db.rollbackTransaction();
    //     std::cout << "rollback insert transaction." << std::endl;
    // }
    // else
    // {
    //     db.commitTransaction();
    //     std::cout << "commit insert transaction." << std::endl;
    // }

    // ////query////
    // sql = "select * from [TestUser] where 1=1";
    // stmt.release();
    // stmt.reCompile(sql);
    // db.beginTransaction();
    // SQLiteResultSet resultSet = stmt.executeQuery();
    // while (resultSet.next())
    // {
    //     int id = resultSet.getInteger(0);
    //     string name = resultSet.getString(1);
    //     string pass = resultSet.getString(2);
    //     char line[128] = {0};
    //     sprintf(line, "query TestUser, column counts=%d, %s=%d, %s=%s, %s=%s\n", resultSet.columnCount(),
    //             resultSet.columnName(0).c_str(), id,
    //             resultSet.columnName(1).c_str(), name.c_str(),
    //             resultSet.columnName(2).c_str(), pass.c_str());
    //     cout << line;
    // }
    // db.commitTransaction();
    // std::cout << "commit query transaction." << std::endl;

    // ////update////
    // sql = "update TestUser set ManagerName=?,ManagerPasswd=? where ManagerID=?";
    // stmt.release();
    // stmt.reCompile(sql);
    // db.beginTransaction();
    // for (int i = 5; i < 10; i++)
    // {
    //     char name[32] = {0}, pass[32] = {0};
    //     sprintf(name, "upd_admin%d", i);
    //     sprintf(pass, "upd_pass%d", i);
    //     stmt.bindString(0, name);
    //     stmt.bindString(1, name);
    //     stmt.bindInteger(2, i);
    //     nres = stmt.executeUpdateDelete();
    //     if (nres < 0)
    //     {
    //         std::cout << "Update table [TestUser] failed, id=" << i << std::endl;
    //         std::cout << "Error: " << db.errcode() << ", " << db.errmsg() << std::endl;
    //         break;
    //     }
    //     else
    //     {
    //         std::cout << "Update table [TestUser] success, id=" << i << std::endl;
    //     }
    //     stmt.clearBindings();
    // }
    // if (nres < 0)
    // {
    //     db.rollbackTransaction();
    //     std::cout << "rollback update transaction." << std::endl;
    // }
    // else
    // {
    //     db.commitTransaction();
    //     std::cout << "commit update transaction." << std::endl;
    // }

    // ////query after update////
    // sql = "select * from [TestUser] where 1=1";
    // stmt.release();
    // stmt.reCompile(sql);
    // db.beginTransaction();
    // SQLiteResultSet rest = stmt.executeQuery();
    // while (rest.next())
    // {
    //     int id = rest.getInteger(0);
    //     string name = rest.getString(1);
    //     string pass = rest.getString(2);
    //     char line[128] = {0};
    //     sprintf(line, "query TestUser, column counts=%d, %s=%d, %s=%s, %s=%s\n", rest.columnCount(),
    //             rest.columnName(0).c_str(), id,
    //             rest.columnName(1).c_str(), name.c_str(),
    //             rest.columnName(2).c_str(), pass.c_str());
    //     cout << line;
    // }
    // db.commitTransaction();
    // std::cout << "commit query transaction." << std::endl;

    // ////delete////
    // sql = "delete from TestUser where ManagerID=?";
    // stmt.release();
    // stmt.reCompile(sql);
    // db.beginTransaction();
    // for (int i = 2; i < 5; i++)
    // {
    //     stmt.bindInteger(0, i);
    //     nres = stmt.executeUpdateDelete();
    //     if (nres < 0)
    //     {
    //         std::cout << "Delete from table TestUser failed, id=" << i << std::endl;
    //         std::cout << "Error: " << db.errcode() << ", " << db.errmsg() << std::endl;
    //         break;
    //     }
    //     else
    //     {
    //         std::cout << "Delete from table TestUser success, id=" << i << std::endl;
    //     }
    //     stmt.clearBindings();
    // }
    // if (nres < 0)
    // {
    //     db.rollbackTransaction();
    //     std::cout << "rollback delete transaction." << std::endl;
    // }
    // else
    // {
    //     db.commitTransaction();
    //     std::cout << "commit delete transaction." << std::endl;
    // }

    // ////drop table////
    // sql = "drop table TestUser;";
    // stmt.release();
    // stmt.reCompile(sql);
    // nres = stmt.execute();

    // if (nres < 0)
    // {
    //     std::cout << "drop table TestUser failed." << std::endl;
    //     std::cout << "Error: " << db.errcode() << ", " << db.errmsg() << std::endl;
    //     db.rollbackTransaction();
    //     std::cout << "rollback drop table transaction." << std::endl;
    // }
    // else
    // {
    //     db.commitTransaction();
    //     std::cout << "commit drop table transaction." << std::endl;
    // }

    db.close();
    std::cout << "close database." << std::endl;

    return 0;
}

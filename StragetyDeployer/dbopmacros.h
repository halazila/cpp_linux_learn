#pragma once

#define STMT_BIND_1(type1, arg1, i, objVec) \
    int k = 0;                              \
    stmt.bind##type1(k++, objVec[i].arg1)

#define STMT_BIND_2(type1, arg1, type2, arg2, i, objVec) \
    int k = 0;                                           \
    stmt.bind##type1(k++, objVec[i].arg1);               \
    stmt.bind##type2(k++, objVec[i].arg2)

#define STMT_BIND_3(type1, arg1, type2, arg2, type3, arg3, i, objVec) \
    int k = 0;                                                        \
    stmt.bind##type1(k++, objVec[i].arg1);                            \
    stmt.bind##type2(k++, objVec[i].arg2);                            \
    stmt.bind##type3(k++, objVec[i].arg3)

#define STMT_BIND_4(type1, arg1, type2, arg2, type3, arg3, type4, arg4, i, objVec) \
    int k = 0;                                                                     \
    stmt.bind##type1(k++, objVec[i].arg1);                                         \
    stmt.bind##type2(k++, objVec[i].arg2);                                         \
    stmt.bind##type3(k++, objVec[i].arg3);                                         \
    stmt.bind##type4(k++, objVec[i].arg4)

#define STMT_BIND_5(type1, arg1, type2, arg2, type3, arg3, type4, arg4, type5, arg5, i, objVec) \
    int k = 0;                                                                                  \
    stmt.bind##type1(k++, objVec[i].arg1);                                                      \
    stmt.bind##type2(k++, objVec[i].arg2);                                                      \
    stmt.bind##type3(k++, objVec[i].arg3);                                                      \
    stmt.bind##type4(k++, objVec[i].arg4);                                                      \
    stmt.bind##type5(k++, objVec[i].arg5)

#define STMT_BIND_6(type1, arg1, type2, arg2, type3, arg3, type4, arg4, type5, arg5, type6, arg6, i, objVec) \
    int k = 0;                                                                                               \
    stmt.bind##type1(k++, objVec[i].arg1);                                                                   \
    stmt.bind##type2(k++, objVec[i].arg2);                                                                   \
    stmt.bind##type3(k++, objVec[i].arg3);                                                                   \
    stmt.bind##type4(k++, objVec[i].arg4);                                                                   \
    stmt.bind##type5(k++, objVec[i].arg5);                                                                   \
    stmt.bind##type6(k++, objVec[i].arg6)

#define STMT_BIND_7(type1, arg1, type2, arg2, type3, arg3, type4, arg4, type5, arg5, type6, arg6, type7, arg7, i, objVec) \
    int k = 0;                                                                                                            \
    stmt.bind##type1(k++, objVec[i].arg1);                                                                                \
    stmt.bind##type2(k++, objVec[i].arg2);                                                                                \
    stmt.bind##type3(k++, objVec[i].arg3);                                                                                \
    stmt.bind##type4(k++, objVec[i].arg4);                                                                                \
    stmt.bind##type5(k++, objVec[i].arg5);                                                                                \
    stmt.bind##type6(k++, objVec[i].arg6);                                                                                \
    stmt.bind##type7(k++, objVec[i].arg7)

#define STMT_BIND_8(type1, arg1, type2, arg2, type3, arg3, type4, arg4, type5, arg5, type6, arg6, type7, arg7, type8, arg8, i, objVec) \
    int k = 0;                                                                                                                         \
    stmt.bind##type1(k++, objVec[i].arg1);                                                                                             \
    stmt.bind##type2(k++, objVec[i].arg2);                                                                                             \
    stmt.bind##type3(k++, objVec[i].arg3);                                                                                             \
    stmt.bind##type4(k++, objVec[i].arg4);                                                                                             \
    stmt.bind##type5(k++, objVec[i].arg5);                                                                                             \
    stmt.bind##type6(k++, objVec[i].arg6);                                                                                             \
    stmt.bind##type7(k++, objVec[i].arg7);                                                                                             \
    stmt.bind##type8(k++, objVec[i].arg8)

#define STMT_BIND_9(type1, arg1, type2, arg2, type3, arg3, type4, arg4, type5, arg5, type6, arg6, type7, arg7, type8, arg8, type9, arg9, i, objVec) \
    int k = 0;                                                                                                                                      \
    stmt.bind##type1(k++, objVec[i].arg1);                                                                                                          \
    stmt.bind##type2(k++, objVec[i].arg2);                                                                                                          \
    stmt.bind##type3(k++, objVec[i].arg3);                                                                                                          \
    stmt.bind##type4(k++, objVec[i].arg4);                                                                                                          \
    stmt.bind##type5(k++, objVec[i].arg5);                                                                                                          \
    stmt.bind##type6(k++, objVec[i].arg6);                                                                                                          \
    stmt.bind##type7(k++, objVec[i].arg7);                                                                                                          \
    stmt.bind##type8(k++, objVec[i].arg8);                                                                                                          \
    stmt.bind##type9(k++, objVec[i].arg9)

#define STMT_BIND_10(type1, arg1, type2, arg2, type3, arg3, type4, arg4, type5, arg5, type6, arg6, type7, arg7, type8, arg8, type9, arg9, type10, arg10, i, objVec) \
    int k = 0;                                                                                                                                                      \
    stmt.bind##type1(k++, objVec[i].arg1);                                                                                                                          \
    stmt.bind##type2(k++, objVec[i].arg2);                                                                                                                          \
    stmt.bind##type3(k++, objVec[i].arg3);                                                                                                                          \
    stmt.bind##type4(k++, objVec[i].arg4);                                                                                                                          \
    stmt.bind##type5(k++, objVec[i].arg5);                                                                                                                          \
    stmt.bind##type6(k++, objVec[i].arg6);                                                                                                                          \
    stmt.bind##type7(k++, objVec[i].arg7);                                                                                                                          \
    stmt.bind##type8(k++, objVec[i].arg8);                                                                                                                          \
    stmt.bind##type9(k++, objVec[i].arg9);                                                                                                                          \
    stmt.bind##type10(k++, objVec[i].arg10)

#define GET_MACRO_COUNT(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, N, ...) N

#define STMT_BIND_NULL(...)

#define STMT_BIND(...)                                                                                                                                                                                                                                                                                              \
    GET_MACRO_COUNT(__VA_ARGS__, STMT_BIND_10, STMT_BIND_NULL, STMT_BIND_9, STMT_BIND_NULL, STMT_BIND_8, STMT_BIND_NULL, STMT_BIND_7, STMT_BIND_NULL, STMT_BIND_6, STMT_BIND_NULL, STMT_BIND_5, STMT_BIND_NULL, STMT_BIND_4, STMT_BIND_NULL, STMT_BIND_3, STMT_BIND_NULL, STMT_BIND_2, STMT_BIND_NULL, STMT_BIND_1) \
    (__VA_ARGS__)

/////insert macros/////
#define InsertByVector(sql, objVec, ...)                                                                                 \
    if (objVec.size() == 0)                                                                                              \
        return 0;                                                                                                        \
    SQLiteDatabase db(DISPATCHER_DATABASE);                                                                              \
    db.open();                                                                                                           \
    SQLiteStatement stmt = db.compileStatement(sql);                                                                     \
    int res = 0;                                                                                                         \
    for (int i = 0; i < objVec.size(); i++)                                                                              \
    {                                                                                                                    \
        STMT_BIND(__VA_ARGS__, i, objVec);                                                                               \
        res = stmt.executeInsert();                                                                                      \
        if (res < 0)                                                                                                     \
        {                                                                                                                \
            std::cout << __FILE__ << ": " << __LINE__ << " Error: " << db.errcode() << ", " << db.errmsg() << std::endl; \
            StaticDefines::sqlite_error_msg = db.errmsg();                                                               \
            break;                                                                                                       \
        }                                                                                                                \
        stmt.clearBindings();                                                                                            \
    }                                                                                                                    \
    if (res < 0)                                                                                                         \
        db.rollbackTransaction();                                                                                        \
    else                                                                                                                 \
        db.commitTransaction();                                                                                          \
    db.close();                                                                                                          \
    return res

/////update macros/////
#define UpdateByVector(sql, objVec, ...)                                                                                 \
    if (objVec.size() == 0)                                                                                              \
        return 0;                                                                                                        \
    SQLiteDatabase db(DISPATCHER_DATABASE);                                                                              \
    db.open();                                                                                                           \
    SQLiteStatement stmt = db.compileStatement(sql);                                                                     \
    int res = 0;                                                                                                         \
    for (int i = 0; i < objVec.size(); i++)                                                                              \
    {                                                                                                                    \
        STMT_BIND(__VA_ARGS__, i, objVec);                                                                               \
        res = stmt.executeUpdateDelete();                                                                                \
        if (res < 0)                                                                                                     \
        {                                                                                                                \
            std::cout << __FILE__ << ": " << __LINE__ << " Error: " << db.errcode() << ", " << db.errmsg() << std::endl; \
            StaticDefines::sqlite_error_msg = db.errmsg();                                                               \
            break;                                                                                                       \
        }                                                                                                                \
        stmt.clearBindings();                                                                                            \
    }                                                                                                                    \
    if (res < 0)                                                                                                         \
        db.rollbackTransaction();                                                                                        \
    else                                                                                                                 \
        db.commitTransaction();                                                                                          \
    db.close();                                                                                                          \
    return res

/////Query macros/////
#define QryBegin(sql)                                \
    SQLiteDatabase db(DISPATCHER_DATABASE);          \
    db.open();                                       \
    SQLiteStatement stmt = db.compileStatement(sql); \
    db.beginTransaction();                           \
    SQLiteResultSet resultSet = stmt.executeQuery()
#define QryEnd              \
    db.commitTransaction(); \
    db.close()

/////Delete macros/////
#define DeleteByVector(sql, objVec, ...)                                                                                 \
    if (objVec.size() == 0)                                                                                              \
        return 0;                                                                                                        \
    SQLiteDatabase db(DISPATCHER_DATABASE);                                                                              \
    db.open();                                                                                                           \
    SQLiteStatement stmt = db.compileStatement(sql);                                                                     \
    db.beginTransaction();                                                                                               \
    int res = 0;                                                                                                         \
    for (int i = 0; i < objVec.size(); i++)                                                                              \
    {                                                                                                                    \
        STMT_BIND(__VA_ARGS__, i, objVec);                                                                               \
        res = stmt.executeUpdateDelete();                                                                                \
        if (res < 0)                                                                                                     \
        {                                                                                                                \
            std::cout << __FILE__ << ": " << __LINE__ << " Error: " << db.errcode() << ", " << db.errmsg() << std::endl; \
            StaticDefines::sqlite_error_msg = db.errmsg();                                                               \
            break;                                                                                                       \
        }                                                                                                                \
        stmt.clearBindings();                                                                                            \
    }                                                                                                                    \
    if (res < 0)                                                                                                         \
        db.rollbackTransaction();                                                                                        \
    else                                                                                                                 \
        db.commitTransaction();                                                                                          \
    db.close();                                                                                                          \
    return res

#define QryBySql(T, sql) qry##T##BySql(sql)
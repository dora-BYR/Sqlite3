//
// Created by 龙辉 on 16/4/3.
// Copyright (c) 2016 yuemo studio. All rights reserved.
//

#ifndef LSQLITE_LSQLITEDATA_H
#define LSQLITE_LSQLITEDATA_H

#include "define_l.h"
#include "sqlite3.h"
#include <string>
#include <functional>

NS_LONG_BEGIN

    class LSqliteResult;

class LSqliteData
{
public:
    LSqliteData();
    virtual ~LSqliteData();

    void setDBFile(std::string dbFile);
    std::string getDBFile();
    void setDB(sqlite3 * db);
    sqlite3 * getDB();
    void setIsAttached(bool isAttached);
    bool getIsAttached();
    void setIsRemoved(bool isRemoved);
    bool getIsRemoved();
    void detach(bool isRemoved);
    void removeDB();

public:
    sqlite3 * m_pDB;
    std::function<void(LSqliteResult *)> *m_callback;

private:
    std::string m_szDBFile;
    bool m_bIsAttached;
    bool m_bIsRemoved;
};

NS_LONG_END

#endif //LSQLITE_LSQLITEDATA_H

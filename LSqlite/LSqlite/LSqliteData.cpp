//
// Created by 龙辉 on 16/4/3.
// Copyright (c) 2016 yuemo studio. All rights reserved.
//

#include "LSqliteData.h"

NS_LONG_BEGIN

    LSqliteData::LSqliteData():
        m_pDB(nullptr),
        m_bIsAttached(false),
        m_bIsRemoved(false),
        m_szDBFile(""),
        m_callback(nullptr)
    {

    }

    LSqliteData::~LSqliteData() {

    }

    void LSqliteData::setDB(sqlite3 * db) {
        m_pDB = db;
    }

    sqlite3 * LSqliteData::getDB() {
        return m_pDB;
    }

    void LSqliteData::setDBFile(std::string dbFile) {
        m_szDBFile = dbFile;
    }

    std::string LSqliteData::getDBFile() {
        return m_szDBFile;
    }

    void LSqliteData::setIsAttached(bool isAttached) {
        m_bIsAttached = isAttached;
    }

    bool LSqliteData::getIsAttached() {
        return m_bIsAttached;
    }

    void LSqliteData::setIsRemoved(bool isRemoved) {
        m_bIsRemoved = isRemoved;
    }

    bool LSqliteData::getIsRemoved() {
        return m_bIsRemoved;
    }

    void LSqliteData::removeDB() {
        if (m_pDB)
        {
            sqlite3_close_v2(m_pDB);
            delete m_pDB;
            m_pDB = nullptr;
        }
    }

NS_LONG_END

//
// Created by 龙辉 on 16/4/2.
// Copyright (c) 2016 yuemo studio. All rights reserved.
//

#ifndef LSQLITE_LSQLITE_H
#define LSQLITE_LSQLITE_H

#include "define_l.h"
#include "LSqliteResult.h"
#include <string>
#include <map>
#include <mutex>

NS_LONG_BEGIN

    class LSqliteData;
    class LSqliteCommand;
    class LSqliteResult;

    class LSqlite
    {
    public:
        static LSqlite * getInstance();
        virtual ~LSqlite();

        bool attach(std::string dbFile, bool isRemoved = true);
        void detach(std::string dbFile, bool isRemoved);

        bool execute(LSqliteCommand * command);
        bool execute(LSqliteCommand * command, std::function<void(LSqliteResult *)> callback, bool isTrasactionEnabled = false);
        bool execute(std::string sentence);
        bool execute(std::string sentence, std::function<void(LSqliteResult *)> callback, bool isTrasactionEnabled = false);

        bool executeAsync(LSqliteCommand * command);
        bool executeAsync(LSqliteCommand * command, std::function<void(LSqliteResult *)> callback, bool isTrasactionEnabled = false);
        bool executeAsync(std::string sentence);
        bool executeAsync(std::string sentence, std::function<void(LSqliteResult *)> callback, bool isTrasactionEnabled = false);

    private:
        LSqlite();
        static LSqlite * create();
        virtual bool init();
        LSqliteData * getLSqliteData(std::string dbFile);
        bool getIsAttached(std::string dbFile);
        static int sqlite3_query_callback(void* param,int n_column,char** column_value,char** column_name);

    private:
        std::map<std::string, LSqliteData *> m_lsqliteDataMap;
        LSqliteData * m_pCurAttachedData;

    };

NS_LONG_END

#endif //LSQLITE_LSQLITE_H

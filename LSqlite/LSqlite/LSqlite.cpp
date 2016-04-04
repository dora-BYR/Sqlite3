//
// Created by 龙辉 on 16/4/2.
// Copyright (c) 2016 yuemo studio. All rights reserved.
//

#include "LSqlite.h"
#include "LSqliteData.h"
#include "LSqliteCommand.h"
#include "LSqliteResult.h"

NS_LONG_BEGIN

    static std::atomic<LSqlite *> m_pInstance;
    static std::mutex m_mutex;

    LSqlite::LSqlite():
        m_pCurAttachedData(nullptr)
    {
        m_lsqliteDataMap.clear();
    }

    LSqlite::~LSqlite()
    {

    }

    bool LSqlite::init()
    {
        return true;
    }

    LSqlite* LSqlite::create()
    {
        auto lsqlite = new LSqlite();
        if (lsqlite && lsqlite->init())
        {
            return lsqlite;
        }
        else
        {
            delete lsqlite;
            return nullptr;
        }
    }

    LSqlite* LSqlite::getInstance()
    {
        auto temp = m_pInstance.load(std::memory_order_relaxed);
        std::atomic_thread_fence(std::memory_order_acquire);
        if (nullptr == temp)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            temp = m_pInstance.load(std::memory_order_relaxed);
            if (nullptr == temp)
            {
                temp = LSqlite::create();
                std::atomic_thread_fence(std::memory_order_release);
                m_pInstance.store(temp, std::memory_order_relaxed);
            }
        }
        return m_pInstance;
    }

    bool LSqlite::attach(std::string dbFile, bool isRemoved)
    {
        auto data = this->getLSqliteData(dbFile);
        if (data)
        {
            if (!data->getIsAttached())
            {
                data->setIsAttached(true);
            }
            return true;
        }
        else
        {
            data = new LSqliteData();
            data->setDBFile(dbFile);
            m_lsqliteDataMap[dbFile] = data;
        }

        int code = SQLITE_OK;
        if (dbFile.empty())
        {
            code = sqlite3_open_v2(":memory:", &(data->m_pDB), SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
        }
        else
        {
            code = sqlite3_open_v2(dbFile.c_str(), &(data->m_pDB), SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
        }

        bool ret = (code == SQLITE_OK || code == SQLITE_DONE);
        if (ret == true)
        {
            data->setIsAttached(true);
            data->setIsRemoved(false);
            m_pCurAttachedData = data;
        }
        else
        {
            delete data;
            data = nullptr;
        }

        return ret;
    }

    void LSqlite::detach(std::string dbFile, bool isRemoved)
    {
        auto data = this->getLSqliteData(dbFile);
        if (data)
        {
            data->setIsAttached(false);
            data->setIsRemoved(isRemoved);
            if (isRemoved)
            {
                data->removeDB();
            }
        }
    }

    LSqliteData * LSqlite::getLSqliteData(std::string dbFile)
    {
        return m_lsqliteDataMap[dbFile];
    }

    bool LSqlite::getIsAttached(std::string dbFile)
    {
        auto data = this->getLSqliteData(dbFile);
        return data && data->getIsAttached();
    }

    bool LSqlite::execute(LSqliteCommand * command)
    {
        return this->execute(command, [](LSqliteResult *){}, false);
    }

    bool LSqlite::execute(LSqliteCommand *command, std::function<void(LSqliteResult *)> callback, bool isTrasactionEnabled)
    {
        if (!command)
        {
            return false;
        }
        if (m_pCurAttachedData && !m_pCurAttachedData->getDBFile().empty() && m_pCurAttachedData->getDB())
        {
            int ok = sqlite3_open_v2(m_pCurAttachedData->getDBFile().c_str(), &(m_pCurAttachedData->m_pDB), SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);
            if (ok != SQLITE_OK && ok != SQLITE_DONE)
            {
                return false;
            }

            if (isTrasactionEnabled)
            {
                char * errMsg = nullptr;
                ok |= sqlite3_exec(m_pCurAttachedData->m_pDB, "BEGIN EXCLUSIVE", nullptr, nullptr, &errMsg);
                if (errMsg)
                {
                    LLOG("error msg: %s",errMsg);
                    sqlite3_free(errMsg);
                }
                if (ok != SQLITE_OK && ok != SQLITE_DONE)
                {
                    return false;
                }
            }

            int count = command->getSentenceCount();
            for (int i = 0; i < count; ++i)
            {
                std::string sentence = command->getSentence(i);
                if (sentence.empty())
                {
                    continue;
                }
                char * errMsg = nullptr;
                char ** dbResult;
                int nRow, nColumn;
                ok |= sqlite3_get_table(m_pCurAttachedData->m_pDB, sentence.c_str(), &dbResult, &nRow, &nColumn, &errMsg );

                LSqliteResult result(nRow, nColumn, dbResult);
                if (callback && nColumn > 0 && dbResult)
                {
                    callback(&result);
                }

                if (errMsg)
                {
                    LLOG("error msg: %s",errMsg);
                    sqlite3_free(errMsg);
                }

                sqlite3_free_table(dbResult);
            }

            if (isTrasactionEnabled)
            {
                char * errMsg = nullptr;
                ok |= sqlite3_exec(m_pCurAttachedData->m_pDB, "COMMIT", nullptr, nullptr, &errMsg);
                if (errMsg)
                {
                    LLOG("error msg: %s",errMsg);
                    sqlite3_free(errMsg);
                }
                if (ok != SQLITE_OK && ok != SQLITE_DONE)
                {
                    ok |= sqlite3_exec(m_pCurAttachedData->m_pDB, "ROLLBACK", nullptr, nullptr, &errMsg);
                    if (errMsg)
                    {
                        LLOG("error msg: %s",errMsg);
                        sqlite3_free(errMsg);
                    }
                    if (ok != SQLITE_OK && ok != SQLITE_DONE)
                    {
                        LLOG("Transaction rollback failed");
                        return false;
                    }
                    LLOG("Trasaction failed.");
                    return false;
                }
            }

            sqlite3_close(m_pCurAttachedData->m_pDB);

            return ok == SQLITE_OK || ok == SQLITE_DONE;
        }
        return false;
    }

    int LSqlite::sqlite3_query_callback(void *param, int n_column, char **column_value, char **column_name)
    {
        auto data = (LSqliteData *)param;
        std::function<void(LSqliteResult *)> * callback = data->m_callback;
        if (callback)
        {
            LSqliteResult result(param,n_column,column_value,column_name);
            (*callback)(&result);
        }
        return 0;
    }

    bool LSqlite::executeAsync(LSqliteCommand *command, std::function<void(LSqliteResult *)> callback, bool isTrasactionEnabled)
    {
        if (!command)
        {
            return false;
        }
        if (m_pCurAttachedData && !m_pCurAttachedData->getDBFile().empty() && m_pCurAttachedData->getDB())
        {
            int ok = sqlite3_open_v2(m_pCurAttachedData->getDBFile().c_str(), &(m_pCurAttachedData->m_pDB), SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);
            if (ok != SQLITE_OK && ok != SQLITE_DONE)
            {
                return false;
            }

            if (isTrasactionEnabled)
            {
                char * errMsg = nullptr;
                ok |= sqlite3_exec(m_pCurAttachedData->m_pDB, "BEGIN EXCLUSIVE", nullptr, nullptr, &errMsg);
                if (errMsg)
                {
                    LLOG("error msg: %s",errMsg);
                    sqlite3_free(errMsg);
                }
                if (ok != SQLITE_OK && ok != SQLITE_DONE)
                {
                    return false;
                }
            }

            int count = command->getSentenceCount();
            for (int i = 0; i < count; ++i)
            {
                std::string sentence = command->getSentence(i);
                if (sentence.empty())
                {
                    continue;
                }
                m_pCurAttachedData->m_callback = &callback;
                char * errMsg = nullptr;
                ok |= sqlite3_exec(m_pCurAttachedData->m_pDB, sentence.c_str(), sqlite3_callback(&LSqlite::sqlite3_query_callback), (void *)m_pCurAttachedData, &errMsg);

                if (errMsg)
                {
                    LLOG("error msg: %s",errMsg);
                    sqlite3_free(errMsg);
                }
            }

            if (isTrasactionEnabled)
            {
                char * errMsg = nullptr;
                ok |= sqlite3_exec(m_pCurAttachedData->m_pDB, "COMMIT", nullptr, nullptr, &errMsg);
                if (errMsg)
                {
                    LLOG("error msg: %s",errMsg);
                    sqlite3_free(errMsg);
                }
                if (ok != SQLITE_OK && ok != SQLITE_DONE)
                {
                    ok |= sqlite3_exec(m_pCurAttachedData->m_pDB, "ROLLBACK", nullptr, nullptr, &errMsg);
                    if (errMsg)
                    {
                        LLOG("error msg: %s",errMsg);
                        sqlite3_free(errMsg);
                    }
                    if (ok != SQLITE_OK && ok != SQLITE_DONE)
                    {
                        LLOG("Transaction rollback failed");
                        return false;
                    }
                    LLOG("Trasaction failed.");
                    return false;
                }
            }

            sqlite3_close(m_pCurAttachedData->m_pDB);

            return ok == SQLITE_OK || ok == SQLITE_DONE;
        }
        return true;
    }

    bool LSqlite::execute(std::string sentence)
    {
        return this->execute(sentence, [](LSqliteResult *){}, false);
    }

    bool LSqlite::execute(std::string sentence, std::function<void(LSqliteResult *)> callback, bool isTrasactionEnabled)
    {
        LSqliteCommand command(sentence, isTrasactionEnabled);
        return command.execute(callback);
    }

    bool LSqlite::executeAsync(std::string sentence)
    {
        return this->executeAsync(sentence, [](LSqliteResult *){}, false);
    }

    bool LSqlite::executeAsync(std::string sentence, std::function<void(LSqliteResult *)> callback, bool isTrasactionEnabled)
    {
        LSqliteCommand command(sentence, isTrasactionEnabled);
        return command.executeAsync(callback);
    }

NS_LONG_END

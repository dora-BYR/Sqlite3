//
// Created by 龙辉 on 16/4/2.
// Copyright (c) 2016 yuemo studio. All rights reserved.
//

#include <sstream>
#include "LSqliteCommand.h"
#include "LSqliteResult.h"
#include "LSqlite.h"

NS_LONG_BEGIN

#define MAX_SENTENCE_BUF_LONGTH 4096

    LSqliteCommand::LSqliteCommand():
            m_isTrasactionEnabled(false)
    {

    }

    LSqliteCommand::LSqliteCommand(std::string sentence, bool isTransactionEnabeld):
        m_isTrasactionEnabled(isTransactionEnabeld)
    {
        m_sentenceVector.clear();
        std::map<std::string, std::string> sentenceData;
        sentenceData["origin"] = sentence;
        sentenceData["value"] = sentence;
        m_sentenceVector.push_back(sentenceData);
    }

    LSqliteCommand::~LSqliteCommand()
    {

    }

    LSqliteCommand * LSqliteCommand::create()
    {
        auto lsqliteCmd = new LSqliteCommand("", false);
        return lsqliteCmd;
    }

    LSqliteCommand* LSqliteCommand::create(std::string sentence, bool isTransactionEnabeld)
    {
        auto lsqliteCmd = new LSqliteCommand(sentence, isTransactionEnabeld);
        return lsqliteCmd;
    }

    int LSqliteCommand::getSentenceCount()
    {
        return m_sentenceVector.size();
    }

    std::string LSqliteCommand::getSentence(int index) {
        if (m_sentenceVector.size() > index)
        {
            return m_sentenceVector.at(index)["value"];
        }
        return "";
    }

    void LSqliteCommand::pushLine(std::string sentence)
    {
        std::map<std::string, std::string> sentenceData;
        sentenceData["origin"] = sentence;
        sentenceData["value"] = sentence;
        m_sentenceVector.push_back(sentenceData);
    }

    void LSqliteCommand::bindData(int index, ...)
    {
        if (m_sentenceVector.size() > index)
        {
            std::string origin = m_sentenceVector.at(index)["origin"];
            va_list args;
            va_start(args, index);

            char buffer[MAX_SENTENCE_BUF_LONGTH] = {0};
            vsprintf(buffer, origin.c_str(), args);
            //LLOG("buffer = %s", buffer);
            m_sentenceVector.at(index)["value"] .clear();
            m_sentenceVector.at(index)["value"] = buffer;

            va_end(args);
        }
    }

    void LSqliteCommand::reset(std::string sentence, int index)
    {
        if (m_sentenceVector.empty())
        {
            this->pushLine(sentence);
            return;
        }
        if (m_sentenceVector.size() > index)
        {
            m_sentenceVector.at(index)["origin"] = sentence;
            m_sentenceVector.at(index)["value"] = sentence;
        }
    }

    bool LSqliteCommand::execute()
    {
        return this->execute([](LSqliteResult *){});
    }

    bool LSqliteCommand::execute(std::function<void(LSqliteResult *)> callback)
    {
        auto lsqlite = LSqlite::getInstance();
        if (lsqlite)
        {
            lsqlite->execute(this, callback);
        }
        return true;
    }

    bool LSqliteCommand::executeAsync()
    {
        return this->executeAsync([](LSqliteResult *){});
    }

    bool LSqliteCommand::executeAsync(std::function<void(LSqliteResult *)> callback)
    {
        auto lsqlite = LSqlite::getInstance();
        if (lsqlite)
        {
            lsqlite->executeAsync(this, callback);
        }
        return true;
    }

    void LSqliteCommand::setTransactionEnabed(bool isEnabled)
    {
        m_isTrasactionEnabled = isEnabled;
    }

    void LSqliteCommand::clear()
    {
        for (auto mapData:m_sentenceVector)
        {
            mapData.clear();
        }
        m_sentenceVector.clear();
    }

    bool LSqliteCommand::attach(std::string dbFile)
    {
        return LSqlite::getInstance()->attach(dbFile);
    }

NS_LONG_END

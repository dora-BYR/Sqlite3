//
// Created by 龙辉 on 16/4/2.
// Copyright (c) 2016 yuemo studio. All rights reserved.
//

#ifndef LSQLITE_LSQLITECOMMAND_H
#define LSQLITE_LSQLITECOMMAND_H

#include "define_l.h"
#include "LSqliteResult.h"
#include <string>
#include <functional>

NS_LONG_BEGIN

    class LSqliteResult;

class LSqliteCommand {
public:
    LSqliteCommand();
    LSqliteCommand(std::string sentence, bool isTransactionEnabeld = false);
    virtual ~LSqliteCommand();

    LSqliteCommand * create();
    LSqliteCommand * create(std::string sentence, bool isTransactionEnabeld = false);

    void pushLine(std::string sentence);

    void bindData(int index, ...);

    void reset(std::string sentence, int index = 0);

    bool execute();
    bool execute(std::function<void(LSqliteResult *)> callback);
    bool executeAsync();
    bool executeAsync(std::function<void(LSqliteResult *)> callback);

    void setTransactionEnabed(bool isEnabled);

    bool attach(std::string dbFile);
    void clear();

public:
    int getSentenceCount();
    std::string getSentence(int index = 0);

private:
    std::vector<std::map<std::string, std::string>> m_sentenceVector;
    bool m_isTrasactionEnabled;
};

NS_LONG_END

#endif //LSQLITE_LSQLITECOMMAND_H

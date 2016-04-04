//
// Created by 龙辉 on 16/4/2.
// Copyright (c) 2016 yuemo studio. All rights reserved.
//

#ifndef LSQLITE_LSQLITERESULT_H
#define LSQLITE_LSQLITERESULT_H

#include "define_l.h"
#include <string>
#include <map>
#include <vector>

NS_LONG_BEGIN

class LSqliteResult {
public:
    LSqliteResult();
    LSqliteResult(void* param,int n_column,char** column_value,char** column_name);
    LSqliteResult(int nRow, int nColumn, char ** dbResult);
    virtual ~LSqliteResult();

    std::string getValue(std::string key, int row = 0);
    void getData(std::vector<std::string> &resultData);
    int getColumn();

private:
    bool init(int nRow, int nColumn, char ** dbResult);
    bool init(void* param,int n_column,char** column_value,char** column_name);
    std::vector<std::map<std::string, std::string>> m_resultData;
    void * m_pParam;
    int m_nColumn;
    int m_nRow;
    char ** m_pColumnValue;
    char ** m_pColumnName;
    char ** m_pDBResult;
};

NS_LONG_END

#endif //LSQLITE_LSQLITERESULT_H

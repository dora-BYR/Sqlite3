//
// Created by 龙辉 on 16/4/2.
// Copyright (c) 2016 yuemo studio. All rights reserved.
//

#include "LSqliteResult.h"

NS_LONG_BEGIN

    LSqliteResult::LSqliteResult()
    {
        m_resultData.clear();
    }

    LSqliteResult::LSqliteResult(void *param, int n_column, char **column_value, char **column_name)
    {
        m_resultData.clear();
        this->init(param, n_column, column_value, column_name);
    }

    LSqliteResult::LSqliteResult(int nRow, int nColumn, char **dbResult)
    {
        m_resultData.clear();
        this->init(nRow, nColumn, dbResult);
    }

    LSqliteResult::~LSqliteResult()
    {
        m_resultData.clear();
    }

    bool LSqliteResult::init(int nRow, int nColumn, char **dbResult)
    {
        m_nRow = nRow;
        m_nColumn = nColumn;
        m_pDBResult = dbResult;

        if (dbResult && nColumn > 0)
        {
            int nvIndex = nColumn;
            for (int i = 0; i < nRow; i++)
            {
                std::map<std::string, std::string> data;
                for (int j = 0; j < nColumn; ++j)
                {
                    //LLOG("`````1 key = %s, value = %s", dbResult[j], dbResult[nvIndex + j]);
                    data[dbResult[j]] = dbResult[nvIndex + j];
                }
                m_resultData.push_back(data);
                nvIndex = nvIndex + nColumn;
            }
        }
        return true;
    }

    bool LSqliteResult::init(void *param, int n_column, char ** column_value, char **column_name)
    {
        m_pParam = param;
        m_nColumn = n_column;
        m_pColumnValue = column_value;
        m_pColumnName = column_name;

        if (n_column > 0)
        {
            if (column_value && column_name)
            {
                std::map<std::string, std::string> data;
                for (int i = 0; i < n_column; i++)
                {
                    const char * column_name_t = column_name[i];
                    const char * column_value_t = column_value[i] ? column_value[i] : "";
                    data[column_name_t] = column_value_t;
                }
                m_resultData.push_back(data);
            }
        }

        return true;
    }

    std::string LSqliteResult::getValue(std::string key, int row)
    {
        if (m_resultData.size() > row)
        {
            return m_resultData[row][key];
        }
        return "";
    }

    void LSqliteResult::getData(std::vector<std::string> &resultData)
    {

    }

    int LSqliteResult::getColumn()
    {
        return m_nColumn;
    }

NS_LONG_END
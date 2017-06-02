#include "mysql_resultset.h"
#include "mysql_statement.h"
#include "mysql_connection.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <sstream>
#include <iostream>

template <typename T>
T Convert(const char* p, T default_val)
{
    T res = default_val;
    
    if (p != NULL)
    {
        static std::stringstream ss;
        ss.clear();
        ss << p;
        ss >> res;
    }
    return  res;
}

MySQLResultSet::MySQLResultSet(MySQLStatement *stmt) :
    ResultSet(),
    stmt_(stmt),
    mysql_stmt_(NULL),
    mysql_res_(NULL),
    cur_row_(NULL),
    mysql_fields(NULL),
    field_count_(0),
    rows_(0),
    has_result(false),
    fields_length_(NULL)
{
    assert(stmt_ != NULL);
    
}

MySQLResultSet::~MySQLResultSet() 
{
    mysql_free_result(mysql_res_);
}

bool MySQLResultSet::Next()
{
    bool has_next = false;
    cur_row_ = mysql_fetch_row(mysql_res_);
    if (cur_row_ != NULL)
    {
        has_next = true;   
        fields_length_ = mysql_fetch_lengths(mysql_res_);
        assert(fields_length_ != NULL);
    }
    return has_next;
}

bool MySQLResultSet::Init()
{
    if (!StoreResults())
    {
        fprintf(stderr, "when StoreResults occured error!\n");
        return false;
    }
    return true;
}

const Statement* MySQLResultSet::GetStatement() const
{
    return stmt_;
}

uint32_t MySQLResultSet::GetBlob(uint32_t col_index, char* buffer, uint32_t max_buf_len) const
{
    if (col_index < field_count_)
    {
        uint32_t field_length = static_cast<uint32_t>(fields_length_[col_index]);
        if ( field_length <= max_buf_len)
        {
            memcpy(buffer, cur_row_[col_index], field_length);
            return field_length;
        }
        else
        {
            fprintf(stderr, "buffer size less than data size!\n");
        }
    }
    return 0;
}

bool MySQLResultSet::GetBoolean(uint32_t col_index, bool default_val) const
{
    if (IsCurRowValid(col_index))
    {
        return Convert(cur_row_[col_index], default_val);
    }
    return default_val;
}

double MySQLResultSet::GetDouble(uint32_t col_index, double default_val) const
{
    if (IsCurRowValid(col_index))
    {
        return Convert(cur_row_[col_index], default_val);
    }
    return default_val;
}

int32_t MySQLResultSet::GetInt(uint32_t col_index, int32_t default_val) const
{
    if (IsCurRowValid(col_index))
    {
        return Convert(cur_row_[col_index], default_val);
    }
    return default_val;
}

uint32_t MySQLResultSet::GetUInt(uint32_t col_index, uint32_t default_val) const
{
    if (IsCurRowValid(col_index))
    {
        return Convert(cur_row_[col_index], default_val);
    }
    return default_val;
}

int64_t MySQLResultSet::GetInt64(uint32_t col_index, int64_t default_val) const
{
    if (IsCurRowValid(col_index))
    {
        return Convert(cur_row_[col_index], default_val);
    }
    return default_val;
}

uint64_t MySQLResultSet::GetUInt64(uint32_t col_index, uint64_t default_val) const
{
    if (IsCurRowValid(col_index))
    {
        return Convert(cur_row_[col_index], default_val);
    }
    return default_val;
}

uint32_t MySQLResultSet::GetString(uint32_t col_index, char* buffer, uint32_t max_buf_len) const
{
    if (IsCurRowValid(col_index))
    {
        uint32_t field_length = static_cast<uint32_t>(fields_length_[col_index]);
        if ( field_length <= max_buf_len)
        {
            memcpy(buffer, cur_row_[col_index], field_length);
            return field_length;
        }
        else
        {
            fprintf(stderr, "buffer size less than data size!\n");
        }
    }
    return 0;
}

std::string MySQLResultSet::GetString(uint32_t col_index) const
{
    if (IsCurRowValid(col_index))
    {
        uint32_t field_length = static_cast<uint32_t>(fields_length_[col_index]);
        return std::string(cur_row_[col_index], field_length);
    }
    return "";
}

bool MySQLResultSet::IsNull(uint32_t col_index) const
{
    return false;
}

bool MySQLResultSet::StoreResults()
{
    MySQLConnection* connection = static_cast<MySQLConnection*>(stmt_->GetConnection());
    if (connection == NULL)
        return false;
    
    MYSQL* mysql_handler = connection->GetMySQLHandler();
    if (mysql_handler == NULL)
        return false;
    
    mysql_res_ = mysql_store_result(mysql_handler);
    rows_ = mysql_num_rows(mysql_res_);
    
    mysql_fields = mysql_fetch_fields(mysql_res_);
    field_count_ = mysql_field_count(mysql_handler);
    
    return true;
}

bool MySQLResultSet::IsCurRowValid(uint32_t idx) const
{
    return idx < field_count_ && cur_row_[idx] != NULL;
}


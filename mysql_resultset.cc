#include "mysql_resultset.h"
#include "mysql_statement.h"
#include "mysql_connection.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

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
    if (!StoreResults())
    {
        fprintf(stderr, "when StoreResults occured error!\n");
    }
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

const Statement* MySQLResultSet::GetStatement() const
{
    return stmt_;
}

uint32_t MySQLResultSet::GetBlob(uint32_t idx, char* buffer, uint32_t max_buf_len) const
{
    if (idx < field_count_ && cur_row_[idx] != NULL)
    {
        uint32_t field_length = static_cast<uint32_t>(fields_length_[idx]);
        if ( field_length <= max_buf_len)
        {
            memcpy(buffer, cur_row_[idx], field_length);
            return field_length;
        }
        else
        {
            fprintf(stderr, "buffer size less than data size!\n");
        }
    }
    return 0;
}

bool MySQLResultSet::GetBoolean(uint32_t idx, bool def) const
{
    if (idx < field_count_ && cur_row_[idx] != NULL)
    {
        int value = atoi(cur_row_[idx]);
        return value == 1;
    }
    return def;
}

double MySQLResultSet::GetDouble(uint32_t idx, double def) const
{
    if (idx < field_count_ && cur_row_[idx] != NULL)
    {
        double res = def;
        sscanf(cur_row_[idx], "%lf", &res);
        return res;
    }
    return def;
}

int32_t MySQLResultSet::GetInt(uint32_t idx, int32_t def) const
{
    if (idx < field_count_ && cur_row_[idx] != NULL)
    {
        int32_t res = def;
        sscanf(cur_row_[idx], "%d", &res);
        return res;
    }
    return def;
}

uint32_t MySQLResultSet::GetUInt(uint32_t idx, uint32_t def) const
{
    if (idx < field_count_ && cur_row_[idx] != NULL)
    {
        uint32_t res = def;
        sscanf(cur_row_[idx], "%ud", &res);
        return res;
    }
    return def;
}

int64_t MySQLResultSet::GetInt64(uint32_t idx, int64_t def) const
{
    if (idx < field_count_ && cur_row_[idx] != NULL)
    {
        int64_t res = def;
        sscanf(cur_row_[idx], "%ld", &res);
        return res;
    }
    return def;
}

uint64_t MySQLResultSet::GetUInt64(uint32_t idx, uint64_t def) const
{
    if (idx < field_count_ && cur_row_[idx] != NULL)
    {
        uint64_t res = def;
        sscanf(cur_row_[idx], "%lu", &res);
        return res;
    }
    return def;
}

uint32_t MySQLResultSet::GetString(uint32_t idx, char* buffer, uint32_t max_buf_len) const
{
    if (idx < field_count_ && cur_row_[idx] != NULL)
    {
        uint32_t field_length = static_cast<uint32_t>(fields_length_[idx]);
        if ( field_length <= max_buf_len)
        {
            memcpy(buffer, cur_row_[idx], field_length);
            return field_length;
        }
        else
        {
            fprintf(stderr, "buffer size less than data size!\n");
        }
    }
    return 0;
}

bool MySQLResultSet::IsNull(uint32_t idx) const
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
    if (mysql_res_ == NULL)
    {
        return false;
    }
    
    rows_ = mysql_num_rows(mysql_res_);
    
    mysql_fields = mysql_fetch_fields(mysql_res_);
    field_count_ = mysql_field_count(mysql_handler);
    
    return true;
}


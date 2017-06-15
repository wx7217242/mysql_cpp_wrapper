#include "resultset.h"
#include "statement.h"
#include "connection.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <sstream>
#include <iostream>

using namespace mysql;

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

int MySQLResultSet::GetBlob(int idx, char* buffer, unsigned int max_buf_len) const
{
    if (IsCurRowValid(idx))
    {
        unsigned int field_length = static_cast<unsigned int>(fields_length_[idx]);
        if ( field_length <= max_buf_len)
        {
            memcpy(buffer, cur_row_[idx], field_length);
            return static_cast<int>(field_length);
        }
        else
        {
            fprintf(stderr, "buffer size less than data size!\n");
        }
    }
    return 0;
}

bool MySQLResultSet::GetBoolean(int idx, bool def_val) const
{
    if (IsCurRowValid(idx))
    {
        return Convert(cur_row_[idx], def_val);
    }
    return def_val;
}

double MySQLResultSet::GetDouble(int idx, double def_val) const
{
    if (IsCurRowValid(idx))
    {
        return Convert(cur_row_[idx], def_val);
    }
    return def_val;
}

int MySQLResultSet::GetInt(int idx, int def_val) const
{
    if (IsCurRowValid(idx))
    {
        return Convert(cur_row_[idx], def_val);
    }
    return def_val;
}

unsigned int MySQLResultSet::GetUInt(int idx, unsigned int def_val) const
{
    if (IsCurRowValid(idx))
    {
        return Convert(cur_row_[idx], def_val);
    }
    return def_val;
}

long MySQLResultSet::GetInt64(int idx, long def_val) const
{
    if (IsCurRowValid(idx))
    {
        return Convert(cur_row_[idx], def_val);
    }
    return def_val;
}

unsigned long MySQLResultSet::GetUInt64(int idx, unsigned long def_val) const
{
    if (IsCurRowValid(idx))
    {
        return Convert(cur_row_[idx], def_val);
    }
    return def_val;
}

int MySQLResultSet::GetString(int idx, char* buffer, unsigned int max_buf_len) const
{
    if (IsCurRowValid(idx))
    {
        unsigned int field_length = static_cast<unsigned int>(fields_length_[idx]);
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

std::string MySQLResultSet::GetString(int idx) const
{
    if (IsCurRowValid(idx))
    {
        unsigned int field_length = static_cast<unsigned int>(fields_length_[idx]);
        return std::string(cur_row_[idx], field_length);
    }
    return "";
}

bool MySQLResultSet::IsNull(int idx) const
{
    return false;
}

bool MySQLResultSet::StoreResults()
{
    Connection* connection = static_cast<Connection*>(stmt_->GetConnection());
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

bool MySQLResultSet::IsCurRowValid(int idx) const
{
    return idx > 0 
            && static_cast<unsigned int>(idx) < field_count_ 
            && cur_row_[idx] != NULL;
}


static int MySQLStmtFetch(MySQLPreparedStatement* stmt);

static bool MySQLStmtResultMetaData(MySQLPreparedStatement* stmt, MySQLPreparedResultSet* resultset);

static int MySQLStmtBindResult(MySQLPreparedStatement* stmt, MySQLPreparedResultSet* resultset);

int MySQLStmtFetch(MySQLPreparedStatement *stmt)
{
    MYSQL_STMT* mysql_stmt = stmt->GetMySQLStmtHandler();
    assert(mysql_stmt != NULL);
    return mysql_stmt_fetch(mysql_stmt);
}

bool MySQLStmtResultMetaData(MySQLPreparedStatement *stmt, MySQLPreparedResultSet *resultset)
{
    if (stmt == NULL || resultset == NULL)
        return false;
    MYSQL_RES* res = mysql_stmt_result_metadata(stmt->GetMySQLStmtHandler());
    resultset->set_mysql_metadata_res(res);
    
    return true;
}

int MySQLStmtBindResult(MySQLPreparedStatement *stmt, MySQLPreparedResultSet *resultset)
{
    int status = mysql_stmt_bind_result(stmt->GetMySQLStmtHandler(), resultset->mysql_results());
    return status;
}

MySQLPreparedResultSet::MySQLPreparedResultSet(MySQLPreparedStatement *stmt) :
    stmt_(stmt),
    mysql_metadata_res_(NULL),
    mysql_fields(NULL),
    field_count_(0),
    rows_(0),
    has_result(false)
{   
    assert(stmt_ != NULL);
}

MySQLPreparedResultSet::~MySQLPreparedResultSet()
{
    mysql_free_result(mysql_metadata_res_);
}

bool MySQLPreparedResultSet::Next()
{
    has_result = false;
    for (unsigned int i = 0; i < field_count_; ++i)
	{
		MYSQL_BIND &data = results_[i];
		memset(data.buffer, 0, data.buffer_length);
	}
    
    if (stmt_ == NULL)
    {
        return false;
    }
    
    assert(stmt_ != NULL);
    int status = MySQLStmtFetch(stmt_);
    switch(status)
    {
    case 0:
        has_result = true;
        break;
    case 1:
        fprintf(stderr, "%s\n", stmt_->GetError());
        break;
    case MYSQL_NO_DATA: // 不存在行／数据
//        fprintf(stderr, "不存在行／数据\n");
        break;
    case MYSQL_DATA_TRUNCATED: // 出现数据截短
//        fprintf(stderr, "出现数据截短\n");
        break;
    default:
        break;
    }
    return has_result;
}

const Statement *MySQLPreparedResultSet::GetStatement() const
{
    return stmt_;
}

int MySQLPreparedResultSet::GetBlob(int idx, char *buffer, unsigned int max_buf_len) const
{
    if (IsCurRowValid(idx))
    {
        unsigned int buffer_length = static_cast<unsigned int>(fields_length_[idx]); //data.buffer_length
        const MYSQL_BIND& data = results_[idx];
        if (max_buf_len < buffer_length)
        {
            memcpy(buffer, data.buffer, max_buf_len);
            return max_buf_len;
        }
        else
        {
            memcpy(buffer, data.buffer, buffer_length);
            return buffer_length;
        }
    }
    return 0;
}

bool MySQLPreparedResultSet::GetBoolean(int idx, bool def_val) const
{
    if (IsCurRowValid(idx))
    {
        const MYSQL_BIND& data = results_[idx];
        char* buffer = reinterpret_cast<char*>(data.buffer);
        return *buffer == '1';
    }
    return def_val;
}

double MySQLPreparedResultSet::GetDouble(int idx, double def_val) const
{
    if (IsCurRowValid(idx))
    {
        const MYSQL_BIND& data = results_[idx];
        return *reinterpret_cast<double*>(data.buffer);
    }
    return def_val;
}

int MySQLPreparedResultSet::GetInt(int idx, int def_val) const
{
    if (IsCurRowValid(idx))
    {
        const MYSQL_BIND& data = results_[idx];
        return *reinterpret_cast<int32_t*>(data.buffer);
    }
    return def_val;
}

unsigned int MySQLPreparedResultSet::GetUInt(int idx, unsigned int def_val) const
{
    if (IsCurRowValid(idx))
    {
        const MYSQL_BIND& data = results_[idx];
        return *reinterpret_cast<unsigned int*>(data.buffer);
    }
    return def_val;
}

long MySQLPreparedResultSet::GetInt64(int idx, long def_val) const
{
    if (IsCurRowValid(idx))
    {
        const MYSQL_BIND& data = results_[idx];
        return *reinterpret_cast<int64_t*>(data.buffer);
    }
    return def_val;
}

unsigned long MySQLPreparedResultSet::GetUInt64(int idx, unsigned long def_val) const
{
    if (IsCurRowValid(idx))
    {
        const MYSQL_BIND& data = results_[idx];
        return *reinterpret_cast<size_t*>(data.buffer);
    }
    return def_val;
}

int MySQLPreparedResultSet::GetString(int idx, char *buffer, unsigned int max_buf_len) const
{
    if (IsCurRowValid(idx))
    {
        unsigned int buffer_length = static_cast<unsigned int>(fields_length_[idx]); //data.buffer_length
        const MYSQL_BIND& data = results_[idx];
        if (max_buf_len < buffer_length)
        {
            memcpy(buffer, data.buffer, max_buf_len);
            return max_buf_len;
        }
        else
        {
            memcpy(buffer, data.buffer, buffer_length);
            return buffer_length;
        }
    }
    return 0;
}

std::string MySQLPreparedResultSet::GetString(int idx) const
{
    if (IsCurRowValid(idx))
    {
        unsigned int buffer_length = static_cast<unsigned int>(fields_length_[idx]); //data.buffer_length
        const MYSQL_BIND& data = results_[idx];
        return std::string(static_cast<char*>(data.buffer), buffer_length);
    }
    return "";
}

bool MySQLPreparedResultSet::IsNull(int idx) const
{
    if (IsCurRowValid(idx))
    {
        const MYSQL_BIND& data = results_[idx];
        if (data.is_null)
            return true;
        else
            return false;
    }
    return true;
}

void MySQLPreparedResultSet::set_mysql_metadata_res(MYSQL_RES *res)
{
    mysql_metadata_res_ = res;
}

MYSQL_BIND* MySQLPreparedResultSet::mysql_results()
{
    return results_.data();
}

bool MySQLPreparedResultSet::BindResults()
{
    MySQLStmtResultMetaData(stmt_, this);
    
    // 获取返回的列数
    field_count_ = mysql_num_fields(mysql_metadata_res_);
    fields_length_.resize(field_count_);
    
    // 获取每一列的信息，字段的类型
    mysql_fields = mysql_fetch_fields(mysql_metadata_res_);
    results_.resize(field_count_);
    memset(results_.data(), 0x00, sizeof(MYSQL_BIND) * field_count_);
    
    bool need_extend = false;
    
    do
    {
        if (need_extend)
        {
            if (result_buffer_->ExtendBuffer())
            {
                result_buffer_->ResetBuffer();
                memset(results_.data(), 0x00, sizeof(MYSQL_BIND) * field_count_);
                need_extend = false;
            }
            else
            {
                abort();
            }
        }
        
        for (unsigned int i = 0; i < field_count_; ++i)
        {
            MYSQL_BIND &data = results_[i];
            data.buffer_type = mysql_fields[i].type;
            data.length = &(fields_length_[i]);
            data.buffer_length = mysql_fields[i].length;
            
            data.buffer = result_buffer_->buffer_from_current();
            if (!result_buffer_->IncreaseBufferCurPos(data.buffer_length)) // need extend
            {
                need_extend = true;
                break;
            }
            
            my_bool* isNull = result_buffer_->buffer_from_current();
            if (!result_buffer_->IncreaseBufferCurPos(sizeof(my_bool)))
            {
                need_extend = true;
                break;
            }
            data.is_null = isNull;
        }
        
    } while (need_extend);
    
    if (result_buffer_->buffer_cur_pos() > result_buffer_->buffer_size())
    {
        fprintf(stderr, "error, result buffer is full. cant bindResult!\n");
        return false;
    }
    
    int status = MySQLStmtBindResult(stmt_, this);
    if (status == 0)
    {
        // has_result = Next(); // TODO
        return true;
    }
    else
    {
        fprintf(stderr, "%s\n", stmt_->GetError());
        return false;
    }
}

bool MySQLPreparedResultSet::IsCurRowValid(int idx) const
{
    if (idx > 0 && static_cast<unsigned int>(idx) < field_count_)
    {
        return true;
    }
    return false;
}

bool MySQLPreparedResultSet::Init()
{
    Connection* conn = static_cast<Connection*>(stmt_->GetConnection());
    assert(conn != NULL);
    
    result_buffer_ = conn->result_buffer();
    assert(result_buffer_ != NULL);
    result_buffer_->ResetBuffer();
    
    if (!BindResults())
    {
        fprintf(stderr, "MySQLPreparedResultSet BindResults failed!\n");
        return false;
    }
    return true;
}


#include "mysql_ps_resultset.h"
#include "mysql_prepared_statement.h"
#include "mysql_connection.h"
#include "mysql_exception.h"
#include "mysql_buffer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>   

static int MySQLStmtFetch(MySQLPreparedStatement* stmt);

static bool MySQLStmtResultMetaData(MySQLPreparedStatement* stmt, MySQLPreparedResultSet* resultset);

static int MySQLStmtBindResult(MySQLPreparedStatement* stmt, MySQLPreparedResultSet* resultset);

int MySQLStmtFetch(MySQLPreparedStatement *stmt)
{
    return mysql_stmt_fetch(stmt->GetMySQLStmtHandler());
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
    MySQLConnection* conn = static_cast<MySQLConnection*>(stmt_->GetConnection());
    assert(conn != NULL);
    
    result_buffer_ = conn->result_buffer();
    assert(result_buffer_ != NULL);
    result_buffer_->ResetBuffer();
    
    if (!BindResults())
    {
        fprintf(stderr, "MySQLPreparedResultSet BindResults failed!\n");
    }
}

MySQLPreparedResultSet::~MySQLPreparedResultSet()
{
    mysql_free_result(mysql_metadata_res_);
}

bool MySQLPreparedResultSet::Next()
{
    has_result = false;
    for (uint32_t i = 0; i < field_count_; ++i)
	{
		MYSQL_BIND &data = results_[i];
		memset(data.buffer, 0, data.buffer_length);
	}
    
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
        fprintf(stderr, "不存在行／数据\n");
        break;
    case MYSQL_DATA_TRUNCATED: // 出现数据截短
        fprintf(stderr, "出现数据截短\n");
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

uint32_t MySQLPreparedResultSet::GetBlob(uint32_t col_index, char *buffer, uint32_t max_buf_len) const
{
    if (col_index >= 0 && col_index < field_count_)
    {
        uint32_t buffer_length = static_cast<uint32_t>(fields_length_[col_index]); //data.buffer_length
        const MYSQL_BIND& data = results_[col_index];
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

bool MySQLPreparedResultSet::GetBoolean(uint32_t col_index, bool default_val) const
{
    if (col_index >= 0 && col_index < field_count_)
    {
        const MYSQL_BIND& data = results_[col_index];
        char* buffer = reinterpret_cast<char*>(data.buffer);
        return *buffer == '1';
    }
    return default_val;}

double MySQLPreparedResultSet::GetDouble(uint32_t col_index, double default_val) const
{
    if (col_index >= 0 && col_index < field_count_)
    {
        const MYSQL_BIND& data = results_[col_index];
        return *reinterpret_cast<double*>(data.buffer);
    }
    return default_val;
}

int32_t MySQLPreparedResultSet::GetInt(uint32_t col_index, int32_t default_val) const
{
    if (col_index >= 0 && col_index < field_count_)
    {
        const MYSQL_BIND& data = results_[col_index];
        return *reinterpret_cast<int32_t*>(data.buffer);
    }
    return default_val;
}

uint32_t MySQLPreparedResultSet::GetUInt(uint32_t col_index, uint32_t default_val) const
{
    if (col_index >= 0 && col_index < field_count_)
    {
        const MYSQL_BIND& data = results_[col_index];
        return *reinterpret_cast<uint32_t*>(data.buffer);
    }
    return default_val;
}

int64_t MySQLPreparedResultSet::GetInt64(uint32_t col_index, int64_t default_val) const
{
    if (col_index >= 0 && col_index < field_count_)
    {
        const MYSQL_BIND& data = results_[col_index];
        return *reinterpret_cast<int64_t*>(data.buffer);
    }
    return default_val;
}

uint64_t MySQLPreparedResultSet::GetUInt64(uint32_t col_index, uint64_t default_val) const
{
    if (col_index >= 0 && col_index < field_count_)
    {
        const MYSQL_BIND& data = results_[col_index];
        return *reinterpret_cast<uint64_t*>(data.buffer);
    }
    return default_val;
}

uint32_t MySQLPreparedResultSet::GetString(uint32_t col_index, char *buffer, uint32_t max_buf_len) const
{
    if (col_index >= 0 && col_index < field_count_)
    {
        uint32_t buffer_length = static_cast<uint32_t>(fields_length_[col_index]); //data.buffer_length
        const MYSQL_BIND& data = results_[col_index];
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

bool MySQLPreparedResultSet::IsNull(uint32_t col_index) const
{
    if (col_index >= 0 && col_index < field_count_)
    {
        const MYSQL_BIND& data = results_[col_index];
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
    memset(results_.data(), 0, sizeof(MYSQL_BIND) * field_count_);
                
    for (uint32_t i = 0; i < field_count_; ++i)
    {
        MYSQL_BIND &data = results_[i];
        data.buffer_type = mysql_fields[i].type;
        data.length = &(fields_length_[i]);
        
        switch (mysql_fields[i].type)
        {
        case MYSQL_TYPE_LONGLONG: // long
        {
            data.buffer_length = sizeof(long long);
        }break;
        case MYSQL_TYPE_LONG: // int
        {
            data.buffer_length = sizeof(int);
        }break;
        case MYSQL_TYPE_FLOAT: // float
        {
            data.buffer_length = sizeof(float);
        }break;
        case MYSQL_TYPE_VAR_STRING:
        case MYSQL_TYPE_STRING:
        {
            data.buffer_length = mysql_fields[i].length;
        }break;
        case MYSQL_TYPE_BLOB:
        {
            data.buffer_length = mysql_fields[i].length;
        }break;
        case MYSQL_TYPE_DATETIME:
        {
            data.buffer_length = sizeof(MYSQL_TIME);
        }break;
        case MYSQL_TYPE_TINY:
        {
            data.buffer_length = mysql_fields[i].length;
        }break;
        case MYSQL_TYPE_DOUBLE:
        {
            data.buffer_length = sizeof(double);
        }break;
        default:
        {
            fprintf(stderr, "other mysql type: %d\n", mysql_fields[i].type);
            return false;
        }break;
        }
        
        data.buffer = result_buffer_->buffer_from_current();
        result_buffer_->IncreaseBufferCurPos(data.buffer_length);
        
        my_bool* isNull = result_buffer_->buffer_from_current();
        result_buffer_->IncreaseBufferCurPos(sizeof(my_bool));
        data.is_null = isNull;
    }
    
    if (result_buffer_->buffer_cur_pos() > result_buffer_->buffer_size())
    {
        fprintf(stderr, "error, result buffer is full. cant bindResult!\n");
        return false;
    }
    
    int status = MySQLStmtBindResult(stmt_, this);
    if (status == 0)
    {
        //            has_result = Next(); // TODO
        return true;
    }
    else
    {
        fprintf(stderr, "%s\n", stmt_->GetError());
        return false;
    }
}


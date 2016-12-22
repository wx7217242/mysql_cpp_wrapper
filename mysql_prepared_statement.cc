#include "mysql_prepared_statement.h"
#include "mysql_ps_resultset.h"
#include "mysql_connection.h"
#include "mysql_exception.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

MySQLPreparedStatement::MySQLPreparedStatement(MySQLConnection *conn, const std::string &sql) :
    connection_(conn),
    sql_(sql),
    resultset_(NULL),
    insert_id_(0),
    affected_rows_(0)
{
    assert(conn != NULL && sql.size() > 0);
    
    param_buffer_ = conn->param_buffer();
    assert(param_buffer_ != NULL);
    
    param_buffer_->ResetBuffer();
    cur_param_idx_ = 0;
    
    mysql_stmt_ = mysql_stmt_init(connection_->GetMySQLHandler());
    if (mysql_stmt_ == NULL)
    {
        throw MySQLException(GetError());
    }    
    
    if (mysql_stmt_prepare(mysql_stmt_, sql.c_str(), sql.length()) != 0)
    {
        throw MySQLException(GetError());
    }
    
    param_count_ = mysql_stmt_param_count(mysql_stmt_);
    params_.resize(param_count_);
}

MySQLPreparedStatement::~MySQLPreparedStatement()
{
    if (mysql_stmt_ != NULL)
        mysql_stmt_close(mysql_stmt_);
}

Connection *MySQLPreparedStatement::GetConnection()
{
    return connection_;
}

ResultSet *MySQLPreparedStatement::GetResultSet()
{
    return resultset_;
}

bool MySQLPreparedStatement::Execute(const std::string &sql)
{
    return false;
}

bool MySQLPreparedStatement::Execute()
{
    if (cur_param_idx_ != param_count_)
    {
        throw MySQLException("bind param count incorrect!");
    }
    
    if (mysql_stmt_bind_param(mysql_stmt_, params_.data()) != 0)
    {
        throw MySQLException(GetError());
    }
    
    if (mysql_stmt_execute(mysql_stmt_) != 0)
    {
        throw MySQLException(GetError());
    }
    
    affected_rows_ = static_cast<int>(mysql_stmt_affected_rows(mysql_stmt_));
    return true;
}

ResultSet *MySQLPreparedStatement::ExecuteQuery(const std::string &sql)
{
    return NULL;
}

ResultSet *MySQLPreparedStatement::ExecuteQuery()
{
    try
    {
        Execute();
        resultset_ = new MySQLPreparedResultSet(this);
        return resultset_; 
    }
    catch(MySQLException& e)
    {
        throw(e);
    }
    return NULL;
}

int MySQLPreparedStatement::ExecuteUpdate(const std::string &sql)
{
    return -1;
}

int MySQLPreparedStatement::ExecuteUpdate()
{
    try
    {
        Execute();
        return affected_rows_;
    }
    catch(MySQLException& e)
    {
        throw(e);
    }
    return -1;
}

bool MySQLPreparedStatement::GetMoreResults()
{
    if (mysql_stmt_ == NULL)
        return false;
    
    int status = mysql_stmt_next_result(mysql_stmt_);
    if (status == 0)
        return true;
    
    return false;
}

bool MySQLPreparedStatement::SetBoolean(uint32_t param_idx, bool value)
{
    char val = (value ? '1' : '0');
    return SetParamToBuffer(param_idx, MYSQL_TYPE_TINY, &val, sizeof(val), false);
}

bool MySQLPreparedStatement::SetInt(uint32_t param_idx, int32_t value)
{
    return SetParamToBuffer(param_idx, MYSQL_TYPE_LONG, &value, sizeof(value), false);
}

bool MySQLPreparedStatement::SetUInt(uint32_t param_idx, uint32_t value)
{
    return SetParamToBuffer(param_idx, MYSQL_TYPE_LONG, &value, sizeof(value), true);
}

bool MySQLPreparedStatement::SetBigInt(uint32_t param_idx, int64_t value)
{
    return SetParamToBuffer(param_idx, MYSQL_TYPE_LONG, &value, sizeof(value), false);
}

bool MySQLPreparedStatement::SetInt64(uint32_t param_idx, int64_t value)
{
    return SetParamToBuffer(param_idx, MYSQL_TYPE_LONGLONG, &value, sizeof(value), false);
}

bool MySQLPreparedStatement::SetUInt64(uint32_t param_idx, uint64_t value)
{
    return SetParamToBuffer(param_idx, MYSQL_TYPE_LONGLONG, &value, sizeof(value), true);
}

bool MySQLPreparedStatement::SetDouble(uint32_t param_idx, double value)
{
    return SetParamToBuffer(param_idx, MYSQL_TYPE_DOUBLE, &value, sizeof(value), false);
}

bool MySQLPreparedStatement::SetBlob(uint32_t param_idx, const char *blob, uint32_t length)
{
    return SetParamToBuffer(param_idx, MYSQL_TYPE_BLOB, blob, length, false);
}

bool MySQLPreparedStatement::SetString(uint32_t param_idx, const char *value)
{
    return SetParamToBuffer(param_idx, MYSQL_TYPE_STRING, value, strlen(value), false);
}

bool MySQLPreparedStatement::SetDateTime(uint32_t param_idx, const char *value)
{
    return SetParamToBuffer(param_idx, MYSQL_TYPE_TIME, value, strlen(value), false);
}

bool MySQLPreparedStatement::SetNull(uint32_t param_idx, int sql_type)
{
    // MYSQL_TYPE_NULL
    return false; // TODO
}

int MySQLPreparedStatement::GetErrNo()
{
    if (mysql_stmt_ != NULL)
        return mysql_stmt_errno(mysql_stmt_);
    return -1;
}

const char *MySQLPreparedStatement::GetError()
{
    if (mysql_stmt_ != NULL)
        return mysql_stmt_error(mysql_stmt_);
    return "Unkonwn mysql_stmt error!";
}

unsigned long long MySQLPreparedStatement::GetLastInsertID() const
{
    return insert_id_;
}

int MySQLPreparedStatement::GetAffectedRows() const
{
    return affected_rows_;
}

MYSQL_STMT *MySQLPreparedStatement::GetMySQLStmtHandler() const
{
    return mysql_stmt_;
}

bool MySQLPreparedStatement::SetParamToBuffer(uint32_t param_idx, enum_field_types type, const void *value, uint64_t size, bool is_unsigned)
{
    if (param_idx < param_count_ && param_buffer_->IsCapacityAvailable(size))
    {
        if (cur_param_idx_ != param_idx)
        {
            fprintf(stderr, "param index error!\n");
            return false;
        }
        
        MYSQL_BIND& data = params_[param_idx];	
        memset(&data, 0, sizeof(data));
        
        data.buffer_type = type;
        param_buffer_->SetParamToBuffer(value, size);
        data.buffer = param_buffer_->buffer_from_current();
        data.buffer_length = size; // 对于string 和 blob 需要设置 buffer_length
        param_buffer_->IncreaseBufferCurPos(size);
        
        data.is_unsigned = is_unsigned;
        
        ++cur_param_idx_;
        return true;
    }
    return false;
}


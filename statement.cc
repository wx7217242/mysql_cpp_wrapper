#include "statement.h"
#include "connection.h"
#include "resultset.h"
#include <mysql/mysql.h>

#include <stdio.h>
#include <assert.h>

using namespace mysql;

MySQLStatement::MySQLStatement(Connection* conn) : Statement(),
    connection_(conn),
    resultset_(NULL),
    affected_rows_(0),
    insert_id_(0)
{
    
}

MySQLStatement::~MySQLStatement()
{
    
}

Connection *MySQLStatement::GetConnection()
{
    return connection_;
}

bool MySQLStatement::Init()
{
    return true;
}

bool MySQLStatement::Execute(const std::string &sql)
{
    if (mysql_real_query(connection_->GetMySQLHandler(), sql.c_str(), sql.length()) != 0)
    {
        fprintf(stderr, "%s\n", connection_->GetError());
        return false;
    }
    
    affected_rows_ = mysql_affected_rows(connection_->GetMySQLHandler());
    
    // 没有产生 AUTO_INCREMENT 的 ID，则 mysql_insert_id() 返回 0。
    insert_id_ = mysql_insert_id(connection_->GetMySQLHandler());
    
    return true;
}

ResultSet* MySQLStatement::ExecuteQuery(const std::string &sql)
{
    if (mysql_real_query(connection_->GetMySQLHandler(), sql.c_str(), sql.length()) != 0)
    {
        fprintf(stderr, "%s\n", connection_->GetError());
        return NULL;
    }
    resultset_ = new MySQLResultSet(this);
    if (resultset_ != NULL)
    {
        if (!resultset_->Init())
        {
            delete resultset_;
            resultset_ = NULL;
        }
    }
    return resultset_;
}

ResultSet* MySQLStatement::GetResultSet()
{
    return resultset_;
}

int MySQLStatement::ExecuteUpdate(const std::string &sql)
{
    if (mysql_real_query(connection_->GetMySQLHandler(), sql.c_str(), sql.length()) != 0)
    {
        fprintf(stderr, "%s\n", connection_->GetError());
        return -1;
    }
    else
    {
        affected_rows_ = mysql_affected_rows(connection_->GetMySQLHandler());
        return static_cast<int>(affected_rows_);
    }
}

bool MySQLStatement::GetMoreResults()
{
    bool ret = false;
    int status = mysql_next_result(connection_->GetMySQLHandler());
    switch (status) 
    {
    case 0: // 成功并有多个结果。
        ret = true;
        break;
    case -1: // 成功但没有多个结果。
        break;
    default: // <0 出错
        fprintf(stderr, "%s\n", connection_->GetError());
        break;
    }
    return ret;
}

unsigned long long MySQLStatement::GetLastInsertID()
{
    return insert_id_;
}


MySQLPreparedStatement::MySQLPreparedStatement(Connection *conn, const std::string &sql) :
    connection_(conn),
    sql_(sql),
    resultset_(NULL),
    insert_id_(0),
    affected_rows_(0)
{
    assert(conn != NULL && sql.size() > 0);
    
    
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
        if (resultset_ != NULL)
        {
            if (!resultset_->Init())
            {
                delete resultset_;
                resultset_ = NULL;
            }
        }
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

bool MySQLPreparedStatement::SetBoolean(int idx, bool value)
{
    return SetParamToBuffer(idx, MYSQL_TYPE_TINY, &value, sizeof(value), false);
}

bool MySQLPreparedStatement::SetInt(int idx, int value)
{
    return SetParamToBuffer(idx, MYSQL_TYPE_LONG, &value, sizeof(value), false);
}

bool MySQLPreparedStatement::SetUInt(int idx, unsigned int value)
{
    return SetParamToBuffer(idx, MYSQL_TYPE_LONG, &value, sizeof(value), true);
}

bool MySQLPreparedStatement::SetBigInt(int idx, long value)
{
    return SetParamToBuffer(idx, MYSQL_TYPE_LONG, &value, sizeof(value), false);
}

bool MySQLPreparedStatement::SetInt64(int idx, long value)
{
    return SetParamToBuffer(idx, MYSQL_TYPE_LONGLONG, &value, sizeof(value), false);
}

bool MySQLPreparedStatement::SetUInt64(int idx, unsigned long value)
{
    return SetParamToBuffer(idx, MYSQL_TYPE_LONGLONG, &value, sizeof(value), true);
}

bool MySQLPreparedStatement::SetDouble(int idx, double value)
{
    return SetParamToBuffer(idx, MYSQL_TYPE_DOUBLE, &value, sizeof(value), false);
}

bool MySQLPreparedStatement::SetBlob(int idx, const char *blob, unsigned int length)
{
    return SetParamToBuffer(idx, MYSQL_TYPE_BLOB, blob, length, false);
}

bool MySQLPreparedStatement::SetString(int idx, const char *value)
{
    return SetParamToBuffer(idx, MYSQL_TYPE_STRING, value, strlen(value), false);
}

bool MySQLPreparedStatement::SetDateTime(int idx, const char *value)
{
    return SetParamToBuffer(idx, MYSQL_TYPE_TIME, value, strlen(value), false);
}

bool MySQLPreparedStatement::SetNull(int idx, int sql_type)
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

bool MySQLPreparedStatement::SetParamToBuffer(int idx, enum_field_types type, const void *value, size_t size, bool is_unsigned)
{
    if (idx > 0 && 
            static_cast<unsigned long>(idx) < param_count_ 
            && param_buffer_->IsCapacityAvailable(size))
    {
        if (cur_param_idx_ != static_cast<unsigned long>(idx))
        {
            fprintf(stderr, "param index error!\n");
            return false;
        }
        
        MYSQL_BIND& data = params_[idx];	
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



bool MySQLPreparedStatement::Init()
{
    param_buffer_ = connection_->param_buffer();
    assert(param_buffer_ != NULL);
    
    param_buffer_->ResetBuffer();
    cur_param_idx_ = 0;
    
    mysql_stmt_ = mysql_stmt_init(connection_->GetMySQLHandler());
    if (mysql_stmt_ == NULL)
    {
        return false;
//        throw MySQLException(GetError());
    }    
    
    if (mysql_stmt_prepare(mysql_stmt_, sql_.c_str(), sql_.length()) != 0)
    {
        return false;
//        throw MySQLException(GetError());
    }
    
    param_count_ = mysql_stmt_param_count(mysql_stmt_);
    params_.resize(param_count_);
    
    return true;
}
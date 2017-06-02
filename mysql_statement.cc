#include "mysql_statement.h"
#include "mysql_connection.h"
#include "mysql_resultset.h"

#include <mysql/mysql.h>

#include <stdio.h>
#include <assert.h>

MySQLStatement::MySQLStatement(MySQLConnection* conn) : Statement(),
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

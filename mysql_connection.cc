#include "mysql_connection.h"
#include "mysql_statement.h"
#include "mysql_prepared_statement.h"
#include "mysql_exception.h"

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

MySQLConnection::MySQLConnection(const char *host, 
                                 const char *user, 
                                 const char *passwd, 
                                 const char *database, 
                                 short port, 
                                 uint32_t param_buf_size, 
                                 uint32_t result_buf_size)
: Connection(host, user, passwd, database, port),
  connected_(false),
  host_(host),
  user_(user),
  passwd_(passwd),
  db_(database),
  port_(port),
  ping_counter_(0),
  timeout_(15),
  auto_commit_(true),
  param_buffer_(param_buf_size),
  result_buffer_(result_buf_size)
{
    if (mysql_init(&mysql_) == NULL)
    {
        fprintf(stderr, "mysql_init error!\n");
        abort();
    }
}

MySQLConnection::~MySQLConnection()
{
    Close();
}

bool MySQLConnection::Connect(const char *charset, unsigned int timeout, bool auto_commit)
{
    if (connected_)
        return false;
    
    charset_ = charset;
    timeout_ = timeout;
    auto_commit_ = auto_commit;
    
    mysql_set_character_set(&mysql_, charset);
    mysql_options(&mysql_, MYSQL_OPT_READ_TIMEOUT, &timeout);
    mysql_options(&mysql_, MYSQL_OPT_WRITE_TIMEOUT, &timeout);
        
    if (mysql_real_connect(&mysql_, 
                           host_.c_str(), 
                           user_.c_str(), 
                           passwd_.c_str(), 
                           db_.c_str(), 
                           port_, 
                           NULL, CLIENT_COMPRESS | CLIENT_MULTI_STATEMENTS) != NULL)
    {
        connected_ = true;
        mysql_autocommit(&mysql_, auto_commit_);
    }
    else
    {
        throw MySQLException(GetError());
    }
    return connected_;
}

bool MySQLConnection::Reconnect()
{
    Close();
    return Connect(charset_.c_str(), timeout_, auto_commit_);
}

bool MySQLConnection::IsConnected()
{
    ++ping_counter_;
    ping_counter_  %=  1000;
    if (ping_counter_ == 0)
    {
		int ping_code = mysql_ping(&mysql_);
        connected_ = ( ping_code == 0);
		if (!connected_) 
		{
			fprintf(stderr, "mysql_ping:%d", ping_code);
		}
    }
    return connected_;
}

Statement *MySQLConnection::CreateStatement()
{
    if (!connected_)
    {
        fprintf(stderr, "not connected!\n");
        return NULL;
    }
    return new MySQLStatement(this);
}

PreparedStatement *MySQLConnection::PrepareStatement(const std::string &sql)
{
    if (!connected_)
    {
        fprintf(stderr, "not connected!\n");
        return NULL;
    }
    PreparedStatement* stmt = new MySQLPreparedStatement(this, sql); 
    if (stmt)
    {
        if (!stmt->Init())
        {
            delete stmt;
            stmt = NULL;
        }
    }
    return stmt;
}

void MySQLConnection::SetAutoCommit(bool auto_commit)
{
    auto_commit_ = auto_commit;
    mysql_autocommit(&mysql_, auto_commit);
}

void MySQLConnection::Commit()
{
    if (mysql_commit(&mysql_) != 0)
    {
        fprintf(stderr, "commit failure: %d - %s", GetErrNo(), GetError());
    }
}

bool MySQLConnection::GetAutoCommit()
{
    return auto_commit_;
}

void MySQLConnection::Close()
{
    if (connected_)
    {
        Commit();
        mysql_close(&mysql_);
        connected_ = false;
    }
}

int MySQLConnection::GetErrNo()
{
    return mysql_errno(&mysql_);
}

const char *MySQLConnection::GetError()
{
    return mysql_error(&mysql_);
}

MYSQL *MySQLConnection::GetMySQLHandler()
{
    return &mysql_;
}

MySQLBuffer *MySQLConnection::param_buffer()
{
    return &param_buffer_;
}

MySQLBuffer *MySQLConnection::result_buffer()
{
    return &result_buffer_;
}

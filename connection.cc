#include "connection.h"
#include "statement.h"

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

using namespace mysql;

Connection::Connection(const char *host, 
                       const char *user, 
                       const char *password, 
                       const char *database, 
                       unsigned int port, 
                       unsigned int param_buf_size)
    : conf_(),
      connected_(false),
      ping_counter_(0),
      param_buffer_(param_buf_size),
      result_buffer_(kDefaultResultBufferSize)
{
    if (host != NULL)
        conf_.host = host;
    
    if (user != NULL)
        conf_.user = user;
    
    if (password != NULL)
        conf_.password = password;
    
    if (database != NULL)
        conf_.database = database;
    
    conf_.port = port;
    conf_.param_buf_size = param_buf_size;
    
    if (mysql_init(&mysql_) == NULL)
    {
        fprintf(stderr, "mysql_init error!\n");
        abort();
    }
}

Connection::Connection(DBConf &conf)
    : conf_(conf),
      connected_(false),
      ping_counter_(0),
      param_buffer_(conf_.param_buf_size),
      result_buffer_(conf_.result_buf_size)
    
{
    
}

Connection::~Connection()
{
    Close();
}

bool Connection::Connect()
{
    if (connected_)
        return false;
    
    
    
    mysql_set_character_set(&mysql_, conf_.charset.c_str());
    mysql_options(&mysql_, MYSQL_OPT_READ_TIMEOUT, &conf_.timeout);
    mysql_options(&mysql_, MYSQL_OPT_WRITE_TIMEOUT, &conf_.timeout);
    
    if (mysql_real_connect(&mysql_, 
                           conf_.host.c_str(), 
                           conf_.user.c_str(), 
                           conf_.password.c_str(), 
                           conf_.database.c_str(), 
                           conf_.port, 
                           NULL, 
                           CLIENT_COMPRESS | CLIENT_MULTI_STATEMENTS | conf_.flag) 
            != NULL)
    {
        connected_ = true;
        mysql_autocommit(&mysql_, conf_.autocommit);
    }
    else
    {
        throw MySQLException(GetError());
    }
    return connected_;
}

bool Connection::Reconnect()
{
    Close();
    return Connect();
}

bool Connection::IsConnected()
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

Statement *Connection::CreateStatement()
{
    if (!connected_)
    {
        fprintf(stderr, "not connected!\n");
        return NULL;
    }
    return new MySQLStatement(this);
}

PreparedStatement *Connection::PrepareStatement(const std::string &sql)
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

void Connection::SetAutoCommit(bool auto_commit)
{
    conf_.autocommit = auto_commit;
    mysql_autocommit(&mysql_, auto_commit);
}

void Connection::Commit()
{
    if (mysql_commit(&mysql_) != 0)
    {
        fprintf(stderr, "commit failure: %d - %s", GetErrNo(), GetError());
    }
}

bool Connection::GetAutoCommit()
{
    return conf_.autocommit;
}

void Connection::Close()
{
    if (connected_)
    {
        Commit();
        mysql_close(&mysql_);
        connected_ = false;
    }
}

int Connection::GetErrNo()
{
    return mysql_errno(&mysql_);
}

const char *Connection::GetError()
{
    return mysql_error(&mysql_);
}

MYSQL *Connection::GetMySQLHandler()
{
    return &mysql_;
}

MySQLBuffer *Connection::param_buffer()
{
    return &param_buffer_;
}

MySQLBuffer *Connection::result_buffer()
{
    return &result_buffer_;
}

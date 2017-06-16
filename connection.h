#ifndef CONNECTION_H_
#define CONNECTION_H_

#include "types.h"
#include <string>
#include <mysql/mysql.h>
#include <stdexcept>


namespace mysql 
{

class Statement;
class PreparedStatement;

class MySQLException: public std::exception
{
public:
    MySQLException(const std::string& reason) : reason_(reason)
    {    
    }
    
    virtual ~MySQLException() throw()
    {   
    }
    
    virtual const char* what() const throw()
    {
        return reason_.c_str();
    }
    
private:
    std::string reason_;
};


struct DBConf
{
    std::string host;
    unsigned int port;
    std::string user;
    std::string password;
    std::string database;
    std::string charset;
    int timeout;
    int flag;
    bool autocommit;
    unsigned int param_buf_size;
    unsigned int result_buf_size;
    
    DBConf() : 
        port(3306),
        charset("utf8"), 
        timeout(15), 
        flag(CLIENT_COMPRESS | CLIENT_MULTI_STATEMENTS), 
        autocommit(true),
        param_buf_size(kDefaultParamBufferSize),
        result_buf_size(kDefaultResultBufferSize)
    {}
};

class Connection
{
public:
    Connection(const char* host, 
               const char* user, 
               const char* passwd, 
               const char* database, 
               unsigned int port, 
               unsigned int param_buf_size = kDefaultParamBufferSize);
    
    Connection(DBConf& conf);
    
    ~Connection();
    
    bool Connect();
    bool Reconnect();
    bool IsConnected();
    
    Statement* CreateStatement();
    PreparedStatement* PrepareStatement(const std::string& sql);
    
    void SetAutoCommit(bool auto_commit);
    void Commit();
    bool GetAutoCommit();
    
    void Close();
    
    int GetErrNo();
    const char* GetError();
    
    MYSQL* GetMySQLHandler();
    
    MySQLBuffer* param_buffer() ;
    
    MySQLBuffer* result_buffer() ;
    
    
private:
    DBConf          conf_;
    MYSQL           mysql_;
    bool            connected_;
    int             ping_counter_;
    MySQLBuffer     param_buffer_;
    MySQLBuffer     result_buffer_;
};

}

#endif

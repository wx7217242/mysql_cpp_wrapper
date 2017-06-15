#ifndef CONNECTION_H_
#define CONNECTION_H_

#include "types.h"
#include <string>
#include <mysql/mysql.h>
#include <stdexcept>

namespace mysql {



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

class Connection
{
public:
    Connection(const char* host, 
                    const char* user, 
                    const char* passwd, 
                    const char* database, 
                    short port, 
                    uint32_t param_buf_size = kDefaultParamBufferSize, 
                    uint32_t result_buf_size = kDefaultResultBufferSize);
    virtual ~Connection();
    
    virtual bool Connect(const char* charset, unsigned int timeout, bool auto_commit);
    virtual bool Reconnect();
    virtual bool IsConnected();
    
    virtual Statement* CreateStatement();
    virtual PreparedStatement* PrepareStatement(const std::string& sql);
    
    virtual void SetAutoCommit(bool auto_commit);
    virtual void Commit();
    virtual bool GetAutoCommit();
    
    virtual void Close();
    
    virtual int GetErrNo();
    virtual const char* GetError();
    
    MYSQL* GetMySQLHandler();
    
    MySQLBuffer* param_buffer() ;
    
    MySQLBuffer* result_buffer() ;
    
    
private:
    MYSQL mysql_;
    bool connected_;
    std::string host_;
    std::string user_;
    std::string passwd_;
    std::string db_;
    short port_;
    int ping_counter_;
    std::string charset_;
    unsigned int timeout_;
    bool auto_commit_;
    
    MySQLBuffer param_buffer_;
    MySQLBuffer result_buffer_;
};

}

#endif
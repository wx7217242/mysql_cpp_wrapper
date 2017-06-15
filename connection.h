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

#define PTR_OR_EMPTY(ptr) (ptr != NULL ? ptr : "")

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
    uint32_t param_buf_size;
    uint32_t result_buf_size;
    DBConf(const char* h,
           unsigned int p,
           const char* u,
           const char* pw,
           const char* db,
           const char* cs) :
        host(PTR_OR_EMPTY(h)),
        port(p),
        user(PTR_OR_EMPTY(u)),
        password(PTR_OR_EMPTY(pw)),
        database(PTR_OR_EMPTY(db)),
        charset(PTR_OR_EMPTY(cs)),
        timeout(),
        flag(),
        autocommit(),
        param_buf_size(),
        result_buf_size()
    {
    }
};

class Connection
{
public:
    Connection(const char* host, 
               const char* user, 
               const char* passwd, 
               const char* database, 
               unsigned int port, 
               uint32_t param_buf_size = kDefaultParamBufferSize, 
               uint32_t result_buf_size = kDefaultResultBufferSize);
               
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

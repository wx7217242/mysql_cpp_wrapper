#ifndef CONNECTION_H_
#define CONNECTION_H_

#include <string>
#include <vector>
#include <stdexcept>

#include <string.h>

#include <mysql/mysql.h>

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

const int kDefaultParamBufferSize = 1024; 
const int kDefaultResultBufferSize = 10 * 1024; 
const int kMaxBufferSize = 10 * 1024 * 1024;

class MySQLBuffer
{
public:
    MySQLBuffer(size_t size) : 
        buffer_size_(size),
        buffer_cur_pos_(0)
    {
        buffer_.resize(buffer_size_);
    }
    
    ~MySQLBuffer()
    {
        
    }
    
    bool Append(const void* param, size_t size)
    {
        if (buffer_cur_pos_ + size > buffer_size_)
            return false;
        
        memcpy(buffer_from_current(), param, size);
        return true;
    }
    
    void ResetBuffer()
    {
        buffer_cur_pos_ = 0;
        buffer_.clear();
    }
    
    char* buffer() { return buffer_.data(); }
    
    char* buffer_from_current() { return buffer_.data() + buffer_cur_pos_; }
    
    size_t buffer_size() const { return buffer_size_; }
    
    size_t buffer_cur_pos() const { return buffer_cur_pos_; }
    
    bool IsCapacityAvailable(size_t size) const
    {
        return buffer_cur_pos_ + size <= buffer_size_;
    }
    
    bool IncreaseBufferCurPos(size_t offset)
    {
        if (IsCapacityAvailable(offset))
        {
            buffer_cur_pos_ += offset;
            return true;
        }
        
        return false;
    }
    
    bool ExtendBuffer()
    {
        if (buffer_size_ + (buffer_size_ << 1) >= kMaxBufferSize)
        {
            return false;
        }
        
        buffer_size_ += (buffer_size_ << 1);
        buffer_.resize(buffer_size_);
        return true;
    }
    
private:
    std::vector<char>   buffer_;
    size_t              buffer_size_;
    size_t              buffer_cur_pos_;
};


struct DBConf
{
    std::string     host;
    unsigned int    port;
    std::string     user;
    std::string     password;
    std::string     database;
    std::string     charset;
    int             timeout;
    int             flag;
    bool            autocommit;
    unsigned int    param_buf_size;
    unsigned int    result_buf_size;
    
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

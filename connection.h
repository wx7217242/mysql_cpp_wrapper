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

/**
  * @brief MySQL操作的异常类
*/
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

const size_t kDefaultParamBufferSize = 1024;           // 默认一条SQL的长度
const size_t kDefaultResultBufferSize = 10 * 1024;     // 默认一条结果集的长度
const size_t kMaxBufferSize = 10 * 1024 * 1024;        // 缓冲区的最大长度

/**
 * @brief MySQLBuffer 类，用于保存PreparedStatemt的参数和PreparedStatemtResult的结果
*/
class MySQLBuffer
{
public:
    MySQLBuffer(size_t size) : 
        buffer_size_(std::min(size, kMaxBufferSize)),
        buffer_cur_pos_(0)
    {
        buffer_.resize(buffer_size_);
    }
    
    ~MySQLBuffer()
    {
        
    }
    
    /**
      * @brief 把参数追加到Buffer中，成功需要调用IncreaseBufferCurPos
      * @return 成功返回true；失败返回false；
    */
    bool Append(const void* param, size_t size)
    {
        if (buffer_cur_pos_ + size > buffer_size_)
            return false;
        
        memcpy(buffer_from_current(), param, size);
        return true;
    }
    
    /**
      * @brief 重置Buffer
    */
    void ResetBuffer()
    {
        buffer_cur_pos_ = 0;
        buffer_.clear();
    }
    
    /**
      * @brief 获取Buffer的头指针
    */
    char* buffer() { return buffer_.data(); }
    
    /**
      * @brief 获取Buffer的当前位置的指针
    */
    char* buffer_from_current() { return buffer_.data() + buffer_cur_pos_; }
    
    /**
      * @brief 获取Buffer的大小
    */
    size_t buffer_size() const { return buffer_size_; }
    
    /**
      * @brief 获取Buffer的当前位置
    */
    size_t buffer_cur_pos() const { return buffer_cur_pos_; }
    
    /**
      * @brief 判断Buffer的容量时候足够
    */
    bool IsCapacityAvailable(size_t size) const
    {
        return buffer_cur_pos_ + size <= buffer_size_;
    }
    
    /**
      * @brief 移动Buffer的当前位置
    */
    bool IncreaseBufferCurPos(size_t offset)
    {
        if (IsCapacityAvailable(offset))
        {
            buffer_cur_pos_ += offset;
            return true;
        }
        
        return false;
    }
    
    /**
      * @brief 扩展Buffer的容量，默认是当前容量的两倍；最大不超过 kMaxBufferSize
    */
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

/**
 * @brief 数据库连接类，一个线程一个Connection
 * @brief 由此连接创建的PreparedStatement共享param_buffer_
 * @brief 由此连接创建的PreparedStatementResultSet共享result_buffer_
 * @brief 
*/
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

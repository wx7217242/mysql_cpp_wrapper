#ifndef MYSQL_CONNECTION_POOL_H_
#define MYSQL_CONNECTION_POOL_H_

#include <string>
#include <list>
#include <boost/thread/mutex.hpp>

namespace mysql
{

class Connection;

/**
 * @brief MySQL数据库连接池，线程安全
 * 依赖boost::mutex 、boost::lock_guard
*/
class ConnectionPool
{
public:
    /**
     * 连接链表
    */
    typedef std::list<Connection*>      ConnectionList;
    typedef ConnectionList::iterator    ConnectionListIter;
    
    /**
     * @brief 单例类
    */
    static ConnectionPool& GetInstance();

    /**
     * @brief 创建连接数为最大连接数一半的连接
    */
    void InitConnectionPool(const std::string& host, 
                            int port, 
                            const std::string& user, 
                            const std::string& password, 
                            const std::string& database, 
                            int max_pool_size, 
                            int init_size);
    
    /**
     * @brief 获取一个空闲的连接
    */
    Connection* GetConnection();
    
    /**
     * @brief 把连接放回池中
    */
    void ReleaseConnection(Connection* connection);
    
private:
    ConnectionPool();
	~ConnectionPool();
    
    ConnectionPool(const ConnectionPool&);
    ConnectionPool& operator=(const ConnectionPool&);
    
    /**
     * @brief 创建一个连接
    */
    Connection* CreateConnection();
    
    /**
     * @brief 销毁数据库连接池
    */
    void DestroyConnectionPool();
    
    /**
     * @brief 把连接放回到空闲的连接链表中
    */
    void DestroyConnection(Connection* connection);
    
private:
    /**
     * 是否初始化
    */
    bool                initialized_;
    
    /**
     * 数据库地址
    */
    std::string         host_;
    
    /**
     * 数据库端口
    */
    int                 port_;
    
    /**
     * 数据库用户名
    */
    std::string         user_;
    
    /**
     * 数据库密码
    */
    std::string         password_;
    
    /**
     * 数据库名
    */
    std::string         database_;
    
    /**
     * 连接池最大容量
    */
    int                 max_pool_size_;
    
    /**
     * 当前连接池的大小
    */
    int                 current_pool_size_;
    
    /**
     * 空闲的数据库连接链表
    */
    ConnectionList      idle_conns_;
    
    /**
     * 使用中的数据库连接链表
    */
    ConnectionList      occupiped_conns_;
    
    /**
     * 互斥锁
    */
    boost::mutex        mutex_;
};

}

#endif // MYSQL_CONNECTION_POOL_H_
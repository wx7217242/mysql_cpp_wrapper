#ifndef MYSQL_CONNECTION_POOL_H_
#define MYSQL_CONNECTION_POOL_H_

#include <string>
#include <list>

#include <boost/thread/mutex.hpp>

class MySQLConnection;

class MySQLConnectionPool
{
public:
    typedef std::list<MySQLConnection*> ConnectionList;
    typedef ConnectionList::iterator ConnectionListIter;
    
    static MySQLConnectionPool& GetInstance();

    // 创建连接数为最大连接数一半的连接
    void InitConnectionPool(const std::string& ip, int port, const std::string& user, const std::string& password, 
                            const std::string& database, int max_pool_size, int init_size);
    
    MySQLConnection* GetConnection();
    
    // 把连接放回池中
    void ReleaseConnection(MySQLConnection* connection);
    
private:
    MySQLConnectionPool();
	~MySQLConnectionPool();
    
    MySQLConnectionPool(const MySQLConnectionPool&);
    MySQLConnectionPool& operator=(const MySQLConnectionPool&);
    
    MySQLConnection* CreateConnection();
    void DestroyConnectionPool();
    void DestroyConnection(MySQLConnection* connection);

    std::string ip_;
    int port_;
    std::string user_;
    std::string password_;
    std::string database_;
    
    int max_pool_size_;
    int current_pool_size_;
    
    ConnectionList idle_conns_;
    ConnectionList occupiped_conns_;
    
    boost::mutex mutex_;
};

#endif // MYSQL_CONNECTION_POOL_H_

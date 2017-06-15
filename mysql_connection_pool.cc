#include "mysql_connection_pool.h"

#include <stdio.h>
#include "mysql_exception.h"
#include "mysql_connection.h"
#include <boost/thread/lock_guard.hpp>

MySQLConnectionPool::MySQLConnectionPool() : current_pool_size_(0)
{
}

MySQLConnectionPool::~MySQLConnectionPool()
{
    DestroyConnectionPool();
}

MySQLConnectionPool& MySQLConnectionPool::GetInstance()
{
    static MySQLConnectionPool s_Instance;
    return s_Instance;
}

void MySQLConnectionPool::InitConnectionPool(const std::string &ip, int port, const std::string &user, 
                                             const std::string &password, const std::string &database, int max_pool_size, int init_size)
{
    boost::lock_guard<boost::mutex> guard(mutex_);
    
    ip_ = ip;
    port_ = port;
    user_ = user;
    password_ = password;
    database_ = database;
    max_pool_size_ = max_pool_size;
    
    assert(init_size <= max_pool_size_);
    
    MySQLConnection * connection = NULL;
    while (current_pool_size_ < init_size)
    {
        if ((connection = CreateConnection()) != NULL)
        {
            idle_conns_.push_back(connection); 
            ++current_pool_size_;
        }
    }
}

MySQLConnection *MySQLConnectionPool::GetConnection()
{
    boost::lock_guard<boost::mutex> guard(mutex_);
    
    MySQLConnection* connection = NULL;
    if (!idle_conns_.empty())
    {
        connection = idle_conns_.front();
        idle_conns_.pop_front();
        
        if (!connection->IsConnected())
        {
            delete connection;
            connection = CreateConnection();
        }
        if (connection == NULL)
            --current_pool_size_;
        else
            occupiped_conns_.push_back(connection);
        
        return connection;
    }
    else 
    {
         if (current_pool_size_ < max_pool_size_)
         {
             if ((connection = CreateConnection()) != NULL)
             {
                 ++current_pool_size_;
                 occupiped_conns_.push_back(connection);
             }
             return connection;
         }
         else
         {
             return NULL;
         }
    }
}

void MySQLConnectionPool::ReleaseConnection(MySQLConnection *connection)
{
    boost::lock_guard<boost::mutex> guard(mutex_);
    
    if (connection != NULL)
    {
        occupiped_conns_.remove(connection);
        idle_conns_.push_back(connection);
    }
}

MySQLConnection *MySQLConnectionPool::CreateConnection()
{
    MySQLConnection * connection = NULL;

    try
    {
        connection = new MySQLConnection(ip_.c_str(), user_.c_str(), password_.c_str(), database_.c_str(), port_);
        connection->Connect("utf8", 15, true);
    }
    catch (MySQLException & e)
    {
        fprintf(stderr, "%s\n", e.what());
    }
    
    return connection;
}

void MySQLConnectionPool::DestroyConnectionPool()
{
    ConnectionListIter iter = idle_conns_.begin();
    while (iter != idle_conns_.end())
    {
        DestroyConnection(*iter);
        ++iter;
    }
    iter = occupiped_conns_.begin();
    while (iter != occupiped_conns_.end())
    {
        DestroyConnection(*iter);
        ++iter;
    }
    
    idle_conns_.clear();
    occupiped_conns_.clear();
    current_pool_size_ = 0;
}

void MySQLConnectionPool::DestroyConnection(MySQLConnection* connection)
{
    if (connection)
    {
        try
        {
            connection->Close();
        }
        catch (MySQLException & e)
        {
            fprintf(stderr, "%s\n", e.what());
        }
        delete connection;
    }
}

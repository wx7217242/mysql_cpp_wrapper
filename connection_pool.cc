#include "connection_pool.h"

#include <stdio.h>
#include "connection.h"
#include <boost/thread/lock_guard.hpp>

using namespace mysql;

ConnectionPool::ConnectionPool() 
 :  initialized_(false),
    current_pool_size_(0)
{
}

ConnectionPool::~ConnectionPool()
{
    DestroyConnectionPool();
}

ConnectionPool& ConnectionPool::GetInstance()
{
    static ConnectionPool s_Instance;
    return s_Instance;
}

void ConnectionPool::InitConnectionPool(const std::string &host, 
                                        int port, 
                                        const std::string &user, 
                                        const std::string &password, 
                                        const std::string &database, 
                                        int max_pool_size, 
                                        int init_size)
{
    boost::lock_guard<boost::mutex> guard(mutex_);
    
    if (initialized_)
        return;
    
    host_ = host;
    port_ = port;
    user_ = user;
    password_ = password;
    database_ = database;
    max_pool_size_ = max_pool_size;
    
    assert(init_size <= max_pool_size_);
    
    Connection * connection = NULL;
    while (current_pool_size_ < init_size)
    {
        if ((connection = CreateConnection()) != NULL)
        {
            idle_conns_.push_back(connection); 
            ++current_pool_size_;
        }
    }
    initialized_ = true;
}

Connection *ConnectionPool::GetConnection()
{
    boost::lock_guard<boost::mutex> guard(mutex_);
    
    if (!initialized_)
        return NULL;

    Connection* connection = NULL;
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

void ConnectionPool::ReleaseConnection(Connection *connection)
{
    boost::lock_guard<boost::mutex> guard(mutex_);
    
    if (!initialized_ || connection != NULL)
    {
        occupiped_conns_.remove(connection);
        idle_conns_.push_back(connection);
    }
}

Connection *ConnectionPool::CreateConnection()
{
    Connection * connection = NULL;

    try
    {
        connection = new Connection(host_.c_str(), user_.c_str(), password_.c_str(), database_.c_str(), port_);
        connection->Connect();
    }
    catch (MySQLException & e)
    {
        fprintf(stderr, "%s\n", e.what());
    }
    
    return connection;
}

void ConnectionPool::DestroyConnectionPool()
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

void ConnectionPool::DestroyConnection(Connection* connection)
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
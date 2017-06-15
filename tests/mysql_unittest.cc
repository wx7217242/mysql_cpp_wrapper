
#include <stdexcept>

#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "../connection.h"
#include "../resultset.h"
#include "../statement.h"


#include <iostream>
using namespace std;
using namespace mysql;

const char* host = "192.168.157.1";
const char* user = "root";
const char* passwd = "root";
const char* database = "mysql_test";
short port = 3306;

/*
+---------+---------------------+------+-----+---------+-------+
| Field   | Type                | Null | Key | Default | Extra |
+---------+---------------------+------+-----+---------+-------+
| id      | int(11)             | YES  |     | NULL    |       |
| name    | varchar(20)         | YES  |     | NULL    |       |
| sex     | tinyint(1)          | YES  |     | NULL    |       |
| age     | int(10) unsigned    | YES  |     | NULL    |       |
| money   | bigint(20)          | YES  |     | NULL    |       |
| diamond | bigint(20) unsigned | YES  |     | NULL    |       |
| ext     | blob                | YES  |     | NULL    |       |
+---------+---------------------+------+-----+---------+-------+

*/

void statement_resultset_test()
{
    Connection* conn = NULL;
    Statement* stmt = NULL;
    ResultSet* resultset = NULL;
    try
    {
        do
        {
            conn  = new Connection(host, user, passwd, database, port);
            if (!conn->Connect("utf8", 15, false))
            {
                printf("Connect error!\n");
                break;
            }
            
            if (!conn->IsConnected())
            {
                printf("not connected error!\n");
                break;
            }
            stmt = conn->CreateStatement();
            if (stmt == NULL)
            {
                printf("CreateStatement error!\n");
                break;
            }
             printf("CreateStatement success!\n");
             
            const char* sql = "insert into mysql_test(name, sex, age, money, diamond, ext) values('nini', 1, 20, 3333333, 44444444, '1234554321');";
             if (!stmt->Execute(sql))
            {
                printf("insert failed!\n");
                break;
            }
            else
                 printf("insert success!\n");
             
//             conn->Commit();
            
            sql = "select id, name, sex, age, money, diamond, ext from mysql_test;";
            resultset = stmt->ExecuteQuery(sql);
            if (resultset == NULL)
            {
                printf("ExecuteQuery failed!\n");
                break;
            }
             printf("ExecuteQuery success!\n");
            
            char name[32] = {0};
            char buffer[1024] = {0};
            while (resultset->Next())
            {
                int id = resultset->GetInt(0, 0);
                uint32_t name_len = resultset->GetString(1, name, sizeof(name));
                bool sex = resultset->GetBoolean(2, false);
                int age = resultset->GetInt(3, 10);
                int64_t money = resultset->GetInt64(4, 100);
                uint64_t diamond = resultset->GetUInt64(5, 100);
                uint32_t ext_len = resultset->GetBlob(6, buffer, sizeof(buffer)); 
                
                std::cout << "id : " << id << std::endl;
                std::cout << "name : " << name << "\t, length is " << name_len << std::endl;
                std::cout << "sex : " << ((sex) ? "male" : "female") << std::endl;
                std::cout << "age : " << age << std::endl;
                std::cout << "money : " << money << std::endl;
                std::cout << "diamond : " << diamond << std::endl;
                std::cout << "ext : " << buffer << "\t, length is " << ext_len << std::endl;
            }
            
            int res = stmt->ExecuteUpdate("update mysql_test set id = 8;");
            if (res < 0)
            {
                printf("update error!\n");
                break;
            }
            else
            {
                printf("affected rows %d\n", res);
            }
            
        }while (false);
    }
    catch (MySQLException& e)
    {
        printf("catch MySQLException:  %s\n", e.what());
    }
    catch(std::exception& e)
    {
        printf("catch std;:exception:  %s\n", e.what());
    }
    catch(...)
    {
        printf("catch other exception\n");
    }
    
    if (resultset != NULL)
        delete resultset;
    
    if (stmt != NULL)
        delete stmt;
    
    if (conn != NULL)
        delete conn;
}

void preparedstatement_resultset_test()
{
    Connection* conn = NULL;
    PreparedStatement* stmt = NULL;
    ResultSet* resultset = NULL;
    try
    {
        do
        {
            conn  = new Connection(host, user, passwd, database, port);
            if (!conn->Connect("utf8", 15, true))
            {
                printf("Connect error!\n");
                break;
            }
            
            if (!conn->IsConnected())
            {
                printf("not connected error!\n");
                break;
            }
            
            const char* prepared_sql = "insert into mysql_test(name, sex, age, money, diamond, ext) values(?, ?, ?, ?, ?, ?);";
            stmt = conn->PrepareStatement(prepared_sql);
            if (stmt == NULL)
            {
                printf("PrepareStatement error!\n");
                break;
            }
             printf("PrepareStatement success!\n");
             
             stmt->SetString(0, "momo");
             stmt->SetBoolean(1, true);
             stmt->SetInt(2, 22);
             stmt->SetInt64(3, 30000);
             stmt->SetUInt64(4, 40000);
             stmt->SetBlob(5, "momomomomomo", 12);
            
             if (!stmt->Execute())
            {
                printf("insert failed!\n");
                break;
            }
            else
            {
                  MySQLPreparedStatement* pre_stmt = static_cast<MySQLPreparedStatement*>(stmt);
                  printf("insert success， affected rows %d !\n", pre_stmt->GetAffectedRows());
             }
             
             conn->Commit();
            
             delete stmt; stmt = NULL;
             const char* sql = "select id, name, sex, age, money, diamond, ext from mysql_test where id > ?;";
             stmt = conn->PrepareStatement(sql);
             if (stmt == NULL)
             {
                 printf("PrepareStatement error!\n");
                 break;
             }
              printf("PrepareStatement success!\n");
              
            stmt->SetInt(0, 0);
            resultset = stmt->ExecuteQuery();
            if (resultset == NULL)
            {
                printf("ExecuteQuery failed!\n");
                break;
            }
             printf("ExecuteQuery success!\n");
            
            char name[32] = {0};
            char buffer[1024] = {0};
            while (resultset->Next())
            {
                memset(name, 0, sizeof(name));
                memset(buffer, 0, sizeof(buffer));
                int id = resultset->GetInt(0, 0);
                uint32_t name_len = resultset->GetString(1, name, sizeof(name));
                bool sex = resultset->GetBoolean(2, false);
                int age = resultset->GetInt(3, 10);
                int64_t money = resultset->GetInt64(4, 100);
                uint64_t diamond = resultset->GetUInt64(5, 100);
                uint32_t ext_len = resultset->GetBlob(6, buffer, sizeof(buffer)); 
                
                std::cout << "id : " << id << std::endl;
                std::cout << "name : " << name << "\t, length is " << name_len << std::endl;
                std::cout << "sex : " << ((sex) ? "male" : "female") << std::endl;
                std::cout << "age : " << age << std::endl;
                std::cout << "money : " << money << std::endl;
                std::cout << "diamond : " << diamond << std::endl;
                std::cout << "ext : " << buffer << "\t, length is " << ext_len << std::endl;
            }
            
            if (resultset != NULL)
            {
                delete resultset; 
                resultset= NULL;
            }
            
            if (stmt != NULL)
            {
                delete stmt; 
                stmt= NULL;
            }
            sql = "update mysql_test set id = ?";
            stmt = conn->PrepareStatement(sql);
            if (stmt == NULL)
            {
                printf("PrepareStatement error!\n");
                break;
            }
             printf("PrepareStatement success!\n");
             
            stmt->SetInt(0, 10);
           int res = stmt->ExecuteUpdate();
            if (res < 0)
            {
                printf("update error!\n");
                break;
            }
            else
            {
                printf("affected rows %d\n", res);
            }
            
        }while (false);
    }
    catch (MySQLException& e)
    {
        printf("catch MySQLException:  %s\n", e.what());
    }
    catch(std::exception& e)
    {
        printf("catch std;:exception:  %s\n", e.what());
    }
    catch(...)
    {
        printf("catch other exception\n");
    }
    
    if (resultset != NULL)
        delete resultset;
    
    if (stmt != NULL)
        delete stmt;
    
    if (conn != NULL)
        delete conn;
            
}

int main()
{
//    statement_resultset_test();
    preparedstatement_resultset_test();
    
    return 0;
}

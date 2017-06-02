
#include <stdexcept>

#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "../mysql_connection.h"
#include "../mysql_exception.h"
#include "../mysql_prepared_statement.h"
#include "../mysql_resultset.h"
#include "../mysql_statement.h"


#include <iostream>
using namespace std;

const char* host = "192.168.157.1";
const char* user = "root";
const char* passwd = "root";
const char* database = "center_ans_sec";
short port = 3306;


int main()
{
    Connection* conn = NULL;
    PreparedStatement* stmt = NULL;
    ResultSet* result = NULL;
    
    try
    {
        do
        {
            conn = new MySQLConnection(host, user, passwd, database, port);
            if (conn == NULL)
            {
                break;
            }
            
            if (!conn->Connect("utf8", 15, true))
            {
                break;
            }
            
            stmt = conn->PrepareStatement("select * from tb_country_map order by country_code_num;");
            if (!stmt)
            {
                break;
            }
            
            result = stmt->ExecuteQuery();
            if (!result)
            {
                break;
            }
            
//            char country_code_two[16];
//            char country_code_three[16];
            int country_code_num;
//            char country_code_name[64];
//            char regin_code[16];
            int col = 0;
            while (result->Next())
            {
//                memset(country_code_two, 0x00, sizeof(country_code_two));
//                memset(country_code_three, 0x00, sizeof(country_code_three));
//                memset(country_code_name, 0x00, sizeof(country_code_name));
//                memset(regin_code, 0x00, sizeof(regin_code));
                
//                result->GetString(0, country_code_two, sizeof(country_code_two));
                std::string country_code_two = result->GetString(0);
                std::string country_code_three = result->GetString(1);
//                result->GetString(1, country_code_three, sizeof(country_code_three));
                country_code_num = result->GetInt(2, 0);
                std::string country_code_name = result->GetString(3);
                std::string regin_code = result->GetString(4);
                
                printf("%s, %s, %d, %s, %s\n", country_code_two.c_str(), country_code_three.c_str(), 
                       country_code_num, country_code_name.c_str(), regin_code.c_str());
                ++col;
            }
            
            printf("total col is %d\n", col);
        } while (false);
    }
    catch(MySQLException& e)
    {
        printf("catch exception : %s\n", e.what());
    }        
    
    if (result != NULL)
        delete result;
    if (stmt != NULL)
        delete stmt;
    if (conn != NULL)
        delete conn;
    
    
    return 0;
}

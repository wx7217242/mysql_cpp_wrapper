
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
const char* database = "center_ans_sec";
short port = 3306;


int main()
{
    Connection* conn = NULL;
    Statement* stmt = NULL;
    ResultSet* result = NULL;
    
    try
    {
        do
        {
            conn = new Connection(host, user, passwd, database, port);
            if (conn == NULL)
            {
                break;
            }
            
            if (!conn->Connect("utf8", 15, true))
            {
                break;
            }
            
            stmt = conn->CreateStatement();
            if (!stmt)
            {
                break;
            }
            
            result = stmt->ExecuteQuery("select * from tb_authcode;");
            if (!result)
            {
                break;
            }
            
            char auth_guid[40];
            char valid_time[40];
            int userid;
            char exit_groups_id[128];
            char desc_info[256];
            while (result->Next())
            {
                memset(auth_guid, 0x00, sizeof(auth_guid));
                memset(valid_time, 0x00, sizeof(valid_time));
                memset(exit_groups_id, 0x00, sizeof(exit_groups_id));
                memset(desc_info, 0x00, sizeof(desc_info));
                
                result->GetString(0, auth_guid, sizeof(auth_guid));
                result->GetString(1, valid_time, sizeof(valid_time));
                userid = result->GetInt(2, 0);
                result->GetString(3, exit_groups_id, sizeof(exit_groups_id));
                result->GetString(4, desc_info, sizeof(desc_info));
                
                printf("%s, %s, %d, %s, %s\n", auth_guid, valid_time, userid, exit_groups_id, desc_info);
            }
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

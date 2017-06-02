#ifndef MYSQL_STATEMENT_H_
#define MYSQL_STATEMENT_H_

#include "statement.h"
#include "types.h"

class MySQLConnection;

class MySQLStatement : public Statement
{
public:
    MySQLStatement(MySQLConnection* conn);
    virtual ~MySQLStatement();
    
    virtual Connection* GetConnection();
    
    virtual bool Init();
    
//      virtual void Cancel() = 0;
//      virtual void ClearWarnings() = 0;
//      virtual void Close() = 0;
    
    virtual bool Execute(const std::string& sql);
    virtual ResultSet* ExecuteQuery(const std::string& sql);
    virtual ResultSet* GetResultSet();
    virtual int ExecuteUpdate(const std::string& sql);
    virtual bool GetMoreResults();
    unsigned long long GetLastInsertID();
    
private:
    MySQLConnection* connection_;
    ResultSet* resultset_;
    unsigned long long affected_rows_;
    unsigned long long insert_id_; 
};

#endif
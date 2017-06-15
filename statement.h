#ifndef STATEMENT_H_
#define STATEMENT_H_

#include <string>

#include "types.h"
#include <vector>
#include <mysql/mysql.h>

namespace mysql 
{

class Connection;
class ResultSet;

class Statement 
{
public:
      Statement() {}
      virtual ~Statement() {}
      
      virtual Connection* GetConnection() = 0;
      
      virtual bool Init() = 0;
      
//      virtual void Cancel() = 0;
//      virtual void ClearWarnings() = 0;
//      virtual void Close() = 0;
      
      virtual bool Execute(const std::string& sql) = 0;
      virtual ResultSet * ExecuteQuery(const std::string& sql) = 0;
      virtual ResultSet* GetResultSet() = 0;
      virtual int ExecuteUpdate(const std::string& sql) = 0;
      virtual bool GetMoreResults() = 0;
      
//      virtual size_t GetFetchSize() = 0;
//      virtual unsigned int GetMaxFieldSize() = 0;
//      virtual uint64_t GetMaxRows() = 0;
//      virtual unsigned int GetQueryTimeout() = 0;
//      virtual sql::ResultSet::enum_type getResultSetType() = 0;
//      virtual uint64_t getUpdateCount() = 0;
//      virtual const SQLWarning * getWarnings() = 0;
//      virtual void setCursorName(const sql::SQLString & name) = 0;
//      virtual void setEscapeProcessing(bool enable) = 0;
//      virtual void setFetchSize(size_t rows) = 0;
//      virtual void setMaxFieldSize(unsigned int max) = 0;
//      virtual void setMaxRows(unsigned int max) = 0;      
//      virtual void setQueryTimeout(unsigned int seconds) = 0;
//      virtual Statement * setResultSetType(sql::ResultSet::enum_type type) = 0;
      
};

class PreparedStatement : public Statement
{
public:
      PreparedStatement() {}
      virtual ~PreparedStatement() {}      
      
      virtual bool Execute(const std::string& sql) = 0;
      virtual bool Execute() = 0;
      
      virtual ResultSet * ExecuteQuery(const std::string& sql) = 0;
      virtual ResultSet * ExecuteQuery() = 0;
      
      virtual int ExecuteUpdate(const std::string& sql) = 0;
      virtual int ExecuteUpdate() = 0;
      
      virtual bool GetMoreResults() = 0;
      
      virtual bool SetBoolean(uint32_t param_idx, bool value) = 0;
      
      virtual bool SetInt(uint32_t param_idx, int32_t value) = 0;
      
      virtual bool SetUInt(uint32_t param_idx, uint32_t value) = 0;
      
      virtual bool SetBigInt(uint32_t param_idx, int64_t value) = 0;
      
      virtual bool SetInt64(uint32_t param_idx, int64_t value) = 0;
      
      virtual bool SetUInt64(uint32_t param_idx, uint64_t value) = 0;
      
      virtual bool SetDouble(uint32_t param_idx, double value) = 0;
      
      virtual bool SetBlob(uint32_t param_idx, const char* blob, uint32_t length) = 0;
      
      virtual bool SetString(uint32_t param_idx, const char* value) = 0;
      
      virtual bool SetDateTime(uint32_t param_idx, const char* value) = 0;

      virtual bool SetNull(uint32_t param_idx, int sql_type) = 0;      
};

class MySQLStatement : public Statement
{
public:
    MySQLStatement(Connection* conn);
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
    Connection* connection_;
    ResultSet* resultset_;
    unsigned long long affected_rows_;
    unsigned long long insert_id_; 
};



class MySQLPreparedStatement : public PreparedStatement
{
public:
    MySQLPreparedStatement(Connection* conn, const std::string& sql) ;
    virtual ~MySQLPreparedStatement() ;
    
    virtual Connection* GetConnection() ;
    
    virtual bool Init();
    
    virtual ResultSet* GetResultSet() ;
    
    virtual bool Execute(const std::string& sql) ;
    virtual bool Execute() ;
    
    virtual ResultSet * ExecuteQuery(const std::string& sql) ;
    virtual ResultSet * ExecuteQuery() ;
    
    virtual int ExecuteUpdate(const std::string& sql) ;
    virtual int ExecuteUpdate() ;
    
    virtual bool GetMoreResults() ;
    
    virtual bool SetBoolean(uint32_t param_idx, bool value) ;
    
    virtual bool SetInt(uint32_t param_idx, int32_t value) ;
    
    virtual bool SetUInt(uint32_t param_idx, uint32_t value) ;
    
    virtual bool SetBigInt(uint32_t param_idx, int64_t value) ;
    
    virtual bool SetInt64(uint32_t param_idx, int64_t value) ;
    
    virtual bool SetUInt64(uint32_t param_idx, uint64_t value) ;
    
    virtual bool SetDouble(uint32_t param_idx, double value) ;
    
    virtual bool SetBlob(uint32_t param_idx, const char* blob, uint32_t length) ;
    
    virtual bool SetString(uint32_t param_idx, const char* value) ;
    
    virtual bool SetDateTime(uint32_t param_idx, const char* value) ;

    virtual bool SetNull(uint32_t param_idx, int sql_type) ;    
    
    int GetErrNo();
    const char* GetError();
    
    unsigned long long GetLastInsertID() const;
    
    int GetAffectedRows() const;
    
    MYSQL_STMT* GetMySQLStmtHandler() const;
    
    
private:
    bool SetParamToBuffer(uint32_t param_idx, enum_field_types type, const void* value, uint64_t size, bool is_unsigned) ;
    
private:
    Connection* connection_;
    std::string sql_;
    ResultSet* resultset_;
    unsigned long long insert_id_;
    int affected_rows_;
    
    MYSQL_STMT* mysql_stmt_;
    std::vector<MYSQL_BIND> params_;
    
    unsigned long cur_param_idx_;
    unsigned long param_count_;
    
    MySQLBuffer* param_buffer_;    
};

}


#endif
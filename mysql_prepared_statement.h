#ifndef MYSQL_PREPARED_STATEMENT_H_
#define MYSQL_PREPARED_STATEMENT_H_

#include "prepared_statement.h"
#include <vector>
#include <mysql/mysql.h>

class MySQLBuffer;
class MySQLConnection;

class MySQLPreparedStatement : public PreparedStatement
{
public:
    MySQLPreparedStatement(MySQLConnection* conn, const std::string& sql) ;
    virtual ~MySQLPreparedStatement() ;
    
    virtual Connection* GetConnection() ;
    
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
    MySQLConnection* connection_;
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

#endif
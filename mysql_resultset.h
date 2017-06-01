#ifndef MYSQL_RESULTSET_H_
#define MYSQL_RESULTSET_H_

#include "resultset.h"
#include <vector>
#include <mysql/mysql.h>

class MySQLStatement;

class MySQLResultSet : public ResultSet
{
public:
    MySQLResultSet(MySQLStatement* stmt);
    virtual ~MySQLResultSet();
    
    virtual bool Next();
    
    virtual const Statement* GetStatement() const;
    
    virtual uint32_t GetBlob(uint32_t idx, char* buffer, uint32_t max_buf_len)  const;
    
    virtual bool GetBoolean(uint32_t idx, bool def) const;
    
    virtual double GetDouble(uint32_t idx, double def) const;
    
    virtual int32_t GetInt(uint32_t idx, int32_t def)  const;
    
    virtual uint32_t GetUInt(uint32_t idx, uint32_t def) const;
    
    virtual int64_t GetInt64(uint32_t idx, int64_t def) const;
    
    virtual uint64_t GetUInt64(uint32_t idx, uint64_t def) const;
    
    virtual uint32_t GetString(uint32_t idx, char* buffer, uint32_t max_buf_len)  const;
    
    virtual bool IsNull(uint32_t idx) const;
    
private:
    bool StoreResults();
    
private:
    MySQLStatement* stmt_;
    
    MYSQL_STMT* mysql_stmt_;
    MYSQL_RES	* mysql_res_;
    MYSQL_ROW cur_row_;
    MYSQL_FIELD* mysql_fields;
    uint32_t field_count_;
    uint64_t rows_;
    bool	has_result;
    uint64_t* fields_length_;
};

#endif
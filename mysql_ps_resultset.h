#ifndef MYSQL_PREPARED_RESULTSET_H_
#define MYSQL_PREPARED_RESULTSET_H_

#include "resultset.h"
#include <vector>
#include <mysql/mysql.h>

class MySQLBuffer;
class MySQLPreparedStatement;

class MySQLPreparedResultSet : public ResultSet
{
public:
    MySQLPreparedResultSet(MySQLPreparedStatement* stmt);
    virtual ~MySQLPreparedResultSet();
    
    virtual bool Next();
    
    virtual const Statement* GetStatement() const;
    
    virtual uint32_t GetBlob(uint32_t col_index, char* buffer, uint32_t max_buf_len)  const;
    
    virtual bool GetBoolean(uint32_t col_index, bool default_val) const;
    
    virtual double GetDouble(uint32_t col_index, double default_val) const;
    
    virtual int32_t GetInt(uint32_t col_index, int32_t default_val)  const;
    
    virtual uint32_t GetUInt(uint32_t col_index, uint32_t default_val) const;
    
    virtual int64_t GetInt64(uint32_t col_index, int64_t default_val) const;
    
    virtual uint64_t GetUInt64(uint32_t col_index, uint64_t default_val) const;
    
    virtual uint32_t GetString(uint32_t col_index, char* buffer, uint32_t max_buf_len)  const;
    
    virtual bool IsNull(uint32_t col_index) const;
    
    void set_mysql_metadata_res(MYSQL_RES* res);
    
    MYSQL_BIND* mysql_results();
    
private:
    bool BindResults();
    
private:
    MySQLPreparedStatement* stmt_;
    
    std::vector<MYSQL_BIND> results_;
    MYSQL_RES	* mysql_metadata_res_;
    MYSQL_ROW cur_row_;
    MYSQL_FIELD* mysql_fields;
    uint32_t field_count_;
    uint64_t rows_;
    bool	has_result;
    std::vector<uint64_t> fields_length_;
    
    MySQLBuffer* result_buffer_;
};

#endif
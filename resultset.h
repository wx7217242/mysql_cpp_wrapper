#ifndef RESULTSET_H_
#define RESULTSET_H_

#include "types.h"
#include <istream>
#include <vector>
#include <mysql/mysql.h>

namespace mysql 
{

class Statement;
class MySQLStatement;
class MySQLPreparedStatement;

class ResultSet 
{
public:
    virtual ~ResultSet();
    
    virtual bool Next() = 0;
    
    virtual bool Init() = 0;
    
    virtual const Statement* GetStatement() const = 0;
    
    virtual int GetBlob(int idx, char* buffer, unsigned int max_buf_len)  const = 0;
    
    virtual bool GetBoolean(int idx, bool def_val) const = 0;
    
    virtual double GetDouble(int idx, double def_val) const = 0;
    
    virtual int GetInt(int idx, int def_val)  const = 0;
    
    virtual unsigned int GetUInt(int idx, unsigned int def_val) const = 0;
    
    virtual long GetInt64(int idx, long def_val) const = 0;
    
    virtual unsigned long GetUInt64(int idx, unsigned long def_val) const = 0;
    
    virtual int GetString(int idx, char* buffer, unsigned int max_buf_len)  const = 0;
    
    virtual std::string GetString(int idx)  const = 0;
    
    virtual bool IsNull(int idx) const = 0;
    
    //          virtual void clearWarnings() = 0;
    //          virtual void close() = 0;
    //          virtual unsigned int FindColumn(const std::string& col_label) const = 0;
    //          virtual int getConcurrency() = 0;
    //          virtual SQLString getCursorName() = 0;
    //          virtual int getFetchDirection() = 0;
    //          virtual size_t getFetchSize() = 0;
    //          virtual int getHoldability() = 0;
    //          virtual ResultSetMetaData * getMetaData() const = 0;
    //          virtual size_t getRow() const = 0;
    //          virtual RowID * getRowId(unsigned int columnIndex) = 0;
    //          virtual RowID * getRowId(const sql::SQLString & columnLabel) = 0;
    
    
    
    //          virtual enum_type getType() const = 0;
    
    //          virtual void getWarnings() = 0;
    
    //          virtual void insertRow() = 0;
    
    //          virtual bool isAfterLast() const = 0;
    
    //          virtual bool isBeforeFirst() const = 0;
    
    //          virtual bool isClosed() const = 0;
    
    //          virtual bool isFirst() const = 0;
    
    //          virtual bool isLast() const = 0;
    
    //          virtual bool last() = 0;
    
    //          virtual bool next() = 0;
    
    //          virtual void moveToCurrentRow() = 0;
    
    //          virtual void moveToInsertRow() = 0;
    
    //          virtual bool previous() = 0;
    
    //          virtual void refreshRow() = 0;
    
    //          virtual bool relative(int rows) = 0;
    
    //          virtual bool rowDeleted() = 0;
    
    //          virtual bool rowInserted() = 0;
    
    //          virtual bool rowUpdated() = 0;
    
    //          virtual void setFetchSize(size_t rows) = 0;
    
    //          virtual size_t rowsCount() const = 0;
    
    //          virtual bool wasNull() const = 0;
    
};

class MySQLResultSet : public ResultSet
{
public:
    MySQLResultSet(MySQLStatement* stmt);
    virtual ~MySQLResultSet();
    
    virtual bool Next();
    
    virtual bool Init();
    
    virtual const Statement* GetStatement() const;
    
    virtual int GetBlob(int idx, char* buffer, unsigned int max_buf_len)  const;
    
    virtual bool GetBoolean(int idx, bool def_val) const;
    
    virtual double GetDouble(int idx, double def_val) const;
    
    virtual int GetInt(int idx, int def_val)  const;
    
    virtual unsigned int GetUInt(int idx, unsigned int def_val) const;
    
    virtual long GetInt64(int idx, long def_val) const;
    
    virtual unsigned long GetUInt64(int idx, unsigned long def_val) const;
    
    virtual int GetString(int idx, char* buffer, unsigned int max_buf_len)  const;
    
    virtual std::string GetString(int idx)  const;
    
    virtual bool IsNull(int idx) const;
    
private:
    bool StoreResults();
    bool IsCurRowValid(int idx) const;
    
private:
    MySQLStatement*     stmt_;
    MYSQL_STMT*         mysql_stmt_;
    MYSQL_RES*          mysql_res_;
    MYSQL_ROW           cur_row_;
    MYSQL_FIELD*        mysql_fields;
    unsigned int        field_count_;
    size_t              rows_;
    bool                has_result;
    size_t*             fields_length_;
};

class MySQLPreparedResultSet : public ResultSet
{
public:
    MySQLPreparedResultSet(MySQLPreparedStatement* stmt);
    virtual ~MySQLPreparedResultSet();
    
    virtual bool Next();
    
    virtual bool Init();
    
    virtual const Statement* GetStatement() const;
    
    virtual int GetBlob(int idx, char* buffer, unsigned int max_buf_len)  const;
    
    virtual bool GetBoolean(int idx, bool def_val) const;
    
    virtual double GetDouble(int idx, double def_val) const;
    
    virtual int GetInt(int idx, int def_val)  const;
    
    virtual unsigned int GetUInt(int idx, unsigned int def_val) const;
    
    virtual long GetInt64(int idx, long def_val) const;
    
    virtual unsigned long GetUInt64(int idx, unsigned long def_val) const;
    
    virtual int GetString(int idx, char* buffer, unsigned int max_buf_len)  const;
    
    virtual std::string GetString(int idx)  const;
    
    virtual bool IsNull(int idx) const;
    
    void set_mysql_metadata_res(MYSQL_RES* res);
    
    MYSQL_BIND* mysql_results();
    
private:
    bool BindResults();
    bool IsCurRowValid(int idx) const;
    
private:
    MySQLPreparedStatement*     stmt_;
    std::vector<MYSQL_BIND>     results_;
    MYSQL_RES*                  mysql_metadata_res_;
    MYSQL_ROW                   cur_row_;
    MYSQL_FIELD*                mysql_fields;
    unsigned int                field_count_;
    size_t                      rows_;
    bool                        has_result;
    std::vector<size_t>         fields_length_;
    MySQLBuffer*                result_buffer_;
};

}

#endif
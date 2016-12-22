#ifndef RESULTSET_H_
#define RESULTSET_H_

#include <istream>
#include <stdint.h>

class Statement;

class ResultSet 
{
public:
      virtual ~ResultSet() {}
      
      virtual bool Next() = 0;
      
      virtual const Statement* GetStatement() const = 0;
      
      virtual uint32_t GetBlob(uint32_t col_index, char* buffer, uint32_t max_buf_len)  const = 0;
      
      virtual bool GetBoolean(uint32_t col_index, bool default_val) const = 0;
      
      virtual double GetDouble(uint32_t col_index, double default_val) const = 0;
      
      virtual int32_t GetInt(uint32_t col_index, int default_val) const = 0;
      
      virtual uint32_t GetUInt(uint32_t col_index, uint32_t default_val) const = 0;
      
      virtual int64_t GetInt64(uint32_t col_index, int64_t default_val) const = 0;
      
      virtual uint64_t GetUInt64(uint32_t col_index, uint64_t default_val) const = 0;
      
      virtual uint32_t GetString(uint32_t col_index, char* buffer, uint32_t max_buf_len)  const = 0;
      
      virtual bool IsNull(uint32_t col_index) const = 0;
      
      //          virtual void clearWarnings() = 0;
      //          virtual void close() = 0;
      //          virtual uint32_t FindColumn(const std::string& col_label) const = 0;
//          virtual int getConcurrency() = 0;
//          virtual SQLString getCursorName() = 0;
//          virtual int getFetchDirection() = 0;
//          virtual size_t getFetchSize() = 0;
//          virtual int getHoldability() = 0;
//          virtual ResultSetMetaData * getMetaData() const = 0;
//          virtual size_t getRow() const = 0;
//          virtual RowID * getRowId(uint32_t columnIndex) = 0;
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

#endif
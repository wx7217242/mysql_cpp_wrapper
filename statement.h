#ifndef STATEMENT_H_
#define STATEMENT_H_

#include <string>

class Connection;
class ResultSet;

class Statement 
{
public:
      Statement() {}
      virtual ~Statement() {}
      
      virtual Connection* GetConnection() = 0;
      
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

#endif
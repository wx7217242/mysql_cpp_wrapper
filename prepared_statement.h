#ifndef PREPARED_STATEMENT_H_
#define PREPARED_STATEMENT_H_

#include "statement.h"
#include <stdint.h>

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

#endif
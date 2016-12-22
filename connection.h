#ifndef CONNECTION_H_
#define CONNECTION_H_

#include <string>

class Statement;
class PreparedStatement;

class Connection
{
public:
          Connection(const char* host, const char* user, const char* passwd, const char* database, short port) {}
          virtual ~Connection() {}
          
          virtual bool Connect(const char* charset, unsigned int timeout, bool auto_commit) = 0;
          virtual bool Reconnect() = 0;
          virtual bool IsConnected() = 0;
          
          virtual Statement*CreateStatement() = 0;
          virtual PreparedStatement* PrepareStatement(const std::string& sql) = 0;
          
          virtual void SetAutoCommit(bool auto_commit) = 0;
          virtual void Commit() = 0;
          virtual bool GetAutoCommit() = 0;
          
          virtual void Close()  = 0;
//          virtual bool IsClosed() = 0;         
          
          virtual int GetErrNo() = 0;
          virtual const char* GetError() = 0;

//          virtual bool IsReadOnly() = 0;
//          virtual bool IsValid() = 0;
//          virtual void ClearWarnings() = 0;

//          virtual Savepoint * SetSavepoint() = 0;
//          virtual Savepoint * SetSavepoint(const sql::SQLString& name) = 0;         
//          virtual void ReleaseSavepoint(Savepoint * savepoint) = 0;
//          virtual void Rollback(Savepoint * savepoint) = 0;
//          virtual void Rollback() = 0;
private:
          /* Prevent use of these */
          Connection(const Connection &);
          void operator=(Connection &);
};

#endif
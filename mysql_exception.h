#ifndef MYSQL_EXCEPTION_H_
#define MYSQL_EXCEPTION_H_

#include "exception.h"

class MySQLException : public Exception
{
public:
    MySQLException(const std::string& reason);
};

#endif
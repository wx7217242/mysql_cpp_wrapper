#include "mysql_exception.h"

MySQLException::MySQLException(const std::string& reason) : Exception(reason)
{
}

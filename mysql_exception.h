#ifndef MYSQL_EXCEPTION_H_
#define MYSQL_EXCEPTION_H_

#include <string>

class MySQLException : public std::exception
{
public:
    MySQLException(const std::string& reason) : reason_(reason) {}
    
    virtual ~MySQLException() throw() {}
    
    virtual const char* what() const throw()
    {
        return reason_.c_str();
    }
private:
    std::string reason_;
};

#endif
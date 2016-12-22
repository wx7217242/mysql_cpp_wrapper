#ifndef EXCEPTION_H_
#define EXCEPTION_H_

#include <stdexcept>

class Exception: public std::exception
{
public:
    Exception(const std::string& reason) : reason_(reason)
    {    
    }
    
    virtual ~Exception() throw()
    {   
    }
    
    virtual const char* what() const throw()
    {
        return reason_.c_str();
    }
    
private:
    std::string reason_;
};

#endif
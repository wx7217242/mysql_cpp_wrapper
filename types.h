#ifndef MYSQL_CPP_CONN_TYPES_H_
#define MYSQL_CPP_CONN_TYPES_H_

#ifdef WIN32
typedef char			int8_t;
typedef short			int16_t;
typedef int				int32_t;
typedef	long long		int64_t;
typedef unsigned char	uint8_t;
typedef unsigned short	uint16_t;
typedef unsigned int	uint32_t;
typedef	unsigned long long	uint64_t;
typedef int				socklen_t;
#else
#include <stdint.h>
#endif

#include <vector>
#include <string.h>

namespace mysql
{

const int kDefaultParamBufferSize = 1024; 
const int kDefaultResultBufferSize = 10 * 1024; 
const int kMaxBufferSize = 10 * 1024 * 1024;

class MySQLBuffer
{
public:
    MySQLBuffer(uint64_t size) : 
        buffer_size_(size),
        buffer_cur_pos_(0)
    {
        buffer_.resize(buffer_size_);
    }
    
    ~MySQLBuffer()
    {
        
    }
    
    bool SetParamToBuffer(const void* param, uint64_t size)
    {
        if (buffer_cur_pos_ + size > buffer_size_)
            return false;
        
        memcpy(buffer_from_current(), param, size);
        return true;
    }
    
    void ResetBuffer()
    {
        buffer_cur_pos_ = 0;
        buffer_.clear();
    }
    
    char* buffer() { return buffer_.data(); }
    
    char* buffer_from_current() { return buffer_.data() + buffer_cur_pos_; }
    
    uint64_t buffer_size() const { return buffer_size_; }
    
    uint64_t buffer_cur_pos() const { return buffer_cur_pos_; }
    
    bool IsCapacityAvailable(uint64_t size) const
    {
        return buffer_cur_pos_ + size <= buffer_size_;
    }
    
    bool IncreaseBufferCurPos(uint64_t offset)
    {
        if (IsCapacityAvailable(offset))
        {
            buffer_cur_pos_ += offset;
            return true;
        }
        
        return false;
    }
    
    bool ExtendBuffer()
    {
        if (buffer_size_ + (buffer_size_ << 1) >= kMaxBufferSize)
        {
            return false;
        }
        
        buffer_size_ += (buffer_size_ << 1);
        buffer_.resize(buffer_size_);
        return true;
    }
    
private:
    std::vector<char> buffer_;
    uint64_t buffer_size_;
    uint64_t buffer_cur_pos_;
};

}

#endif
#include "mysql_buffer.h"
#include <string.h>

MySQLBuffer::MySQLBuffer(uint64_t size) : 
    buffer_size_(size),
    buffer_cur_pos_(0)
{
    buffer_.resize(buffer_size_);
}

MySQLBuffer::~MySQLBuffer()
{
    
}

bool MySQLBuffer::SetParamToBuffer(const void* param, uint64_t size)
{
    if (buffer_cur_pos_ + size > buffer_size_)
        return false;
    
    memcpy(buffer_from_current(), param, size);
    return true;
}

void MySQLBuffer::ResetBuffer()
{
    buffer_cur_pos_ = 0;
    buffer_.clear();
}

char *MySQLBuffer::buffer() { return buffer_.data(); }

char *MySQLBuffer::buffer_from_current() { return buffer_.data() + buffer_cur_pos_; }

uint64_t MySQLBuffer::buffer_size() const { return buffer_size_; }

uint64_t MySQLBuffer::buffer_cur_pos() const { return buffer_cur_pos_; }

bool MySQLBuffer::IsCapacityAvailable(uint64_t size) const
{
    return buffer_cur_pos_ + size <= buffer_size_;
}

bool MySQLBuffer::IncreaseBufferCurPos(uint64_t offset)
{
    if (IsCapacityAvailable(offset))
    {
        buffer_cur_pos_ += offset;
        return true;
    }
    return false;
}




#ifndef MYSQL_BUFFER_H_
#define MYSQL_BUFFER_H_

#include <stdint.h>
#include <vector>

class MySQLBuffer
{
public:
    MySQLBuffer(uint64_t size);
    
    ~MySQLBuffer();
    
    bool SetParamToBuffer(const void* param, uint64_t size);
    
    void ResetBuffer();
    
    char* buffer();
    
    char* buffer_from_current();
    
    uint64_t buffer_size() const;
    
    uint64_t buffer_cur_pos() const;
    
    bool IsCapacityAvailable(uint64_t size) const;
    
    bool IncreaseBufferCurPos(uint64_t offset);
    
private:
    std::vector<char> buffer_;
    const uint64_t buffer_size_;
    uint64_t buffer_cur_pos_;
};






#endif
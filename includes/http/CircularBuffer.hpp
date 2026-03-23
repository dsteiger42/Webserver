#ifndef CIRCULARBUFFER_HPP
#define CIRCULARBUFFER_HPP

#include <iostream>
#include <cstring>
#include <vector>
#include <stdexcept>



class CircularBuffer
{
    private:
        std::vector<char> _buffer;
        size_t _head; //write pos 
        size_t _tail; //read pos
        size_t _size; // numbber of bytes in the buffer
        size_t _capacity; // max size of the buffer
    public:
        CircularBuffer(size_t cap);
        bool is_Empty();
        bool is_Full();
        size_t get_Size();
        size_t write(const char *data, size_t len);
        size_t read(char *out, size_t len);
        size_t peek(char *temp, size_t len) const;
        size_t find(const std::string &pattern) const;
        void consume(size_t bytes);
};


#endif
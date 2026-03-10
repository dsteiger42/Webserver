#ifndef CIRCULARBUFFER_HPP
#define CIRCULARBUFFER_HPP

#include <iostream>
#include <cstring>
#include <vector>
#include <stdexcept>



class CircularBuffer
{
    private:
        std::vector<char>buffer;
        size_t head; //write pos 
        size_t tail; //read pos
        size_t size; // numbber of bytes in the buffer
        size_t capacity; // max size of the buffer
    public:
        CircularBuffer(size_t cap);
        bool isEmpty();
        bool isFull();
        size_t getSize();
        size_t write(const char *data, size_t len);
        size_t read(char *out, size_t len);
        size_t peek(std::vector<char> &temp, size_t len) const;
        size_t find(const std::string &pattern) const;
};


#endif
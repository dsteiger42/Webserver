#include <http/CircularBuffer.hpp>

CircularBuffer::CircularBuffer(size_t cap) : buffer(cap), head(0), tail(0), size(0), capacity(cap){
}

bool CircularBuffer::isEmpty()
{
    return (this->size == 0);
}

bool CircularBuffer::isFull()
{
    return (this->size == capacity);
}

size_t CircularBuffer::getSize()
{
    return (this->size);
}
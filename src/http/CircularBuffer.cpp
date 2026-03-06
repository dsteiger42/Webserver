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

void CircularBuffer::write(const char *data, size_t len)
{
    if (len > capacity - size)
        throw std::overflow_error("CircularBuffer overflow");
    for (size_t i = 0; i < len; i++)
    {
        buffer[head] = data[i];
        head = (head + 1) % capacity;
    }
    size += len; 
}

size_t CircularBuffer::read(char *out, size_t len)
{
    if (isEmpty())
        return 0;
    size_t toRead = std::min(len, size);
    for (size_t i = 0; i < len; i++)
    {
        out[i] = buffer[tail];
        tail = (tail + 1) % capacity;
    }
    size -= toRead;
    return toRead;
}
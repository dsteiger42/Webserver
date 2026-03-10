#include <http/CircularBuffer.hpp>

CircularBuffer::CircularBuffer(size_t cap) : buffer(cap), head(0), tail(0),
	size(0), capacity(cap)
{
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

// size_t CircularBuffer::write(const char *data, size_t len)
// {
// 	size_t freeSpace = capacity - size;
// 	size_t toWrite = std::min(len, freeSpace);
// 	for (size_t i = 0; i < toWrite; i++)
// 	{
// 		buffer[head] = data[i];
// 		head = (head + 1) % capacity;
// 	}
// 	size += toWrite;
// 	return toWrite;
// }

size_t CircularBuffer::write(const char *data, size_t len)
{
    if (size == capacity)
        return 0;
    size_t freeSpace = capacity - size;
    size_t toWrite = std::min(len, freeSpace);
    size_t firstChunk = std::min(toWrite, capacity - head);
    std::memcpy(&buffer[head], data, firstChunk); // trocar
    size_t secondChunk = toWrite - firstChunk;
    if (secondChunk)
        std::memcpy(&buffer[0], data + firstChunk, secondChunk);
    head = (head + toWrite) % capacity;
    size += toWrite;
    return toWrite;
}

size_t CircularBuffer::read(char *out, size_t len)
{
	size_t	toRead;

	if (isEmpty())
		return (0);
	toRead = std::min(len, size);
	for (size_t i = 0; i < toRead; i++)
	{
		out[i] = buffer[tail];
		tail = (tail + 1) % capacity;
	}
	size -= toRead;
	return (toRead);
}

size_t CircularBuffer::peek(char *temp, size_t len) const
{
	size_t bytesToCopy = std::min(len, this->size);
	size_t firstChunk = std::min(bytesToCopy, capacity - tail);
	for (size_t i = 0; i < firstChunk; i++)
	{
		temp[i] = buffer[(tail + i) % capacity];
	}
	if (bytesToCopy > firstChunk)
	{
		size_t secondChunk = bytesToCopy - firstChunk;
		for (size_t i = 0; i < secondChunk; i++)
		{
			temp[firstChunk + i] = buffer[i];
		}
	}
	return (bytesToCopy);
}

size_t CircularBuffer::find(const std::string &pattern) const
{
	if (pattern.empty() || size < pattern.size())
		return (std::string::npos);
	size_t limit = size - pattern.size();
	for (size_t i = 0; i <= limit; ++i)
	{
		bool match = true;
		for (size_t j = 0; j < pattern.size(); ++j)
		{
			size_t index = (tail + i + j) % capacity;
			if (buffer[index] != pattern[j])
			{
				match = false;
				break ;
			}
		}
		if (match)
			return (i);
	}
	return (std::string::npos);
}


void CircularBuffer::consume(size_t bytes)
{
	bytes = std::min(bytes, size);
	tail = (tail + bytes) % capacity;
	size -= bytes;
}
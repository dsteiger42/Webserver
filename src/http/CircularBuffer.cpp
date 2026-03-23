#include <http/CircularBuffer.hpp>

CircularBuffer::CircularBuffer(size_t cap) : _buffer(cap), _head(0), _tail(0),
	_size(0), _capacity(cap)
{
}

bool CircularBuffer::is_Empty()
{
	return (this->_size == 0);
}

bool CircularBuffer::is_Full()
{
	return (this->_size == _capacity);
}

size_t CircularBuffer::get_Size()
{
	return (this->_size);
}

size_t CircularBuffer::write(const char *data, size_t len)
{
    if (_size == _capacity)
        return 0;
    size_t freeSpace = (_capacity - _size);
    size_t toWrite = std::min(len, freeSpace);
    size_t firstChunk = std::min(toWrite, _capacity - _head);
    std::memcpy(&_buffer[_head], data, firstChunk); // trocar
    size_t secondChunk = toWrite - firstChunk;
    if (secondChunk)
        std::memcpy(&_buffer[0], data + firstChunk, secondChunk); //trocar
    _head = (_head + toWrite) % _capacity;
    _size += toWrite;
    return toWrite;
}

size_t CircularBuffer::read(char *out, size_t len)
{
	size_t	toRead;

	if (is_Empty())
		return (0);
	toRead = std::min(len, _size);
	for (size_t i = 0; i < toRead; i++)
	{
		out[i] = _buffer[_tail];
		_tail = (_tail + 1) % _capacity;
	}
	_size -= toRead;
	return (toRead);
}

size_t CircularBuffer::peek(char *temp, size_t len) const
{
	size_t bytesToCopy = std::min(len, this->_size);
	size_t firstChunk = std::min(bytesToCopy, _capacity - _tail);
	for (size_t i = 0; i < firstChunk; i++)
	{
		temp[i] = _buffer[(_tail + i) % _capacity];
	}
	if (bytesToCopy > firstChunk)
	{
		size_t secondChunk = bytesToCopy - firstChunk;
		for (size_t i = 0; i < secondChunk; i++)
		{
			temp[firstChunk + i] = _buffer[i];
		}
	}
	return (bytesToCopy);
}

size_t CircularBuffer::find(const std::string &pattern) const
{
	if (pattern.empty() || _size < pattern.size())
		return (std::string::npos);
	size_t limit = _size - pattern.size();
	for (size_t i = 0; i <= limit; ++i)
	{
		bool match = true;
		for (size_t j = 0; j < pattern.size(); ++j)
		{
			size_t index = (_tail + i + j) % _capacity;
			if (_buffer[index] != pattern[j])
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
	bytes = std::min(bytes, _size);
	_tail = (_tail + bytes) % _capacity;
	_size -= bytes;
}
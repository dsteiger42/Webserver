#include <http/Request.hpp>

Request::Request() : _buffer(4096), _state(READING_HEADER)
{
}

const std::string& Request::get_Method() const
{
	return (this->_method);
}

const std::string& Request::get_Path() const
{
	return (this->_path);
}

const std::string& Request::get_Version() const
{
	return (this->_version);
}

const std::string& Request::get_Body() const
{
	return (this->_body);
}

const std::string Request::get_Query() const
{
    return (this->_query);
}

const std::string& Request::get_Header(const std::string &key) const
{
	static const std::string empty = "";
	std::map<std::string, std::string>::const_iterator it = _headers.find(key);
    if (it != _headers.end())
        return it->second;
    return empty;
}

void Request::reset()
{
	_method.clear();
	_path.clear();
	_version.clear();
	_body.clear();
	_headers.clear();
	_contentLength = 0;
	_state = READING_HEADER;
}

bool Request::is_Done() const
{
	return (this->_state == DONE);
}

void Request::fill_Buffer(const std::string &request, size_t len)
{
    size_t written = 0;

    while (written < len)
    {
        size_t bytesWritten = _buffer.write(request.data() + written, len - written);

        if (bytesWritten == 0)
        {
            advanceParsing();
            if (_buffer.is_Full())
            {
                break;
            }
            continue; 
        }
        written += bytesWritten;
        advanceParsing();
    }
}

void Request::validate_Request()
{
	if (_version == "HTTP/1.1" && _headers.find("Host") == _headers.end())
		throw std::runtime_error("400 Bad Request: Missing Host header");
}

std::string Request::extract_HeaderFromBuffer(size_t size)
{
    std::string header(size, '\0');        // aloca string do tamanho exato
    _buffer.peek(reinterpret_cast<char*>(&header[0]), size); // copia direto para string
    _buffer.consume(size);                  // remove do buffer
    return header;
}
void Request::determine_NextState()
{
	std::map<std::string,
		std::string>::iterator it = _headers.find("Content-Length");
	if (it != _headers.end())
	{
		_contentLength = std::strtoul(it->second.c_str(), NULL, 10);
		if (_contentLength > 0)
		{
			_state = READING_BODY;
			return ;
		}
	}
	_state = DONE;
}

bool Request::process_Header() // parsing headers
{
	size_t pos = _buffer.find("\r\n\r\n");
	if (pos == std::string::npos)
		return (false);
	size_t headersize = pos + 4;
	if (_buffer.get_Size() < headersize)
		return (false);
	std::string header = extract_HeaderFromBuffer(headersize);
	parse_Header(header);
	validate_Request();
	determine_NextState();
	return (true);
}

bool Request::process_Body() // read the Body
{
	size_t remaining = _contentLength - _body.size();
	if (remaining == 0)
	{
        _state = DONE;
        return true;
    }
	size_t available = _buffer.get_Size();
	size_t toRead = std::min(remaining, available);
	if (toRead == 0)
        return false;

	size_t oldSize = _body.size();
    _body.resize(oldSize + toRead);
    _buffer.read(&_body[oldSize], toRead);
    if (_body.size() >= _contentLength)
        _state = DONE;
    return true;
}

void Request::parse_RequestLine(std::string &line, std::istringstream &split)
{
	if (std::getline(split, line))
	{
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);
		std::istringstream rl(line);
		if (!(rl >> _method >> _path >> _version))
			throw std::runtime_error("Invalid request line");
		split_PathQuery(_path);
	}
}

void Request::parse_Headers(std::string &line, std::istringstream &split)
{
	size_t	pos;

	while (std::getline(split, line))
	{
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);
		if (line.empty())
			break ;
		pos = line.find(':');
		std::string key = line.substr(0, pos);
		std::string value = line.substr(pos + 1);
		while (!value.empty() && (value[0] == ' ' || value[0] == '\t'))
			value.erase(value.begin());
		_headers[key] = value;
	}
}

void Request::parse_Header(const std::string &headerStr)
{
	std::istringstream split(headerStr);
	std::string line;
	parse_RequestLine(line, split); // POST /index.html HTTP/1.1
	parse_Headers(line, split);     // Host: localhost || Content-Length: 5
}

void Request::advanceParsing()
{
	bool	progressed;

	progressed = true;
	while (progressed)
	{
		progressed = false;
		if (_state == READING_HEADER)
			progressed = process_Header();
		else if (_state == READING_BODY)
			progressed = process_Body();
		else if (_state == DONE)
			return ;
	}
}

void Request::parse(const std::string &request)
{
	fill_Buffer(request, request.length());
	advanceParsing();
}

void Request::split_PathQuery(const std::string& path)
{
    size_t pos = path.find("?");
    if (pos != std::string::npos && pos + 1 <= path.size())
    {
		std::string path1;
        path1 = path.substr(0, pos);
        _query = path.substr(pos + 1);
		_path = path1;
    }
    else
	{
        _path = path;    
		_query = "";
	}
}
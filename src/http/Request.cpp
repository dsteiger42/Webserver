#include <http/Request.hpp>

Request::Request() : Buffer(4096), state(READING_HEADER)
{
}

const std::string Request::getMethod() const
{
	return (this->Method);
}

const std::string Request::getPath() const
{
	return (this->Path);
}

const std::string Request::getVersion() const
{
	return (this->Version);
}

const std::string Request::getBody() const
{
	return (this->Body);
}

const std::string Request::getHeader(const std::string &key) const
{
	std::map<std::string, std::string>::const_iterator it = Headers.find(key);
    if (it != Headers.end())
        return it->second;
    return "";
}

void Request::reset()
{
	Method.clear();
	Path.clear();
	Version.clear();
	Body.clear();
	Headers.clear();
	contentLength = 0;
	state = READING_HEADER;
}

bool Request::isDone() const
{
	return (this->state == DONE);
}

void Request::fillBuffer(const std::string &request, size_t len)
{
    size_t written = 0;

    while (written < len)
    {
        size_t bytesWritten = Buffer.write(request.data() + written, len - written);

        if (bytesWritten == 0)
        {
            advanceParsing();
            if (Buffer.isFull())
            {
                break;
            }
            continue; 
        }
        written += bytesWritten;
        advanceParsing();
    }
}

void Request::validateRequest()
{
	if (Version == "HTTP/1.1" && Headers.find("Host") == Headers.end())
		throw std::runtime_error("400 Bad Request: Missing Host header");
}

std::string Request::extractHeaderFromBuffer(size_t size)
{
    std::string header(size, '\0');        // aloca string do tamanho exato
    Buffer.peek(reinterpret_cast<char*>(&header[0]), size); // copia direto para string
    Buffer.consume(size);                  // remove do buffer
    return header;
}
void Request::determineNextState()
{
	std::map<std::string,
		std::string>::iterator it = Headers.find("Content-Length");
	if (it != Headers.end())
	{
		contentLength = std::strtoul(it->second.c_str(), NULL, 10);
		if (contentLength > 0)
		{
			state = READING_BODY;
			return ;
		}
	}
	state = DONE;
}

bool Request::processHeader() // parsing headers
{
	size_t pos = Buffer.find("\r\n\r\n");
	if (pos == std::string::npos)
		return (false);
	size_t headersize = pos + 4;
	if (Buffer.getSize() < headersize)
		return (false);
	std::string header = extractHeaderFromBuffer(headersize);
	parseHeader(header);
	validateRequest();
	determineNextState();
	return (true);
}

bool Request::processBody() // read the Body
{
	size_t remaining = contentLength - Body.size();
	if (remaining == 0)
	{
        state = DONE;
        return true;
    }
	size_t available = Buffer.getSize();
	size_t toRead = std::min(remaining, available);
	if (toRead == 0)
        return false;

	size_t oldSize = Body.size();
    Body.resize(oldSize + toRead);
    Buffer.read(&Body[oldSize], toRead);
    if (Body.size() >= contentLength)
        state = DONE;
    return true;
}

void Request::parseRequestLine(std::string &line, std::istringstream &split)
{
	if (std::getline(split, line))
	{
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);
		std::istringstream rl(line);
		if (!(rl >> Method >> Path >> Version))
			throw std::runtime_error("Invalid request line");
	}
}

void Request::parseHeaders(std::string &line, std::istringstream &split)
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
		Headers[key] = value;
	}
}

void Request::parseHeader(const std::string &headerStr)
{
	std::istringstream split(headerStr);
	std::string line;
	parseRequestLine(line, split); // POST /index.html HTTP/1.1
	parseHeaders(line, split);     // Host: localhost || Content-Length: 5
}

void Request::advanceParsing()
{
	bool	progressed;

	progressed = true;
	while (progressed)
	{
		progressed = false;
		if (state == READING_HEADER)
			progressed = processHeader();
		else if (state == READING_BODY)
			progressed = processBody();
		else if (state == DONE)
			return ;
	}
}

void Request::parse(const std::string &request)
{
	fillBuffer(request, request.length());
	advanceParsing();
}

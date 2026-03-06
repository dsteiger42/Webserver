#include <http/Request.hpp>

static CircularBuffer	Buffer(4096);

Request::Request() : state(READING_HEADER)
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

const std::string Request::getBoddy() const
{
	return (this->Boddy);
}

const std::string Request::getHeader(const std::string &key) const
{
	(void)key;
	return ("ok");
}

void Request::reset()
{
	Method.clear();
	Path.clear();
	Version.clear();
	Boddy.clear();
	Headers.clear();
	contentLength = 0;
	state = READING_HEADER;
}

bool Request::isDone() const
{
	return (this->state == DONE);
}

/* static size_t	splitHeaderFromBoddy(const std::string &request)
{
	size_t	HeaderPos;

	HeaderPos = request.find("\r\n\r\n");
	return (HeaderPos);
} */
static void	fillBuffer(const std::string &request, size_t pos)
{
	Buffer.write(request.c_str(), pos);
}

void Request::parseHeader(const std::string &headerStr)
{
	size_t	pos;

	std::istringstream split(headerStr);
	std::string line;
	if (std::getline(split, line))
	{
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);
		std::istringstream rl(line);
		if (!(rl >> Method >> Path >> Version))
			throw std::runtime_error("Invalid request line");
	}
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

void Request::advanceParsing()
{
    bool progressed = true;

    while (progressed)
    {
        progressed = false;
        if (this->state == READING_HEADER)
        {
            size_t pos = Buffer.find("\r\n\r\n");
            if (pos == std::string::npos)
                return;
            size_t headersize = pos + 4;
            if (Buffer.getSize() < headersize)
                return;
            std::vector<char> temp(headersize);
            Buffer.read(temp.data(), headersize);
            std::string header(temp.begin(), temp.end());
            parseHeader(header);
            if (Version == "HTTP/1.1" && Headers.find("Host") == Headers.end())
                throw std::runtime_error("400 Bad Request: Missing Host header");
            std::map<std::string, std::string>::iterator it = Headers.find("Content-Length");
            if (it != Headers.end())
            {
                this->contentLength = std::strtoul(it->second.c_str(), NULL, 10);
                if (this->contentLength > 0)
                {
                    this->state = READING_BODY;
                    progressed = true;
                    continue;
                }
            }
            this->state = DONE;
            progressed = true;
            continue;
        }
        if (this->state == READING_BODY)
        {
            if (Buffer.getSize() < this->contentLength)
                return;

            std::vector<char> bodyTemp(this->contentLength);
            Buffer.read(bodyTemp.data(), this->contentLength);

            this->Boddy.assign(bodyTemp.begin(), bodyTemp.end());
            this->state = DONE;
            progressed = true;
            continue;
        }
        if (this->state == DONE)
            return;
    }
}


void Request::parse(const std::string &request)
{
	/* size_t	HeaderFinalPos;
	HeaderFinalPos = splitHeaderFromBoddy(request); */
	/* parseHeader(Buffer, *this); */
	fillBuffer(request, request.length());
	advanceParsing();
}

int	main(void)
{
	Request	req;

	std::string request = "POST /index.html HTTP/1.1\r\n"
							"Host: localhost\r\n"
							"Content-Length: 5\r\n"
							"\r\n"
							"Hello";
	try
	{
		req.parse(request);
		if (req.isDone())
		{
			std::cout << "Request parsed successfully\n";
			std::cout << "Method: " << req.getMethod() << std::endl;
			std::cout << "Path: " << req.getPath() << std::endl;
			std::cout << "Version: " << req.getVersion() << std::endl;
			std::cout << "Body: " << req.getBoddy() << std::endl;
		}
		else
		{
			std::cout << "Request not fully parsed\n";
		}
	}
	catch (const std::exception &e)
	{
		std::cerr << "Error parsing request: " << e.what() << std::endl;
	}
	return (0);
}

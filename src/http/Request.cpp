#include <http/Request.hpp>

static CircularBuffer Buffer(4096);

Request::Request()
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
	return "ok";
}

static size_t	splitHeaderFromBoddy(const std::string &request)
{
	size_t	HeaderPos;

	HeaderPos = request.find("\r\n\r\n");
    return HeaderPos;
}

static void fillBuffer(const std::string &request, size_t pos)
{
    Buffer.write(request.c_str(), pos);
}


void Request::parse(const std::string &request)
{
    size_t	HeaderFinalPos;
    
    HeaderFinalPos = splitHeaderFromBoddy(request);
    fillBuffer(request, HeaderFinalPos);
    /* parseHeader(Buffer, ) */
    
    std::vector<char> tmp(HeaderFinalPos);
    Buffer.read(tmp.data(), HeaderFinalPos);
    for (size_t i = 0; i < tmp.size(); i++)
        std::cout << tmp[i];

}


int	main(void)
{
	std::string request = "POST /index.html HTTP/1.1\r\n"
							"Host: localhost\r\n"
							"Content-Length: 5\r\n"
							"\r\n"
							"Hello";
	Request obj;
     obj.parse(request);
}
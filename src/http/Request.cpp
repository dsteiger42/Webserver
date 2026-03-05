#include <http/Request.hpp>

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
    /* size_t BoddyPos; */

	HeaderPos = request.find("\r\n\r\n");
    return HeaderPos;
}

static void CopyToBuffer(std::vector<char>&Buffer, const std::string& str, size_t length)
{
    for (size_t i = 0; i < length; i++)
        Buffer.push_back(str[i]);
}

void Request::parse(const std::string &request)
{
    static std::vector<char> Header;
    static std::vector<char> Boddy;
    size_t	HeaderFinalPos;

    HeaderFinalPos = splitHeaderFromBoddy(request);
    CopyToBuffer(Header, request, HeaderFinalPos);
    
    for (size_t i = 0; i < Header.size(); i++)
        std::cout << Header[i];

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
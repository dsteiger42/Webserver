#include <http/Response.hpp>


/* static std::map<int, std::string> createStatusMessages()
{
    std::map<int, std::string> message;

    message[200] = "OK";
    message[201] = "Created";
    message[204] = "No Content";
    message[301] = "Moved Permanently";
    message[302] = "Found";
    message[400] = "Bad Request";
    message[403] = "Forbidden";
    message[404] = "Not Found";
    message[405] = "Method Not Allowed";
    message[500] = "Internal Server Error";
    message[502] = "Bad Gateway";
    message[503] = "Service Unavailable";

    return message;
}

static const std::map<int, std::string> STATUS_MESSAGES = createStatusMessages(); */

Response::Response() :  _errorPages(), _statusCode(200),  _httpVersion("HTTP/1.1"),  _statusMessage("OK") {}

Response::Response(ErrorPages &ErrorPages) :  _errorPages(ErrorPages), _statusCode(200),  _httpVersion("HTTP/1.1"),  _statusMessage("OK") {}

int Response::get_StatusCode() const
{
    return (this->_statusCode);
}

std::string Response::get_StatusMessage() const
{
    return (this->_statusMessage);
}

const std::string Response::get_Body() const
{
    return (this->_body);
}

const std::string& Response::get_Header(std::string &key)
{
    static const std::string empty = "";
    if (_headers.find(key) != _headers.end())
        return _headers[key];
    return empty;
}

void Response::set_StatusCode(int code)
{
    this->_statusCode = code;
    std::map<int, std::string>::const_iterator it = _errorPages.error_pages.find(code);
    if (it != _errorPages.error_pages.end())
        this->_statusMessage = it->second;
    else
        this->_statusMessage = "Unknown";
}

void Response::set_HttpVersion(const std::string& version)
{
    this->_httpVersion = version;
}

void Response::set_StatusMessage(std::string message)
{
    this->_statusMessage = message;
}
void Response::set_Body(std::string body)
{
    this->_body = body;
}
void Response::set_Header(std::string key, std::string value)
{
    this->_headers[key] = value;
}

bool Response::has_Header(std::string key) const
{
    if (_headers.find(key) != _headers.end())
        return true;
    return false;
}

void Response::prepare_Headers()
{
    if (!has_Header("Content-Length"))
    {
        size_t size = this->_body.length();
        std::stringstream ss;
        ss << size;
        std::string length = ss.str();
        set_Header("Content-Length", length);
    }
    if (!has_Header("Connection"))
        set_Header("Connection", "close");
}

std::string Response::build_StatusLine()
{
    std::stringstream ss;

    ss << this->_httpVersion << " " << this->_statusCode << " " << this->_statusMessage << "\r\n";
    return ss.str();
}

void Response::build_Header(std::string &result)
{
    for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); it++)
    {
        result += it->first + ": " + it->second + "\r\n";
    }
    result += "\r\n";
}

void Response::build_Body(std::string &result)
{
    result += this->_body;
}

std::string Response::serialize()
{
    prepare_Headers();
    std::string response;
    response = build_StatusLine();
    build_Header(response);
    build_Body(response);
    return (response);
}

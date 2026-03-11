#include <http/Response.hpp>


static std::map<int, std::string> createStatusMessages()
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

static const std::map<int, std::string> STATUS_MESSAGES = createStatusMessages();

Response::Response() :  StatusCode(200),  httpVersion("HTTP/1.1"),  StatusMessage("OK") {}

void Response::setHttpVersion(const std::string& version)
{
    this->httpVersion = version;
}


int Response::getStatusCode() const
{
    return this->StatusCode;
}

const std::string Response::getBody() const
{
    return this->Body;
}

const std::string& Response::getHeader(std::string &key)
{
    if (Headers.find(key) != Headers.end())
        return Headers[key];
    return "";
}

void Response::setStatusCode(int code)
{
    this->StatusCode = code;
    std::map<int, std::string>::const_iterator it = STATUS_MESSAGES.find(code);
    if (it != STATUS_MESSAGES.end())
        this->StatusMessage = it->second;
    else
        this->StatusMessage = "Unknown";
}
void Response::setStatusMessage(std::string message)
{
    this->StatusMessage = message;
}
void Response::setBody(std::string body)
{
    this->Body = body;
}
void Response::setHeader(std::string key, std::string value)
{
    this->Headers[key] = value;
}

bool Response::hasHeader(std::string key) const
{
    if (Headers.find(key) != Headers.end())
        return true;
    return false;
}

void Response::prepareHeaders()
{
    if (!hasHeader("Content-Length"))
    {
        size_t size = this->Body.length();
        std::stringstream ss;
        ss << size;
        std::string length = ss.str();
        setHeader("Content-Length", length);
    }
    if (!hasHeader("Content-Type"))
        setHeader("Content-Type", "text/html");
    if (!hasHeader("Connection"))
        setHeader("Connection", "close");
}

std::string Response::buildStatusLine()
{
    std::stringstream ss;

    ss << this->httpVersion << " " << this->StatusCode << " " << this->StatusMessage << "\r\n";
    return ss.str();
}

void Response::buildHeader(std::string &result)
{
    for (std::map<std::string, std::string>::iterator it = Headers.begin(); it != Headers.end(); it++)
    {
        result += it->first + ": " + it->second + "\r\n";
    }
    result += "\r\n";
}

void Response::buildBody(std::string &result)
{
    result += this->Body;
}

std::string Response::serialize()
{
    prepareHeaders();
    std::string response;
    response = buildStatusLine();
    buildHeader(response);
    buildBody(response);
    return (response);
}

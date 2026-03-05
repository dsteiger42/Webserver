#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <cstring>
#include <iostream>
#include <map>
#include <vector>

class Request
{
    private:
        std::string Method;  // GET, POST, DELETE
        std::string Path;   // /index.html
        std::string Version; // HTTP/1.1
        std::string Boddy;
        std::map<std::string, std::string> Headers;
    public:
        Request();
        const std::string getMethod() const;
        const std::string getPath() const;
        const std::string getVersion() const;
        const std::string getBoddy() const;
        const std::string getHeader(const std::string& key) const;

        void parse(const std::string& request);

};

#endif


/* GET /index.html HTTP/1.1\r\n
Host: localhost\r\n
Content-Length: 5\r\n
\r\n
Hello */
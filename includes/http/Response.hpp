#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include <cstring>
#include <map>
#include <string>
#include <sstream>

class Response
{
    private:
        int StatusCode;
        std::string httpVersion;
        std::string StatusMessage;
        std::map<std::string, std::string> Headers;
        std::string Body;
    public:
        Response();
        int getStatusCode() const;
        const std::string getBody() const;
        const std::string& getHeader(std::string &key);
        std::string getStatusMessage() const;

        void setHttpVersion(const std::string& version);
        void setStatusCode(int code);
        void setStatusMessage(std::string message);
        void setBody(std::string body);
        void setHeader(std::string key, std::string value);
        
        bool hasHeader(std::string key) const;
        
        void prepareHeaders();
        std::string buildStatusLine();
        void buildHeader(std::string &result);
        void buildBody(std::string &result);
        std::string serialize();


};



#endif
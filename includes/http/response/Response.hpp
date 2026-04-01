#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include <cstring>
#include <map>
#include <string>
#include <sstream>
#include <config/model/types.hpp>

class Response
{
    private:
        ErrorPages _errorPages;
        int _statusCode;
        std::string _httpVersion;
        std::string _statusMessage;
        std::string _filePath;
        std::map<std::string, std::string> _headers;
        std::string _body;
    public:
        Response();
        Response(ErrorPages &ErrorPages);
        
        int get_StatusCode() const;
        const std::string get_Body() const;
        const std::string& get_Header(std::string &key);
        std::string get_StatusMessage(int code) const;
        std::string get_FilePath() const;

        void set_HttpVersion(const std::string& version);
        void set_StatusCode(int code);
        void set_StatusMessage(std::string message);
        void set_Body(std::string body);
        void set_Header(std::string key, std::string value);
        
        bool has_Header(std::string key) const;
        
        void prepare_Headers();
        
        std::string build_StatusLine();
        void build_Header(std::string &result);
        void build_Body(std::string &result);
        std::string serialize();

};



#endif
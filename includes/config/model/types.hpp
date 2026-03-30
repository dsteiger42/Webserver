#ifndef TYPES_HPP
#define TYPES_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <cctype>
#include <map>
#include <vector>
#include <cstdlib>
#include <climits>

struct Config
{
    std::string server_name;
    std::string root;
    std::string index;
    size_t client_body_buffer_size;
    size_t client_max_body_size;
    int listen;
    Config();
};

struct ErrorPages
{
    std::map<int, std::string> error_pages;
};

struct MimeTypes
{
    std::map<std::string, std::string> types;
};

struct Location
{
    std::string path;
    std::string root;
    std::string upload_store;
    std::vector<std::string> allowedMethods;
    std::vector<std::string> cgiExt;
    bool autoIndex;
    bool cgiPass;
    bool hasRedirect;
    bool has_tryFiles;
    size_t redirectCode;
    std::string redirectUrl;
    std::vector<std::string> try_files; 
    Location();
};


#endif
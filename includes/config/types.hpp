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

typedef struct s_config
{
    std::string server_name;
    std::string root;
    std::string index;
    int client_body_buffer_size;
    int listen;
    s_config();

} t_config;

typedef struct s_ErrorPages
{
    std::map<int, std::string> error_pages;
} t_ErrorPages;

typedef struct s_MimeTypes
{
    std::map<std::string, std::string> types;
} t_MimeTypes;

typedef struct s_Location
{
    std::string path;
    std::string root;
    std::vector<std::string> allowedMethods;
    std::vector<std::string> cgiExt;
    bool autoIndex;
    bool cgiPass;
    bool hasRedirect;
    size_t redirectCode;
    std::string redirectUrl;
    std::vector<std::string> try_files; 
    s_Location();
} t_Location;


#endif
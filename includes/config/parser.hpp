/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 15:18:48 by dsteiger          #+#    #+#             */
/*   Updated: 2026/03/17 19:37:40 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
#define PARSER_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <cctype>
#include <map>
#include <vector>
#include <http/utils/utils.hpp>
#include <cstdlib>
#include <climits>

/* typedef struct s_locations
{
    std::vector<std::string> allowed_methods = {};
    std::vector<std::string> try_files = {};
    std::string path = "";
    std::string root = ""; 
    bool autoindex = false;
    bool cgi_pass = false;

} t_locations; */

/* typedef struct s_config
{
    // std::map<std::string, std::vector<std::string>> mime = {};
    // std::map<std::string, t_locations> locations = {}; // store all location blocks
    std::string server_name = "";
    std::string root = "";
    std::string index = "";
    int client_body_buffer_size = 0;
    int listen = 0;

} t_config; */

typedef struct s_MimeTypes
{
    std::map<std::string, std::string> types;
} t_MimeTypes;

typedef struct s_Location
{
    std::string path;
    std::string root;
    std::vector<std::string> allowedMethods;
    bool autoIndex;
    bool cgiPass;
    bool regex;
    bool hasRedirect;
    size_t redirectCode;
    std::string redirectUrl;
} t_Location;

typedef struct s_parser
{
    
/*     t_config config; */
    t_MimeTypes MimeTypes;
    t_Location Location;
} t_parser;


void parse_all(const std::string &filename, t_parser &parse);

std::vector<std::string> tokenize(const std::string &filename);
void parse_server_block(const std::vector<std::string> &tokens);
//void parse_error_page(const std::vector<std::string> &tokens, size_t &i, t_config &config);
void parse_mimeTypes(t_MimeTypes& MimeTypes, size_t &i, std::vector<std::string> tokens);


#endif
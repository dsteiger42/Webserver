/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dsteiger <dsteiger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 15:18:48 by dsteiger          #+#    #+#             */
/*   Updated: 2026/03/16 20:30:25 by dsteiger         ###   ########.fr       */
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

typedef struct s_locations
{
    std::vector<std::string> allowed_methods = {};
    std::vector<std::string> try_files = {};
    std::string path = "";
    std::string root = ""; 
    bool autoindex = false;
    bool cgi_pass = false;

} t_locations;

typedef struct s_config
{
    std::map<std::string, std::vector<std::string>> mime = {};
    std::map<int, std::string> error_pages = {};
    std::string server_name = "";
    std::string root = "";
    std::string index = "";
    int client_body_buffer_size = 0;
    int listen = 0;

} t_config;

std::vector<std::string> tokenize(const std::string &filename);
void parse_server_block(const std::vector<std::string> &tokens);
void parse_error_page(const std::vector<std::string> &tokens, size_t &i, t_config &config);
void parse_all(const std::string &filename, t_config &config);

#endif
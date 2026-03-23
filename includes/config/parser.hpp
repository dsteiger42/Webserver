/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 15:18:48 by dsteiger          #+#    #+#             */
/*   Updated: 2026/03/21 05:07:54 by rafael           ###   ########.fr       */
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
#include <config/types.hpp>



typedef struct s_parser
{
    
    t_config config;
    t_ErrorPages ErrorPages;
    t_MimeTypes MimeTypes;
    std::vector<t_Location> Location;
    s_parser();
} t_parser;


void parse_all(const std::string &filename, t_parser &parse);

std::vector<std::string> tokenize(const std::string &filename);
void parse_server_block(const std::vector<std::string> &tokens, size_t &i, t_config &config);
void parse_error_page(const std::vector<std::string> &tokens, size_t &i, t_config &config);
void parse_mimeTypes(t_MimeTypes& MimeTypes, size_t &i, std::vector<std::string> &tokens);
void parse_location(t_Location &Location, size_t &i, std::vector<std::string> &tokens);
void	parse_all(const std::string &filename, t_parser &parser);

#endif
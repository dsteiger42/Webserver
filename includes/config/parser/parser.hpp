/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 15:18:48 by dsteiger          #+#    #+#             */
/*   Updated: 2026/03/24 03:02:01 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef Parser_HPP
#define Parser_HPP

#include <string>
#include <vector>
#include <map>
#include <utils/utils.hpp>
#include <config/model/types.hpp>
#include <config/parser/parsing_utils.hpp>
// #include <iostream>
// #include <fstream>
// #include <sstream>
// #include <stdexcept>
// #include <cctype>
// #include <vector>
// #include <cstdlib>
// #include <climits>


struct Parser
{
    
    Config config;
    ErrorPages errorPages;
    MimeTypes mimeTypes;
    std::vector<Location> location;
    Parser();
};


void parse_all(const std::string &filename, Parser &parse);
void parse_ServerBlock(const std::vector<std::string> &tokens, size_t &i, Config &config);
void parse_ErrorPage(const std::vector<std::string> &tokens, size_t &i, ErrorPages &errorPages);
void parse_MimeTypes(MimeTypes& MimeTypes, size_t &i, std::vector<std::string> &tokens);
void parse_Location(Location &Location, size_t &i, std::vector<std::string> &tokens);

#endif
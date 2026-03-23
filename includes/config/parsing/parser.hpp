/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: raamorim <raamorim@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 15:18:48 by dsteiger          #+#    #+#             */
/*   Updated: 2026/03/23 20:18:25 by raamorim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef Parser_HPP
#define Parser_HPP

#include <string>
#include <vector>
#include <map>
#include <http/utils/utils.hpp>
#include <config/types.hpp>
#include <config/parsing/parsing_utils.hpp>
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
    ErrorPages ErrorPages;
    MimeTypes MimeTypes;
    std::vector<Location> Location;
    Parser();
};


void parse_all(const std::string &filename, Parser &parse);
void parse_ServerBlock(const std::vector<std::string> &tokens, size_t &i, Config &config);
void parse_ErrorPage(const std::vector<std::string> &tokens, size_t &i, Config &config);
void parse_MimeTypes(MimeTypes& MimeTypes, size_t &i, std::vector<std::string> &tokens);
void parse_Location(Location &Location, size_t &i, std::vector<std::string> &tokens);

#endif
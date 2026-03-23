/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: raamorim <raamorim@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 15:18:48 by dsteiger          #+#    #+#             */
/*   Updated: 2026/03/23 19:22:19 by raamorim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
#define PARSER_HPP

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


struct parser
{
    
    Config config;
    ErrorPages ErrorPages;
    MimeTypes MimeTypes;
    std::vector<Location> Location;
    parser();
};


void parse_all(const std::string &filename, parser &parse);


void parse_server_block(const std::vector<std::string> &tokens, size_t &i, Config &config);
void parse_error_page(const std::vector<std::string> &tokens, size_t &i, Config &config);
void parse_mimeTypes(MimeTypes& MimeTypes, size_t &i, std::vector<std::string> &tokens);
void parse_location(Location &Location, size_t &i, std::vector<std::string> &tokens);

#endif
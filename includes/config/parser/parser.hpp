/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 15:18:48 by dsteiger          #+#    #+#             */
/*   Updated: 2026/04/10 15:24:06 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef Parser_HPP
# define Parser_HPP

# include <config/model/types.hpp>
# include <config/parser/parsing_utils.hpp>
# include <map>
# include <string>
# include <utils/utils.hpp>
# include <vector>

struct	ServerConfig
{
	Config		config;
	ErrorPages	errorPages;
	MimeTypes	mimeTypes;
	std::vector<Location> location;
};

struct	Parser
{
	std::vector<ServerConfig> servers;
	Parser();
};

bool	parse_all(const std::string &filename, Parser &parse);
void	parse_ServerBlock(const std::vector<std::string> &tokens, size_t &i, ServerConfig &sc);
void	parse_ErrorPage(const std::vector<std::string> &tokens, size_t &i, ErrorPages &errorPages);
void	parse_MimeTypes(MimeTypes &MimeTypes, size_t &i, const std::vector<std::string> &tokens);
void	parse_Location(Location &Location, size_t &i, const std::vector<std::string> &tokens);

#endif
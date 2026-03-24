/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 15:17:16 by dsteiger          #+#    #+#             */
/*   Updated: 2026/03/24 03:05:02 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <config/parser/parser.hpp>
#include <config/parser/Location_setters.hpp>
#include <config/parser/parsing_utils.hpp>

Config::Config() : server_name(""), root(""), index(""), client_body_buffer_size(0), listen(0)
{
}

Location::Location() : path(""), root(""), autoIndex(false), cgiPass(false),
	hasRedirect(false), has_tryFiles(false), redirectCode(0), redirectUrl("")
{
}

Parser::Parser() : config(), location() {}


void	parse_all(const std::string &filename, Parser &Parser)
{
	std::vector<std::string> tokens = Tokenize(filename);
	size_t	i = 0;
	if(!countBraces(tokens))
		return ;
	while (i < tokens.size())
	{
		if (tokens[i] == "server" && tokens[i + 1] == "{")
		{
			i += 2;
			parse_ServerBlock(tokens, i, Parser.config);
		}
		if (i + 1 < tokens.size() && tokens[i] == "mime_types" && tokens[i + 1] == "{")
			parse_MimeTypes(Parser.mimeTypes, i, tokens);
		if (i + 2 < tokens.size() && tokens[i] == "location" && tokens[i + 2] == "{")
		{
			Location loc;
			parse_Location(loc, i, tokens);
			Parser.location.push_back(loc);
		}
		else if (tokens[i] == "error_page")
		{
			i++;
			parse_ErrorPage(tokens, i, Parser.errorPages);
		}
		i++;
	}
}


/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 15:17:16 by dsteiger          #+#    #+#             */
/*   Updated: 2026/04/10 15:23:57 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <config/parser/Location_setters.hpp>
#include <config/parser/parser.hpp>
#include <config/parser/parsing_utils.hpp>

Config::Config() : server_name(""), root(""), index(""),
	client_body_buffer_size(0), client_max_body_size(0), listen(0)
{
}

Location::Location() : path(""), root(""), upload_store(""), autoIndex(false), cgiPass(false),
	hasRedirect(false), has_tryFiles(false), redirectCode(0), redirectUrl("")
{
}

Parser::Parser() : servers()
{
}

bool	parse_all(const std::string &filename, Parser &parser)
{
	size_t	i;
	ServerConfig sc;

	std::vector<std::string> tokens = Tokenize(filename);
	if (!countBraces(tokens))
		return false;
	i = 0;
	while (i < tokens.size())
	{
		if (tokens[i] == "server" && i + 1 < tokens.size() && tokens[i
			+ 1] == "{")
		{
			i += 2; // skip "server" and "{"
			parse_ServerBlock(tokens, i, sc);
			parser.servers.push_back(sc);
		}
		else
			i++;
	}
	return true;
}

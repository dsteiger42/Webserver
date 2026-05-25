/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 15:17:16 by dsteiger          #+#    #+#             */
/*   Updated: 2026/05/14 05:42:10 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <config/parser/Location_setters.hpp>
#include <config/parser/parser.hpp>
#include <config/parser/parsing_utils.hpp>

Config::Config() : server_name(""), root(""), index(""),
	client_max_body_size(0)
{
}

Location::Location() : index(""), path(""), root(""), upload_store(""), autoIndex(false), cgiPass(false),
	hasRedirect(false), has_tryFiles(false), redirectCode(0), redirectUrl(""), client_max_body_size(0)
{
}

Parser::Parser() : serverBlocks()
{
}

bool	parse_all(const std::string &filename, Parser &parser)
{
	size_t	i;

	std::vector<std::string> tokens;
	try
    {
        tokens = Tokenize(filename);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return false;
    }
	if (!countBraces(tokens))
		return false;
	i = 0;
	while (i < tokens.size())
	{
		if (tokens[i] == "server" && i + 1 < tokens.size() && tokens[i
			+ 1] == "{")
		{
			i += 2;
			ServerConfig sc;
			if (!parse_ServerBlock(tokens, i, sc))
				return false;
			parser.serverBlocks.push_back(sc);
		}
		else
			i++;
	}
	return true;
}

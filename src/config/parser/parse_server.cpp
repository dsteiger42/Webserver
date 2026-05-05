/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_server.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 00:54:23 by rafael            #+#    #+#             */
/*   Updated: 2026/05/05 18:02:34 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <config/parser/parser.hpp>

bool	parse_ServerBlock(const std::vector<std::string> &tokens, size_t &i, ServerConfig &sc)
{
	while (i < tokens.size() && tokens[i] != "}")
	{
		if (tokens[i] == "listen" && i + 2 < tokens.size())
		{
			if (!is_Number(tokens[i + 1]))
				return false;
			sc.config.listen = std::atoi(tokens[i + 1].c_str());
			if (sc.config.listen < 1 || sc.config.listen > 65535)
				return false; //invalid ports
			i += 3; // skip "listen", value, ";"
		}
		else if (tokens[i] == "server_name" && i + 2 < tokens.size())
		{
			sc.config.server_name = tokens[i + 1];
			i += 3;
		}
		else if (tokens[i] == "root" && i + 2 < tokens.size())
		{
			sc.config.root = tokens[i + 1];
			i += 3;
		}
		else if (tokens[i] == "index" && i + 2 < tokens.size())
		{
			sc.config.index = tokens[i + 1];
			i += 3;
		}
		else if (tokens[i] == "client_max_body_size" && i + 2 < tokens.size())
		{
			if (!is_Number(tokens[i + 1]))
				return false;
			sc.config.client_max_body_size = std::atoi(tokens[i + 1].c_str());
			if (sc.config.client_max_body_size > INT_MAX)
				return false;
			i += 3;
		}
		else if (tokens[i] == "error_page" && i + 2 < tokens.size())
		{
			i++;
			if (!parse_ErrorPage(tokens, i, sc.errorPages))
				return false;
		}
		else if (tokens[i] == "mime_types" && i + 1 < tokens.size() && tokens[i + 1] == "{")
		{
			if (!parse_MimeTypes(sc.mimeTypes, i, tokens)) // make sure this advances i past "}"
				return false;
		}
		else if (tokens[i] == "location" && i + 2 < tokens.size() && tokens[i + 2] == "{")
		{
			Location loc;
			if (!parse_Location(loc, i, tokens))
				return false; // make sure this advances i past "}"
			sc.location.push_back(loc);
		}
		else
			i++;
	}
	i++; // skip the closing "}"
	if (sc.location.empty())
        return false;
	return true;
}

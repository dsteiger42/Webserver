/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_server.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 00:54:23 by rafael            #+#    #+#             */
/*   Updated: 2026/05/06 19:01:39 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <config/parser/parser.hpp>

bool	parse_ServerBlock(const std::vector<std::string> &tokens, size_t &i,
		ServerConfig &sc)
{
	int	port;
	Location loc;

	while (i < tokens.size() && tokens[i] != "}")
	{
		if (tokens[i] == "listen" && i + 2 < tokens.size())
		{
			if (!is_Number(tokens[i + 1]))
				return (false);
			port = std::atoi(tokens[i + 1].c_str());
			if (port < 1 || port > 65535)
				return (false);
			sc.config.listen.push_back(port);
			i += 3;
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
				return (false);
			sc.config.client_max_body_size = std::atoi(tokens[i + 1].c_str());
			if (sc.config.client_max_body_size > INT_MAX)
				return (false);
			i += 3;
		}
		else if (tokens[i] == "error_page" && i + 2 < tokens.size())
		{
			i++;
			if (!parse_ErrorPage(tokens, i, sc.errorPages))
				return (false);
		}
		else if (tokens[i] == "mime_types" && i + 1 < tokens.size() && tokens[i
			+ 1] == "{")
		{
			if (!parse_MimeTypes(sc.mimeTypes, i, tokens))
				return (false);
		}
		else if (tokens[i] == "location" && i + 2 < tokens.size() && tokens[i
			+ 2] == "{")
		{
			if (!parse_Location(loc, i, tokens))
				return (false);
			sc.location.push_back(loc);
		}
		else
			i++;
	}
	i++;
	if (sc.location.empty())
		return (false);
	return (true);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_location.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 00:53:07 by rafael            #+#    #+#             */
/*   Updated: 2026/03/24 03:05:02 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <config/parser/Location_setters.hpp>
#include <config/parser/parser.hpp>

void	parse_Location(Location &Location, size_t &i,
		const std::vector<std::string> &tokens)
{
	i++;
	set_Path(tokens, i, Location);
	i++;
	if (tokens[i] != "{")
		throw std::runtime_error("Expected '{'");
	while (i < tokens.size() && tokens[i] != "}")
	{
		if (tokens[i] == "autoindex")
		{
			i++;
			set_Autoindex(tokens[i], Location);
		}
		if (tokens[i] == "allowed_methods" && !tokens[i + 1].empty())
		{
			i++;
			set_AllowedMethods(tokens, i, Location);
		}
		if (tokens[i] == "root" && !tokens[i + 1].empty())
		{
			i++;
			set_Root(tokens[i], Location);
		}
		if (tokens[i] == "cgi_pass" && !tokens[i + 1].empty())
		{
			i++;
			set_CgiPass(tokens[i], Location);
		}
		if (tokens[i] == "return" && !tokens[i + 1].empty())
		{
			i++;
			set_Redirection(tokens, i, Location);
		}
		if (tokens[i] == "try_files" && !tokens[i + 1].empty())
		{
			i++;
			set_TryFiles(tokens, i, Location);
			Location.has_tryFiles = true;
		}
		if (tokens[i] == "cgi_ext" && !tokens[i + 1].empty())
		{
			i++;
			set_CgiExt(tokens, i, Location);
		}
		i++;
	}
	i++;
}

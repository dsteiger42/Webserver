/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_location.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 00:53:07 by rafael            #+#    #+#             */
/*   Updated: 2026/04/15 02:16:57 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <config/parser/Location_setters.hpp>

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
		if (tokens[i] == "allowed_methods" && i + 1 < tokens.size() && !tokens[i + 1].empty())
		{
			i++;
			set_AllowedMethods(tokens, i, Location);
		}
		if (tokens[i] == "root" && i + 1 < tokens.size() && !tokens[i + 1].empty())
		{
			i++;
			set_Root(tokens[i], Location);
		}
		if (tokens[i] == "cgi_pass" && i + 1 < tokens.size() && !tokens[i + 1].empty())
		{
			i++;
			set_CgiPass(tokens[i], Location);
		}
		if (tokens[i] == "return" && i + 1 < tokens.size() && !tokens[i + 1].empty())
		{
			i++;
			set_Redirection(tokens, i, Location);
		}
		if (tokens[i] == "try_files" && i + 1 < tokens.size() && !tokens[i + 1].empty())
		{
			i++;
			set_TryFiles(tokens, i, Location);
			Location.has_tryFiles = true;
		}
		if (tokens[i] == "cgi_ext" && i + 1 < tokens.size() && !tokens[i + 1].empty())
		{
			i++;
			set_CgiExt(tokens, i, Location);
		}
		if (tokens[i] == "cgi_types" && i + 1 < tokens.size() && !tokens[i + 1].empty() && tokens[i + 1] == "{")
		{
			parse_CgiTypes(tokens, i, Location);
		}
		if (tokens[i] == "upload_store" && i + 1 < tokens.size() && !tokens[i + 1].empty())
		{
			i++;
			if (tokens[i] != ";")
				Location.upload_store = tokens[i];
		}
		i++;
	}
	i++;
}

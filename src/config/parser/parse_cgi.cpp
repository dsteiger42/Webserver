/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_cgi.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/15 01:57:10 by rafael            #+#    #+#             */
/*   Updated: 2026/04/29 19:27:15 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <config/parser/Location_setters.hpp>

bool	parse_CgiTypes(const std::vector<std::string> &tokens, size_t &i, Location &location)
{
	i++;
	if (tokens[i] != "{")
		return false;
	i++;
	while (i < tokens.size() && tokens[i] != "}")
	{
		std::string type = tokens[i];
		if (!type.empty() && type[0] == '.')
			type = type.substr(1);
		i++;
		while (i < tokens.size() && tokens[i] != ";")
		{
			std::string value = tokens[i];
			location.cgiPath[type] = value;
			i++;
		}
		i++;
	}
	return true;
}
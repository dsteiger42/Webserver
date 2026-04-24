/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_cgi.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/15 01:57:10 by rafael            #+#    #+#             */
/*   Updated: 2026/04/16 14:52:18 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <config/parser/Location_setters.hpp>

void	parse_CgiTypes(const std::vector<std::string> &tokens, size_t &i, Location &location)
{
	i++;
	if (tokens[i] != "{")
		throw std::runtime_error("Expected '{'");
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
}
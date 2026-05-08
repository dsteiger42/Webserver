/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_mime.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 01:01:35 by rafael            #+#    #+#             */
/*   Updated: 2026/04/29 19:28:33 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <config/parser/parser.hpp>

bool	parse_MimeTypes(MimeTypes &MimeTypes, size_t &i, const std::vector<std::string> &tokens)
{
	i++;
	if (tokens[i] != "{")
		return false;
	i++;
	while (i < tokens.size() && tokens[i] != "}")
	{
		std::string type = tokens[i];
		i++;
		while (i < tokens.size() && tokens[i] != ";")
		{
			std::string value = tokens[i];
			MimeTypes.types[value] = type;
			i++;
		}
		i++;
	}
	i++;
	return true;
}

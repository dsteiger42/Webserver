/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_mime.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 01:01:35 by rafael            #+#    #+#             */
/*   Updated: 2026/03/24 01:02:08 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <config/parser/parser.hpp>

void	parse_MimeTypes(MimeTypes &MimeTypes, size_t &i,
		const std::vector<std::string> &tokens)
{
	i++;
	if (tokens[i] != "{")
		throw std::runtime_error("Expected '{'");
	i++;
	while (i < tokens.size() && tokens[i] != "}")
	{
		std::string type = tokens[i];
		i++;
		while (tokens[i] != ";")
		{
			std::string value = tokens[i];
			MimeTypes.types[value] = type;
			i++;
		}
		i++;
	}
	i++;
}

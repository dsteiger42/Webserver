/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_utils.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 01:31:55 by rafael            #+#    #+#             */
/*   Updated: 2026/03/24 02:59:09 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <config/parser/parsing_utils.hpp>

std::vector<std::string> Tokenize(const std::string &filename)
{
	char	c;
	std::vector<std::string> tokens;
	std::string line;
	std::ifstream file(filename.c_str());
	if (!file.is_open())
		throw std::runtime_error("Error: Cannot open config file");
	while (std::getline(file, line))
	{
		std::string current;
		for (size_t i = 0; i < line.size(); i++)
		{
			c = line[i];
			if (c == '#')
				break ;
			if (std::isspace(c))
			{
				if (!current.empty())
				{
					tokens.push_back(current);
					current.clear();
				}
				continue ;
			}
			if (c == '{' || c == '}' || c == ';')
			{
				if (!current.empty())
				{
					tokens.push_back(current);
					current.clear();
				}
				tokens.push_back(std::string(1, c));
				continue ;
			}
			current += c;
		}
		if (!current.empty())
			tokens.push_back(current);
	}
	return (tokens);
}

bool countBraces(std::vector<std::string> &tokens)
{
	int braces_left = 0;
	int braces_right = 0;
	for (size_t i = 0; i < tokens.size(); i++)
	{
		if(tokens[i] == "{")
			braces_left++;
		if(tokens[i] == "}")
			braces_right++;
	}
	if(braces_left != braces_right)
		return false;
	return true;
}

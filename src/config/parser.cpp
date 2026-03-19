/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dsteiger <dsteiger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 15:17:16 by dsteiger          #+#    #+#             */
/*   Updated: 2026/03/19 15:20:57 by dsteiger         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <config/parser.hpp>

std::vector<std::string> tokenize(const std::string &filename)
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

void	parse_error_page(const std::vector<std::string> &tokens, size_t &i, t_config &config)
{
	int	code;

	if (i + 3 >= tokens.size())
		return ;
	code = std::atoi(tokens[i + 1].c_str());
	std::string path = tokens[i + 2];
	config.error_pages[code] = path;
	i += 4;
}

void	parse_server_block(const std::vector<std::string> &tokens)
{
	t_config	config;
	size_t		i;

	i = 0;
	while (i < tokens.size())
	{
		if (tokens[i] == "listen" && i + 2 < tokens.size())
		{
			config.listen = std::atoi(tokens[i + 1].c_str());
			i += 3;
		}
		else if (tokens[i] == "server_name" && i + 2 < tokens.size())
		{
			config.server_name = tokens[i + 1].c_str();
			i += 3;
		}
		else if (tokens[i] == "root" && i + 2 < tokens.size())
		{
			config.root = tokens[i + 1].c_str();
			i += 3;
		}
		else if (tokens[i] == "index" && i + 2 < tokens.size())
		{
			config.index = tokens[i + 1].c_str();
			i += 3;
		}
		else if (tokens[i] == "client_body_buffer_size" && i + 2 < tokens.size())
		{
			config.client_body_buffer_size = std::atoi(tokens[i + 1].c_str());
			i += 3;
		}
		else if (tokens[i] == "error_page")
		{
			parse_error_page(tokens, i, config);
		}
		else
			i++;
	}
}

void	parse_all(const std::string &filename, t_config &config)
{
	size_t	i;
	int		brace_level;

	std::vector<std::string> tokens = tokenize(filename);
	/* std::cout << "All tokens:\n";
	for (size_t j = 0; j < tokens.size(); j++)
		std::cout << j << ": [" << tokens[j] << "]\n"; */
	i = 0;
	while (i < tokens.size())
	{
		if (tokens[i] == "server" && i + 1 < tokens.size() && tokens[i + 1] == "{")
		{
			i += 2;
			brace_level = 1;
			std::vector<std::string> server_tokens;
			while (i < tokens.size() && brace_level > 0)
			{
				if (tokens[i] == "{")
					brace_level++;
				else if (tokens[i] == "}")
					brace_level--;
				if (brace_level > 0)
					server_tokens.push_back(tokens[i]);
				i++;
			}
			/* std::cout << "Server tokens:\n";
			for (size_t j = 0; j < server_tokens.size(); j++)
				std::cout << j << ": [" << server_tokens[j] << "]\n"; */
			parse_server_block(server_tokens);
		}
		else
			i++;
	}
}

void	parse_mimeTypes(t_MimeTypes &MimeTypes, size_t &i,
		std::vector<std::string> tokens)
{
	i++; // anda para {
	if (tokens[i] != "{")
		throw std::runtime_error("Expected '{'");
	i++;
	while (tokens[i] != "}")
	{
		std::string type = tokens[i];
		std::cout << "type: " << type << std::endl;
		i++;
		size_t prev = i;
		while (tokens[i] != ";")
		{
			std::string value = tokens[i];
			if (i > prev)
				MimeTypes.types[type] += value;
			else
				MimeTypes.types[type] = value;
			i++;
		}
		i++;
	}
	i++;
}

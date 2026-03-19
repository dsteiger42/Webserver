/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dsteiger <dsteiger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 15:17:16 by dsteiger          #+#    #+#             */
/*   Updated: 2026/03/19 18:21:03 by dsteiger         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <config/parser.hpp>

s_config::s_config() : server_name(""), root(""), index(""), client_body_buffer_size(0), listen(0)
{
}

s_Location::s_Location() : path(""), root(""), autoIndex(false), cgiPass(false),
	hasRedirect(false), redirectCode(0), redirectUrl("")
{
}

s_parser::s_parser() : config(), Location() {}

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

void	parse_server_block(const std::vector<std::string> &tokens, size_t &i, t_config &config)
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

static bool count_braces(std::vector<std::string> &tokens)
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

void	parse_all(const std::string &filename, t_parser &parser)
{
	std::vector<std::string> tokens = tokenize(filename);
	size_t	i = 0;
	if(!count_braces(tokens))
		return ;
	while (i < tokens.size())
	{
		if (tokens[i] == "server" && tokens[i + 1] == "{")
		{
			i += 2;
			parse_server_block(tokens, i, parser.config);
		}
		if (i + 1 < tokens.size() && tokens[i] == "mime_types" && tokens[i + 1] == "{")
			parse_mimeTypes(parser.MimeTypes, i, tokens);
		if (i + 2 < tokens.size() && tokens[i] == "location" && tokens[i + 2] == "{")
		{
			t_Location loc;
			parse_location(loc, i, tokens);
			parser.Location.push_back(loc);
		}
		i++;
	}
}

static void	set_Path(std::vector<std::string> &tokens, size_t &i,
		t_Location &location)
{
	if (tokens[i].empty())
		return ;
	if (!tokens[i].empty())
		location.path = tokens[i];
	else 
		location.path = "/";
}
static void	set_Autoindex(std::string &value, t_Location &location)
{
	if (value == "on")
		location.autoIndex = true;
	else
		location.autoIndex = false;
}

static void	set_AllowedMethods(std::vector<std::string> &tokens, size_t &i,
		t_Location &location)
{
	if (!tokens[i].empty())
	{
		while (validateMethod(tokens[i]))
		{
			//trocar
			location.allowedMethods.push_back(tokens[i]);
			i++;
		}
	}
}

static void	set_Root(std::string &value, t_Location &location)
{
	if (!value.empty() && value != ";")
		location.root = value;
	else
		location.root = "";
}

static void	set_cgiPass(std::string &value, t_Location &location)
{
	if (value == "on")
		location.cgiPass = true;
	else
		location.cgiPass = false;
}

static void	set_redirection(std::vector<std::string> &tokens, size_t &i,
		t_Location &location)
{
	long	code;

	if (!isNumber(tokens[i]))
		return ;
	location.hasRedirect = true;
	code = std::atol(tokens[i].c_str());
	if (code > INT_MAX || code < INT_MIN)
		location.redirectCode = 0;
	else
		location.redirectCode = code;
	i++;
	if (tokens[i] != ";")
		location.redirectUrl = tokens[i];
	else
		location.redirectUrl = "";
}

static void	set_tryFiles(std::vector<std::string> &tokens, size_t &i,
		t_Location &location)
{
	if (!tokens[i].empty())
	{
		while (tokens[i] != ";")
		{
			location.try_files.push_back(tokens[i]);
			i++;
		}
	}
}

static void set_cgiExt(std::vector<std::string> &tokens, size_t &i, t_Location &location)
{
	if (!tokens[i].empty())
	{
		while(tokens[i] != ";")
		{
			location.cgiExt.push_back(tokens[i]);
			i++;
		}
	}
}

void	parse_location(t_Location &Location, size_t &i,
		std::vector<std::string> &tokens)
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
			set_cgiPass(tokens[i], Location);
		}
		if (tokens[i] == "return" && !tokens[i + 1].empty())
		{
			i++;
			set_redirection(tokens, i, Location);
		}
		if (tokens[i] == "try_files" && !tokens[i + 1].empty())
		{
			i++;
			set_tryFiles(tokens, i, Location);
		}
		if (tokens[i] == "cgi_ext" && !tokens[i + 1].empty())
		{
			i++;
			set_cgiExt(tokens, i, Location);
		}
		i++;
	}
}

void	parse_mimeTypes(t_MimeTypes &MimeTypes, size_t &i,
		std::vector<std::string> &tokens)
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

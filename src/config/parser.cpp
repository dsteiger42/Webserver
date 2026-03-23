/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: raamorim <raamorim@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 15:17:16 by dsteiger          #+#    #+#             */
/*   Updated: 2026/03/23 17:47:31 by raamorim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <config/parsing/parser.hpp>
#include <config/parsing/setters.hpp>
#include <config/parsing/parsing_utils.hpp>

s_config::s_config() : server_name(""), root(""), index(""), client_body_buffer_size(0), listen(0)
{
}

s_Location::s_Location() : path(""), root(""), autoIndex(false), cgiPass(false),
	hasRedirect(false), has_tryFiles(false), redirectCode(0), redirectUrl("")
{
}

s_parser::s_parser() : config(), Location() {}


void	parse_error_page(const std::vector<std::string> &tokens, size_t &i, t_ErrorPages &errorPages)
{
	int	code;

	if (i + 2 >= tokens.size())
		return ;
	code = std::atol(tokens[i].c_str());
	if (code > INT_MAX || code < INT_MIN)
		return ;
	i++;
	std::string path = tokens[i];
	errorPages.error_pages[code] = path;
}

void	parse_server_block(const std::vector<std::string> &tokens, size_t &i, t_config &config)
{
	//melhorar, o erro era os else ifs
	if (tokens[i] == "listen" && i + 2 < tokens.size())
	{
		config.listen = std::atoi(tokens[i + 1].c_str());
		i += 3;
	}
	if (tokens[i] == "server_name" && i + 2 < tokens.size())
	{
		config.server_name = tokens[i + 1].c_str();
		i += 3;
	}
	if (tokens[i] == "root" && i + 2 < tokens.size())
	{
		config.root = tokens[i + 1].c_str();
		i += 3;
	}
	if (tokens[i] == "index" && i + 2 < tokens.size())
	{
		config.index = tokens[i + 1].c_str();
		i += 3;
	}
	if (tokens[i] == "client_body_buffer_size" && i + 2 < tokens.size())
	{
		config.client_body_buffer_size = std::atoi(tokens[i + 1].c_str());
		i += 3;
	}
	else
		i++;
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
		else if (tokens[i] == "error_page")
		{
			i++;
			parse_error_page(tokens, i, parser.ErrorPages);
		}
		i++;
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
			Location.has_tryFiles = true;
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

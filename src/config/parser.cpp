/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 15:17:16 by dsteiger          #+#    #+#             */
/*   Updated: 2026/03/24 00:25:32 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <config/parsing/parser.hpp>
#include <config/parsing/Location_setters.hpp>
#include <config/parsing/parsing_utils.hpp>

Config::Config() : server_name(""), root(""), index(""), client_body_buffer_size(0), listen(0)
{
}

Location::Location() : path(""), root(""), autoIndex(false), cgiPass(false),
	hasRedirect(false), has_tryFiles(false), redirectCode(0), redirectUrl("")
{
}

Parser::Parser() : config(), location() {}


void	parse_ErrorPage(const std::vector<std::string> &tokens, size_t &i, ErrorPages &errorPages)
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

void	parse_ServerBlock(const std::vector<std::string> &tokens, size_t &i, Config &config)
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

void	parse_all(const std::string &filename, Parser &Parser)
{
	std::vector<std::string> tokens = Tokenize(filename);
	size_t	i = 0;
	if(!countBraces(tokens))
		return ;
	while (i < tokens.size())
	{
		if (tokens[i] == "server" && tokens[i + 1] == "{")
		{
			i += 2;
			parse_ServerBlock(tokens, i, Parser.config);
		}
		if (i + 1 < tokens.size() && tokens[i] == "mime_types" && tokens[i + 1] == "{")
			parse_MimeTypes(Parser.mimeTypes, i, tokens);
		if (i + 2 < tokens.size() && tokens[i] == "location" && tokens[i + 2] == "{")
		{
			Location loc;
			parse_Location(loc, i, tokens);
			Parser.location.push_back(loc);
		}
		else if (tokens[i] == "error_page")
		{
			i++;
			parse_ErrorPage(tokens, i, Parser.errorPages);
		}
		i++;
	}
}


void	parse_Location(Location &Location, size_t &i,
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
}

void	parse_MimeTypes(MimeTypes &MimeTypes, size_t &i,
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

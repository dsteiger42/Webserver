/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 15:17:16 by dsteiger          #+#    #+#             */
/*   Updated: 2026/03/17 19:40:17 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <config/parser.hpp>

std::vector<std::string> tokenize(const std::string &filename)
{
    std::vector<std::string> tokens;
    std::string line;
	char	c;

	std::ifstream file(filename.c_str());
	if (!file.is_open())
		throw std::runtime_error("Cannot open config file");

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

/* void	parse_server_block(const std::vector<std::string> &tokens)
{
    t_config config;
    size_t i = 0;
    
    while(i < tokens.size())
    {
        if (tokens[i] == "listen" && i + 1 < tokens.size())
        {
            config.listen = std::atoi(tokens[i + 1].c_str());
            i += 2;
        }
        else if(tokens[i] == "server_name" && i + 1 < tokens.size())
        {
            config.server_name = tokens[i + 1].c_str();
            i += 2;
        }
        else if(tokens[i] == "root" && i + 1 < tokens.size())
        {
            config.root = tokens[i + 1].c_str();
            i += 2;
        }
        else if(tokens[i] == "index" && i + 1 < tokens.size())
        {
            config.index = tokens[i + 1].c_str();
            i += 2;
        }
        else if(tokens[i] == "client_body_buffer_size" && i + 1 < tokens.size())
        {
            config.client_body_buffer_size = std::atoi(tokens[i + 1].c_str());
            i += 2;
        }
        else
            i++;
    }
}

void parse_error_page(const std::vector<std::string> &tokens, size_t &i, t_config &config)
{
    
} */



void	parse_all(const std::string &filename, t_parser &parser)
{
    std::vector<std::string> tokens = tokenize(filename);
    std::cout << "All tokens:\n";
    for (size_t j = 0; j < tokens.size(); j++)
        std::cout << j << ": [" << tokens[j] << "]\n";
    size_t i = 0;

    while(i < tokens.size())
    {
        if(tokens[i] == "server" && tokens[i + 1] == "{")
        {
            /* i += 2;
            std::vector<std::string> server_tokens;
            while(i < tokens.size() && tokens[i] != "error_page")
            {
                server_tokens.push_back(tokens[i]);
                i++;
            }
            std::cout << "Server tokens:\n";
            for (size_t j = 0; j < server_tokens.size(); j++)
                std::cout << j << ": [" << server_tokens[j] << "]\n";
            parse_server_block(server_tokens); */
        }
        if (i + 1 < tokens.size() && tokens[i] == "mime_types" && tokens[i + 1] == "{")
            parse_mimeTypes(parser.MimeTypes, i, tokens);
        if (i + 2 < tokens.size() && tokens[i] == "location" && tokens[i + 2] == "{")
            /* parse_mimeTypes(parser.MimeTypes, i, tokens); */
        i++;
    }
}

static void set_Autoindex(std::string &value, t_Location &location)
{
    if (value == "true")
        location.autoIndex = true;
    else
        location.autoIndex = false;
}

static void set_AllowedMethods(std::vector<std::string> &tokens, size_t &i, t_Location &location)
{
    if (!tokens[i].empty())
    {
        while(validateMethod(tokens[i]))
        {
            location.allowedMethods.push_back(tokens[i]);
            i++;
        }
    }
}

static void set_Root(std::string &value, t_Location &location)
{
    if (!value.empty() && value != ";")
        location.root = value;
    else
        location.root = "";
}

static void set_cgiPass(std::string &value, t_Location &location)
{
    if (value == "on")
        location.cgiPass = true;
    else
        location.cgiPass = false;
}

static void set_redirection(std::vector<std::string> tokens, size_t &i, t_Location &location)
{
    if (!isNumber(tokens[i]))
        return ;
    location.hasRedirect = true;
    long code = std::atol(tokens[i].c_str());
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

void parse_location(t_Location &Location, size_t &i, std::vector<std::string> tokens)
{
    i++;
    Location.path = tokens[i];
    i++;
    if (tokens[i] != "{")
        throw std::runtime_error("Expected '{'");
    while(tokens[i] != "}")
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
        i++;   
    }
}


void parse_mimeTypes(t_MimeTypes& MimeTypes, size_t &i, std::vector<std::string> tokens)
{
    i++;
    if (tokens[i] != "{")
        throw std::runtime_error("Expected '{'");
    i++;
    while(tokens[i] != "}")
    {
        std::string type =  tokens[i];
        i++;
        while(tokens[i] != ";")
        {
            std::string value = tokens[i];
            MimeTypes.types[value] = type;    
            i++;
        }
        i++;
    }
    i++;
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location_setters.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/23 17:33:01 by raamorim          #+#    #+#             */
/*   Updated: 2026/04/08 03:16:34 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <config/parser/parser.hpp>

void	set_Path(const std::vector<std::string> &tokens, size_t &i,
		Location &location)
{
	if (!tokens[i].empty())
		location.path = tokens[i];
	else
		location.path = "/";
}
void	set_Autoindex(const std::string &value, Location &location)
{
	if (value == "on")
		location.autoIndex = true;
	else
		location.autoIndex = false;
}

void	set_AllowedMethods(const std::vector<std::string> &tokens, size_t &i,
		Location &location)
{
	if (!tokens[i].empty())
	{
		while (i < tokens.size() && validate_Method(tokens[i]))
		{
			location.allowedMethods.push_back(tokens[i]);
			i++;
		}
	}
}

void	set_Root(const std::string &value, Location &location)
{
	if (!value.empty() && value != ";")
		location.root = value;
	else
		location.root = "";
}

void	set_CgiPass(const std::string &value, Location &location)
{
	if (value == "on")
		location.cgiPass = true;
	else
		location.cgiPass = false;
}

void	set_Redirection(const std::vector<std::string> &tokens, size_t &i,
		Location &location)
{
	long	code;

	if (!is_Number(tokens[i]))
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

void	set_TryFiles(const std::vector<std::string> &tokens, size_t &i,
		Location &location)
{
	while (i < tokens.size() && tokens[i] != ";")
	{
		location.try_files.push_back(tokens[i]);
		i++;
	}
}

void	set_CgiExt(const std::vector<std::string> &tokens, size_t &i,
		Location &location)
{
	while (i < tokens.size() && tokens[i] != ";")
	{
		location.cgiExt.push_back(tokens[i]);
		i++;
	}
}

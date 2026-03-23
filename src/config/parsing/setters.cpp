/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setters.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: raamorim <raamorim@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/23 17:33:01 by raamorim          #+#    #+#             */
/*   Updated: 2026/03/23 17:40:30 by raamorim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <config/parsing/parser.hpp>

void	set_Path(std::vector<std::string> &tokens, size_t &i,
		t_Location &location)
{
	if (tokens[i].empty())
		return ;
	if (!tokens[i].empty())
		location.path = tokens[i];
	else 
		location.path = "/";
}
void	set_Autoindex(std::string &value, t_Location &location)
{
	if (value == "on")
		location.autoIndex = true;
	else
		location.autoIndex = false;
}

void	set_AllowedMethods(std::vector<std::string> &tokens, size_t &i,
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

void	set_Root(std::string &value, t_Location &location)
{
	if (!value.empty() && value != ";")
		location.root = value;
	else
		location.root = "";
}

void	set_cgiPass(std::string &value, t_Location &location)
{
	if (value == "on")
		location.cgiPass = true;
	else
		location.cgiPass = false;
}

void	set_redirection(std::vector<std::string> &tokens, size_t &i,
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

void	set_tryFiles(std::vector<std::string> &tokens, size_t &i,
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

void set_cgiExt(std::vector<std::string> &tokens, size_t &i, t_Location &location)
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
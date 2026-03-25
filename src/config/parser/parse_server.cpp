/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_server.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 00:54:23 by rafael            #+#    #+#             */
/*   Updated: 2026/03/25 23:01:31 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <config/parser/parser.hpp>

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
		if (config.client_body_buffer_size > INT_MAX)
			return ;
		i += 3;
		//adicionar handle MB, GB
	}
	if (tokens[i] == "client_max_body_size" && i + 2 < tokens.size())
	{
		config.client_max_body_size = std::atoi(tokens[i + 1].c_str());
		if (config.client_body_buffer_size > INT_MAX)
			return ;
		i += 3;
	}
	else
		i++;
}
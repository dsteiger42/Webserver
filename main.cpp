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

#include <config/parser/parser.hpp>
#include <core/server.hpp>
#include <utils/signals/signals.hpp>

int	main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cerr << "Error: wrong number of arguments" << std::endl;
		return (-1);
	}
	signal(SIGINT, handle_Sigint);
	Parser parser;
	parse_all(argv[1], parser);
	if (parser.servers.empty())
	{
		std::cerr << "Error: no server blocks found in config" << std::endl;
		return (-1);
	}
	std::vector<Server> server;
	for (size_t i = 0; i < parser.servers.size(); i++)
		server.push_back(Server(parser.servers[i].config.listen, parser.servers[i]));
	for (size_t i = 0; i < server.size(); i++)
	{
		if (server[i].setup_Socket() == -1)
			return (-1);
	}
	Server::handle_Clients(server);
	return (0);
}

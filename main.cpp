/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 01:31:55 by rafael            #+#    #+#             */
/*   Updated: 2026/04/10 15:25:01 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <config/parser/parser.hpp>
#include <core/server.hpp>
#include <utils/signals/signals.hpp>

int	main(int argc, char **argv)
{
    std::string configFile;
    if (argc == 1)
        configFile = "./webserver.conf";
    else if (argc == 2)
        configFile = argv[1];
    else
    {
        std::cerr << "Usage: ./webserv [configuration file]" << std::endl;
        return -1;
    }
    Parser parser;
    if (!parse_all(configFile, parser))
    {
        std::cerr << "Error: something wrong in config file" << std::endl;
        return -1;
    }
    if (parser.servers.empty())
    {
        std::cerr << "Error: no server blocks found in config" << std::endl;
        return -1;
    }
    std::vector<Server> servers;
    for (size_t i = 0; i < parser.servers.size(); i++)
        servers.push_back(Server(parser.servers[i].config.listen, parser.servers[i]));
    for (size_t i = 0; i < servers.size(); i++)
    {
        if (servers[i].setup_Socket() == -1)
            return -1;
    }
    Server::handle_Clients(servers);
    return 0;
}

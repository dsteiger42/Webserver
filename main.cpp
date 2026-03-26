#include <core/server.hpp>
#include <config/parser/parser.hpp>

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "Error: wrong number of arguments" << std::endl;
        return -1;
    }
    Parser parser;
    parse_all(argv[1], parser);
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
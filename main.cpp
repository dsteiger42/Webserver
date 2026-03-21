#include <server/server.hpp>
#include <config/parser.hpp>
/*
Create a port variable that will read the conf.file. then call the constructor with that port variable,
which is assign to _port in the constructor. Then _port is called in the struct sockaddr_in and assigned in bind().
*/
void printErrorPages(const t_ErrorPages &errorPages, const std::string &root)
{
    std::cout << "---- DEBUG: ErrorPages ----" << std::endl;
    std::map<int, std::string>::const_iterator it;
    for (it = errorPages.error_pages.begin(); it != errorPages.error_pages.end(); ++it)
    {
        std::cout << "Code: " << it->first << " -> Path: " << it->second;

        // verificar se o ficheiro existe
        std::ifstream f((root + it->second).c_str());
        if (!f)
            std::cout << " (Ficheiro NAO encontrado!)";
        else
            std::cout << " (Ficheiro OK)";
        std::cout << std::endl;
    }
    std::cout << "---------------------------" << std::endl;
}

int main(int argc, char **argv)
{
    t_parser parser;
    if(argc != 2)
    {
        std::cerr << "Error: wrong number of arguments" << std::endl;
        return -1;
    }
    parse_all(argv[1], parser);
    printErrorPages(parser.ErrorPages, parser.config.root);   
    //int port = parseListenPort(); -> parse the "listen" in webserver.conf
    //server myServer(port);

    // used for testing a connection
    Server myServer(8080, parser);
    myServer.setup_socket();
    myServer.handle_clients();
    return 0;
}
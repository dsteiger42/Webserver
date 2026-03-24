#include <server/server.hpp>
#include <config/parser/parser.hpp>/*
Create a port variable that will read the conf.file. then call the constructor with that port variable,
which is assign to _port in the constructor. Then _port is called in the struct sockaddr_in and assigned in bind().
*/

int main(int argc, char **argv)
{
    Parser Parser;
    if(argc != 2)
    {
        std::cerr << "Error: wrong number of arguments" << std::endl;
        return -1;
    }
    parse_all(argv[1], Parser);
    //int port = parseListenPort(); -> parse the "listen" in webserver.conf
    //server myServer(port);

    // used for testing a connection
    Server myServer(8080, Parser);
    myServer.setup_Socket();
    myServer.handle_Clients();
    return 0;
}
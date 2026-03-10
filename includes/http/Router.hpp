#ifndef ROUTER_HPP
#define ROUTER_HPP

#include <http/Request.hpp>
#include <http/Response.hpp>



class Router
{
    private:
        std::vector<std::string> Methods;
    public:
        Router();
        Response handleRequest(const Request& request);
        Response makeErrorCode(size_t code);
        bool validateMethod(const std::string &method);
        bool validatePath(const std::string &path);
        Response handleeRequest(const std::string request);


};



#endif
#ifndef ROUTER_HPP
#define ROUTER_HPP

#include <http/Request.hpp>
#include <http/Response.hpp>



class Router
{
    private:
        std::vector<std::string> Methods;
        std::string Path;
        std::string Query;
        std::string Method;
        std::string DocumentRoot; //in the future will be at .conf
    public:
        Router();
        std::string getPath();
        Response handleRequest(const Request& request);
        Response makeErrorCode(size_t code);
        bool validateMethod(const std::string &method);
        bool validatePath(const std::string &path);
        void splitPathQuery(const std::string& path);
        std::vector<std::string> splitPath(std::string& path);
        bool buildFinalPath(std::string& path);
        Response handleeRequest(const std::string request);


};



#endif
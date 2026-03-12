#ifndef ROUTER_HPP
#define ROUTER_HPP

#include <http/Request.hpp>
#include <http/Response.hpp>
#include <http/utils/utils.hpp>
#include <http/utils/mime.hpp>
#include <http/cgi/CGI.hpp>

//serve para analisar um Request HTTP e decidir qual recurso devolver, construindo a Response adequada (ficheiro ou erro).

class Router
{
    private:
        CGI cgi;
        std::string Path;
        std::string Query;
        std::string Method;
        std::string DocumentRoot; //in the future will be at .conf
        std::string AbsolutePath;
    public:
        Router();

        std::string getPath() const;
        std::string getQuery() const;
        std::string getMethod() const;
        std::string getAbsolutePath() const;
        std::string getDocumentRoot() const;
        
        bool validateMethod(const std::string &method);
        bool validatePath(const std::string &path);
        
        void splitPathQuery(const std::string& path);
        std::vector<std::string> splitPath(const std::string& path);

        bool buildFinalPath(std::string& path);
        bool buildDocumentRoot(std::string& documentRoot);

        Response makeErrorCode(size_t code);
        Response handleRequest(const Request& request);

        bool isCGI(const std::string& path);

    };



#endif
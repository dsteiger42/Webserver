#ifndef ROUTER_HPP
#define ROUTER_HPP

#include <http/Request.hpp>
#include <http/Response.hpp>
#include <http/utils/utils.hpp>
#include <http/utils/mime.hpp>
#include <http/cgi/CGI.hpp>
#include <config/parser.hpp>

//serve para analisar um Request HTTP e decidir qual recurso devolver, construindo a Response adequada (ficheiro ou erro).

class CGI; //to remove
class Router
{
    private:
        t_MimeTypes MimeTypes;
        std::vector<t_Location> Locations;
        std::string Path;
        std::string Query;
        std::string Method;
        std::string DocumentRoot; //in the future will be at .conf
        std::string AbsolutePath;
    public:
        CGI* cgi;
        Router(const t_parser &parser);
        ~Router();
        std::string getPath() const;
        std::string getQuery() const;
        std::string getMethod() const;
        std::string getAbsolutePath() const;
        std::string getDocumentRoot() const;
    
        //bool validateMethod(const std::string &method);
        bool validatePath(const std::string &path);
        void splitPathQuery(const std::string& path);
        std::vector<std::string> splitPath(const std::string& path);
        bool buildFinalPath(std::string& path);
        bool buildDocumentRoot(std::string& documentRoot);
        Response handleRequest(const Request& request);
        bool isCGI(const std::string& path);
        const t_Location& matchLocation(const std::string& path);

};



#endif
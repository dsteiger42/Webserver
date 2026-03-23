#ifndef ROUTER_HPP
#define ROUTER_HPP

#include <http/Request.hpp>
#include <http/Response.hpp>
#include <http/utils/utils.hpp>
#include <http/utils/mime.hpp>
#include <http/cgi/CGI.hpp>
#include <config/parsing/parser.hpp>
#include <dirent.h>
#include <algorithm>

//serve para analisar um Request HTTP e decidir qual recurso devolver, construindo a Response adequada (ficheiro ou erro).

struct Item
{
    std::string name;
    bool isDir;
    size_t size;
    time_t lastModification;
};

class CGI; //to remove
class Router
{
    private:
        t_parser &Parser;
        std::string Path;
        std::string Query;
        std::string Method;
        std::string DocumentRoot; //in the future will be at .conf
        std::string AbsolutePath;
    public:
        Response makeErrorCode(size_t code);
        CGI* cgi;
        Router(t_parser &parser);
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
        t_Location& matchLocation(const std::string& path);
        Response redirect(int redirectCode, std::string redirectUrl);

};



#endif
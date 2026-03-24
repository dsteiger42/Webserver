#ifndef ROUTER_HPP
#define ROUTER_HPP

#include <http/request/Request.hpp>
#include <http/response/Response.hpp>
#include <utils/utils.hpp>
#include <utils/http/mime.hpp>
#include <http/cgi/CGI.hpp>
#include <config/parser/parser.hpp>
#include <dirent.h>
#include <algorithm>

//serve para analisar um Request HTTP e decidir qual recurso devolver, construindo a Response adequada (ficheiro ou erro).

class CGI; //to remove
class Router
{
    private:
        Parser &_parser;
        std::string _path;
        std::string _query;
        std::string _method;
        std::string _documentRoot; //in the future will be at .conf
        std::string _absolutePath;
    public:
        Response make_ErrorCode(size_t code);
        CGI* cgi;
        Router(Parser &Parser);
        ~Router();
        std::string get_Path() const;
        std::string get_Query() const;
        std::string get_Method() const;
        std::string get_AbsolutePath() const;
        std::string get_DocumentRoot() const;
    
        bool validate_Path(const std::string &path);
        void split_PathQuery(const std::string& path);
        std::vector<std::string> split_Path(const std::string& path);
        bool build_FinalPath(std::string& path);
        bool build_DocumentRoot(std::string& documentRoot);
        Response handle_Request(const Request& request);
        Location& matchLocation(const std::string& path);
        Response redirect(int redirectCode, std::string redirectUrl);

};



#endif
#ifndef ROUTER_HPP
#define ROUTER_HPP

#include <http/Request.hpp>
#include <http/Response.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <http/utils/utils.hpp>
#include <http/utils/mime.hpp>



class Router
{
    private:
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
        
        bool validateMethod(const std::string &method);
        bool validatePath(const std::string &path);
        
        bool isDirectory(const std::string& absolutePath);
        bool isInsideRoot(const std::string& path);
        bool checkFile(const std::string& index);

        void splitPathQuery(const std::string& path);
        std::vector<std::string> splitPath(const std::string& path);

        bool buildFinalPath(std::string& path);
        bool buildDocumentRoot(std::string& documentRoot);

        Response makeErrorCode(size_t code);
        Response handleRequest(const Request& request);

};



#endif
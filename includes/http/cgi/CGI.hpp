#ifndef CGI_HPP
#define CGI_HPP

#include <string>
#include <vector>
#include <sys/types.h>
#include <sys/wait.h>
#include <http/Request.hpp>
#include <http/Response.hpp>
#include <http/Router.hpp>
#include <http/utils/utils.hpp>
#include <http/Router.hpp>

class Router; //to remove
class CGI
{
    public:
        CGI();
        ~CGI();
        void setRouter(Router* r);
        struct CGIResult 
        {
            int status;
            std::string contentType;
            std::string body;
            std::map<std::string,std::string> headers;
        };
        Response execute(const Request& req);

    private:
        std::vector<std::string> args;
        std::vector<std::string> env;
        Router *router; // To remove
        std::string resolveScriptPath(const std::string& path);
        std::vector<char*> buildArguments(const std::string& scriptPath);
        void buildEnvironment(const Request& req, const std::string& scriptPath);
        std::vector<char*> convertEnv(const std::vector<std::string>& env);
        void createPipes(int inPipe[2], int outPipe[2]);
        void executeChildProcess(int inPipe[2], int outPipe[2],
                                 const std::string& scriptPath,
                                 char* const argv[],
                                 char* const envp[]);
        std::string handleParentProcess(int inPipe[2], int outPipe[2],
                                        const Request& req);
        /* std::string readCGIOutput(int fd); */
        Response buildResponseFromCGIOutput(const std::string& output);
        CGIResult parseCGIOutput(const std::string& output);

    };
#endif

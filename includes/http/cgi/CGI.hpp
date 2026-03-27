#ifndef CGI_HPP
#define CGI_HPP

#include <string>
#include <vector>
#include <sys/types.h>
#include <sys/wait.h>
#include <http/request/Request.hpp>
#include <http/response/Response.hpp>
#include <utils/utils.hpp>
#include <map>
#include <fcntl.h>


#define MAX_CGI_OUTPUT 1000000

class Router; // forward declaration

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
        std::vector<std::string> _args;
        std::vector<std::string> env;
        Router *router;
        std::string resolve_ScriptPath(const std::string& path);
        std::vector<char*> build_Arguments(const std::string& scriptPath);
        void build_Environment(const Request& req, const std::string& scriptPath);
        std::vector<char*> convert_Env(const std::vector<std::string>& env);
        void create_Pipes(int inPipe[2], int outPipe[2]);
        void execute_ChildProcess(int inPipe[2], int outPipe[2],
                                 const std::string& scriptPath,
                                 char* const argv[],
                                 char* const envp[]);
        std::string handle_ParentProcess(int inPipe[2], int outPipe[2], pid_t pid, int &status,
                                        const Request& req);
        CGIResult parse_CGIOutput(const std::string& output);
        bool is_ValidCGIOutput(const std::string& output);
};

#endif
#ifndef CGI_HPP
#define CGI_HPP

#include <string>
#include <vector>
#include <http/Request.hpp>
#include <http/Response.hpp>
#include <http/Router.hpp>
#include <http/utils/utils.hpp>

class CGI
{
    public:
        CGI();
        ~CGI();

        Response execute(const Request& req);

    private:
        std::vector<std::string> args;
        Router router; // To remove
        std::string resolveScriptPath(const std::string& path);

        std::vector<std::string> buildEnvironment(const Request& req);
        std::vector<char*> convertEnv(const std::vector<std::string>& env);

        std::vector<char*> buildArguments(const std::string& scriptPath);

        void createPipes(int inPipe[2], int outPipe[2]);

        void executeChildProcess(int inPipe[2], int outPipe[2],
                                 const std::string& scriptPath,
                                 char* const argv[],
                                 char* const envp[]);

        std::string handleParentProcess(int inPipe[2], int outPipe[2],
                                        const Request& req);

        std::string readCGIOutput(int fd);

        Response buildResponseFromCGIOutput(const std::string& output);

    };

#endif

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 01:31:55 by rafael            #+#    #+#             */
/*   Updated: 2026/04/27 04:10:56 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
# define CGI_HPP

# include <string>
# include <vector>
# include <map>
# include <sys/types.h>
# include <sys/wait.h>
# include <fcntl.h>
# include <http/request/Request.hpp>
# include <http/response/Response.hpp>
# include <http/cgi/CgiContext.hpp>
# include <utils/utils.hpp>
# include <config/parser/parser.hpp>

class Router; // forward declaration

class CGI
{
  public:
    CGI();
    ~CGI();
    void setRouter(Router *r);

    struct CGIResult
    {
        int         status;
        std::string contentType;
        std::string body;
        std::map<std::string, std::string> headers;
    };

    int launch(const Request &req, Location &location, CgiContext &ctx);
    Response finish(CgiContext &ctx, int waitStatus);

    CGIResult parse_CGIOutput(const std::string &output);
    bool      is_ValidCGIOutput(const std::string &output);

  private:
    std::vector<std::string>  _args;
    std::vector<std::string>  env;
    Router                   *router;

    std::string             resolve_ScriptPath(const std::string &path);
    std::vector<char *>     build_Arguments(const std::string &scriptPath,
                                            std::string &interpreter);
    void                    build_Environment(const Request &req,
                                              const std::string &scriptPath);
    std::vector<char *>     convert_Env(const std::vector<std::string> &env);
    bool                    create_Pipes(int inPipe[2], int outPipe[2]);
    void                    execute_ChildProcess(int inPipe[2], int outPipe[2],
                                const std::string &scriptPath,
                                const std::map<std::string, std::string> &cgiPath,
                                char *const envp[]);
};

#endif

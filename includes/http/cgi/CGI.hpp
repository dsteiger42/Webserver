/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 01:31:55 by rafael            #+#    #+#             */
/*   Updated: 2026/04/15 19:10:26 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
#include <config/parser/parser.hpp>



#define MAX_CGI_OUTPUT 1000000

class Router; // forward declaration

class CGI
{
  public:
	CGI();
	~CGI();
	void setRouter(Router *r);

	struct	CGIResult
	{
		int	status;
		std::string contentType;
		std::string body;
		std::map<std::string, std::string> headers;
	};

	Response execute(const Request &req, Location &location);

  private:
	std::vector<std::string> _args;
	std::vector<std::string> env;
	Router	*router;
	std::string resolve_ScriptPath(const std::string &path);
	std::vector<char *> build_Arguments(const std::string &scriptPath, std::string &interpreter);
	void build_Environment(const Request &req, const std::string &scriptPath);
	std::vector<char *> convert_Env(const std::vector<std::string> &env);
	bool create_Pipes(int inPipe[2], int outPipe[2]);
	void execute_ChildProcess(int inPipe[2], int outPipe[2],
	const std::string &scriptPath, const std::map<std::string, std::string> &cgiPath, char *const envp[]);
	std::string handle_ParentProcess(int inPipe[2], int outPipe[2], pid_t pid, int &status,
	const Request &req);
	CGIResult parse_CGIOutput(const std::string &output);
	bool is_ValidCGIOutput(const std::string &output);
};

#endif
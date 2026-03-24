/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi_env.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 01:13:49 by rafael            #+#    #+#             */
/*   Updated: 2026/03/24 01:20:41 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <http/cgi/CGI.hpp>

void CGI::build_Environment(const Request &req, const std::string &scriptPath)
{
	int	length;

	env.clear();
	std::stringstream ss;
	length = req.get_Body().size();
	ss << length;
	std::string contentLength = ss.str();
	env.push_back("REQUEST_METHOD=" + req.get_Method());
	env.push_back("QUERY_STRING=" + req.get_Query());
	env.push_back("CONTENT_LENGTH=" + contentLength);
	env.push_back("CONTENT_TYPE=" + req.get_Header("Content-Type"));
	env.push_back("SCRIPT_FILENAME=" + scriptPath);
	env.push_back("SCRIPT_NAME=" + req.get_Path());
	env.push_back("SERVER_PROTOCOL=" + req.get_Version());
	env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	env.push_back("REDIRECT_STATUS=200");

}

std::vector<char *> CGI::convert_Env(const std::vector<std::string> &env)
{
	std::vector<char *> envp;
	for (size_t i = 0; i < env.size(); i++)
		envp.push_back(const_cast<char *>(env[i].c_str()));
	envp.push_back(NULL);
	return (envp);
}
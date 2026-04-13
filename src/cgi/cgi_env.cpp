/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi_env.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 01:13:49 by rafael            #+#    #+#             */
/*   Updated: 2026/04/13 20:03:48 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <http/cgi/CGI.hpp>

static std::string sanitize_Env(const std::string &value)
{
	std::string output;
	for (size_t i = 0; i < value.size(); i++)
	{
		if (value[i] == '\n' || value[i] == '\r' || value[i] == '\0')
			continue;
		output += value[i];
	}
	return output;
}

void CGI::build_Environment(const Request &req, const std::string &scriptPath)
{
	int	length;

	env.clear();
	std::stringstream ss;
	length = req.get_Body().size();
	ss << length;
	std::string contentLength = ss.str();
	env.push_back("REQUEST_METHOD=" + sanitize_Env(req.get_Method()));
	env.push_back("QUERY_STRING=" + sanitize_Env(req.get_Query()));
	env.push_back("CONTENT_LENGTH=" + contentLength);
	env.push_back("CONTENT_TYPE=" + sanitize_Env(req.get_Header("content-type")));
	env.push_back("SCRIPT_FILENAME=" + scriptPath);
	env.push_back("SCRIPT_NAME=" + sanitize_Env (req.get_Path()));
	env.push_back("SERVER_PROTOCOL=" + sanitize_Env(req.get_Version()));
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
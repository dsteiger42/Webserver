/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 01:31:55 by rafael            #+#    #+#             */
/*   Updated: 2026/04/24 03:09:50 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <http/cgi/CGI.hpp>
#include <http/routing/Router.hpp>

CGI::CGI()
{
}

CGI::~CGI()
{
}

void CGI::setRouter(Router *r)
{
	router = r;
}

std::string CGI::resolve_ScriptPath(const std::string &path)
{
	std::string locPath = router->matchLocation(path).path;
	std::string relativePath = path.substr(locPath.size() - 1);
	if (!relativePath.empty() && relativePath[0] == '/')
		relativePath.erase(0, 1);
	std::string fullPath = router->get_DocumentRoot();
	if (!fullPath.empty() && fullPath[fullPath.size() - 1] != '/')
		fullPath += '/';
	return (fullPath + relativePath);
}

std::vector<char *> CGI::build_Arguments(const std::string &scriptPath, std::string &interpreter)
{
	_args.clear();
	_args.push_back(interpreter);
	_args.push_back(scriptPath);
	std::vector<char *> argv;
	argv.push_back(const_cast<char *>(_args[0].c_str()));
	argv.push_back(const_cast<char *>(_args[1].c_str()));
	argv.push_back(NULL);
	return (argv);
}


/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi_parse.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 01:18:09 by rafael            #+#    #+#             */
/*   Updated: 2026/04/20 03:13:08 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <http/cgi/CGI.hpp>

CGI::CGIResult CGI::parse_CGIOutput(const std::string &output)
{
	CGIResult	result;
	size_t		pos;
	size_t		sepLen;
	size_t		colon;

	result.status = 200;
	result.contentType = "text/plain";
	pos = output.find("\r\n\r\n");
	sepLen = 4;
	if (pos == std::string::npos)
	{
		pos = output.find("\n\n");
		sepLen = 2;
	}
	if (pos == std::string::npos)
	{
		result.body = output;
		return (result);
	}
	std::string header = output.substr(0, pos);
	result.body = output.substr(pos + sepLen);
	std::istringstream stream(header);
	std::string line;
	while (std::getline(stream, line))
	{
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);
		if (line.empty())
			break ;
		colon = line.find(':');
		std::string key = line.substr(0, colon);
		std::string value = line.substr(colon + 1);
		while (!value.empty() && (value[0] == ' ' || value[0] == '\t'))
			value.erase(value.begin());
		result.headers[key] = value;
		if (key == "Content-Type")
			result.contentType = value;
		if (key == "Status")
		{
			std::istringstream ss(value);
			ss >> result.status;
		}
		if (key == "Location")
		{
			result.headers[key] = value;
			//se cgi nao enviou o status troca para 302
			if (result.status == 200)
				result.status = 302;
		}
	}
	return (result);
}

bool CGI::is_ValidCGIOutput(const std::string &output)
{
	if (output.empty())
		return false;
	size_t pos = output.find("\r\n\r\n");
	if (pos == std::string::npos)
    {
        pos = output.find("\n\n");
        if (pos == std::string::npos)
            return false;
    }
	std::string headers = output.substr(0, pos);
	transform(headers);
	if (headers.find("content-type:") == std::string::npos) //noralizar letra grande e pequena
		return false;
	return true;
}
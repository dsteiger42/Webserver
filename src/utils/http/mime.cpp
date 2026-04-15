/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mime.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 01:31:55 by rafael            #+#    #+#             */
/*   Updated: 2026/04/15 04:53:59 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <utils/http/mime.hpp>

bool sanitize_Filename(std::string &filename)
{
    std::string sanitized;
    for (size_t i = 0; i < filename.size(); i++)
    {
        char c = filename[i];
        if (std::isalnum(c) || c == '_' || c == '.')
            sanitized += c;
    }
	if (sanitized.empty())
		return true;
    if (sanitized == "." || sanitized == ".." || sanitized[0] == '.')
        return false;
    std::string extension = get_Extension(sanitized);
	static char const *invalid[] = {"php", "py", "sh", "cgi", NULL};
	transform(extension);
	for (size_t i = 0; invalid[i]; i++)
	{
		if (extension == invalid[i])
			return false;
	}
	return true;        
}

std::string get_Extension(std::string file)
{
	int	pos;

	if (file.empty())
		return ("");
	std::string extension;
	pos = -1;
	for (int end = file.size() - 1; end >= 0; end--)
	{
		if (file[end] == '.')
		{
			pos = end;
			break ;
		}
	}
	if (pos != -1)
		extension = file.substr(pos);
	return (extension);
}

std::string get_MimeType(const std::string &extension, const std::map<std::string, std::string> &mimeTypes)
{
	std::map<std::string,
		std::string>::const_iterator it = mimeTypes.find(extension);
	if (it != mimeTypes.end())
		return (it->second);
	return ("application/octet-stream");
}

bool is_acceptableExtension(const std::string &path, Location &location)
{
    std::string extension = get_Extension(path);
	size_t i = 0;
	while (i < location.cgiExt.size())
	{
		if (&location.cgiExt[i][1] && extension ==  location.cgiExt[i].substr(0))
			return true;
		i++;
	}
	return false;	
}
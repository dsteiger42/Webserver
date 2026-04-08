/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mime.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 01:31:55 by rafael            #+#    #+#             */
/*   Updated: 2026/04/08 03:18:10 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <utils/http/mime.hpp>

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
		extension = file.substr(pos + 1);
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
		if (&location.cgiExt[i][1] && extension == &location.cgiExt[i][1])
			return true;
		i++;
	}
	return false;	
}
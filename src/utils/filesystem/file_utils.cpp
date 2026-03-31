/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_utils.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 01:31:55 by rafael            #+#    #+#             */
/*   Updated: 2026/03/24 02:59:09 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <utils/utils.hpp>

bool	read_File(const std::string &path, std::string &outContent)
{
	outContent.clear(); // limpa a string
	std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);
	if (!file.is_open())
	{
		std::cout << "teste1\n";
		return (false);
	}
	file.seekg(0, std::ios::end);                // avanca para o fim
	std::ifstream::pos_type size = file.tellg(); // verifica o size
	if (size < 0)
	{
		std::cout << "teste2\n";
		return (false);
	}
	outContent.resize(size);         // aloca espaco para size bytes
	file.seekg(0, std::ios::beg);    // volta ao inicio
	file.read(&outContent[0], size); // le
	if (!file)
	{
		std::cout << "teste3\n";
		return (false);
	}
	file.close();
	return (true);
}

bool	is_Directory(const std::string &absolutePath)
{
	struct stat	info;

	if (stat(absolutePath.c_str(), &info) != 0)
		return (false);
	if (!S_ISDIR(info.st_mode))
		return (false);
	/* if (access(absolutePath.c_str(), R_OK) != 0)
		return (false); */
	return (true);
}

bool	check_File(const std::string &index)
{
	struct stat	info;

	if (stat(index.c_str(), &info) == 0 && S_ISREG(info.st_mode)
		&& access(index.c_str(), R_OK | F_OK) == 0)
		return (true);
	return (false);
}

bool	is_Executable(const std::string &path)
{
	return (access(path.c_str(), X_OK) == 0);
}

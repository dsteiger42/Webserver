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

#ifndef AUTOINDEX_HPP
# define AUTOINDEX_HPP

# include <algorithm>
# include <dirent.h>
# include <iostream>
# include <sstream>
# include <sys/stat.h>
# include <vector>

struct		Item
{
	std::string name;
	bool	isDir;
	size_t	size;
	time_t	lastModification;
};

bool		generateAutoIndex(std::string &AbsolutePath, std::string &Path, std::string &html);

#endif
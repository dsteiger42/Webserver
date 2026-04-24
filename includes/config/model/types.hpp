/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   types.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 01:31:55 by rafael            #+#    #+#             */
/*   Updated: 2026/04/23 01:32:49 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TYPES_HPP
# define TYPES_HPP

# include <cctype>
# include <climits>
# include <cstdlib>
# include <fstream>
# include <iostream>
# include <map>
# include <sstream>
# include <stdexcept>
# include <string>
# include <vector>

struct		Config
{
	std::string server_name;
	std::string root;
	std::string index;
	size_t	client_max_body_size;
	int		listen;
	Config();
};

struct		ErrorPages
{
	std::map<int, std::string> error_pages;
};

struct		MimeTypes
{
	std::map<std::string, std::string> types;
};

struct		Location
{
	std::string path;
	std::string root;
	std::string upload_store;
	std::vector<std::string> allowedMethods;
	std::vector<std::string> cgiExt;
	std::map<std::string, std::string> cgiPath;
	bool	autoIndex;
	bool	cgiPass;
	bool	hasRedirect;
	bool	has_tryFiles;
	size_t	redirectCode;
	std::string redirectUrl;
	Location();
};

#endif

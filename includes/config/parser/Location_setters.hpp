/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location_setters.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/23 17:33:23 by raamorim          #+#    #+#             */
/*   Updated: 2026/03/24 02:55:29 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_SETTERS_HPP
#define LOCATION_SETTERS_HPP

#include <config/model/types.hpp>

void	set_Autoindex(const std::string &value, Location &location);
void	set_AllowedMethods(const std::vector<std::string> &tokens, size_t &i,
	Location &location);
void	set_CgiPass(const std::string &value, Location &location);
void	set_Root(const std::string &value, Location &location);
void 	set_CgiExt(const std::vector<std::string> &tokens, size_t &i, Location &location);
void	set_Redirection(const std::vector<std::string> &tokens, size_t &i,
		Location &location);
void	set_TryFiles(const std::vector<std::string> &tokens, size_t &i,Location &location);
void	set_Path(const std::vector<std::string> &tokens, size_t &i,
		Location &location);

#endif

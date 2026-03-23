/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location_setters.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: raamorim <raamorim@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/23 17:33:23 by raamorim          #+#    #+#             */
/*   Updated: 2026/03/23 19:26:17 by raamorim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_SETTERS_HPP
#define LOCATION_SETTERS_HPP

#include <config/types.hpp>

void	set_Autoindex(std::string &value, Location &location);
void	set_AllowedMethods(std::vector<std::string> &tokens, size_t &i,
	Location &location);
void	set_cgiPass(std::string &value, Location &location);
void	set_Root(std::string &value, Location &location);
void 	set_cgiExt(std::vector<std::string> &tokens, size_t &i, Location &location);
void	set_redirection(std::vector<std::string> &tokens, size_t &i,
		Location &location);
void	set_tryFiles(std::vector<std::string> &tokens, size_t &i,Location &location);
void	set_Path(std::vector<std::string> &tokens, size_t &i,
		Location &location);

#endif
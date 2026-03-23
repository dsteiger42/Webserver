/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setters.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: raamorim <raamorim@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/23 17:33:23 by raamorim          #+#    #+#             */
/*   Updated: 2026/03/23 17:45:32 by raamorim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SETTERS_HPP
#define SETTERS_HPP

#include <config/types.hpp>

void set_cgiExt(std::vector<std::string> &tokens, size_t &i, t_Location &location);
void	set_tryFiles(std::vector<std::string> &tokens, size_t &i,
		t_Location &location);
void	set_redirection(std::vector<std::string> &tokens, size_t &i,
		t_Location &location);
void	set_cgiPass(std::string &value, t_Location &location);
void	set_Root(std::string &value, t_Location &location);
void	set_AllowedMethods(std::vector<std::string> &tokens, size_t &i,
		t_Location &location);
void	set_Autoindex(std::string &value, t_Location &location);
void	set_Path(std::vector<std::string> &tokens, size_t &i,
		t_Location &location);

#endif
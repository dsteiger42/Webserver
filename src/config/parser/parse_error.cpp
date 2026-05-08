/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_error.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 01:02:42 by rafael            #+#    #+#             */
/*   Updated: 2026/04/29 19:29:56 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <config/parser/parser.hpp>

bool	parse_ErrorPage(const std::vector<std::string> &tokens, size_t &i, ErrorPages &errorPages)
{
	int	code;

	if (i + 2 >= tokens.size())
		return false;
	code = std::atol(tokens[i].c_str());
	if (code > INT_MAX || code < INT_MIN)
		return false;
	i++;
	std::string path = tokens[i];
	errorPages.error_pages[code] = path;
	return true;
}

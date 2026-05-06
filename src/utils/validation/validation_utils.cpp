/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   validation_utils.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 01:32:35 by rafael            #+#    #+#             */
/*   Updated: 2026/05/05 17:56:55 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <utils/utils.hpp>

bool is_Number(const std::string &s)
{
    if (s.empty())
        return false;
    size_t i = 0;
    while (i < s.size() && is_Space(s[i]))
        i++;
    if (i < s.size() && s[i] == '+')
        i++;
    if (i >= s.size())
        return false;
    while (i < s.size())
    {
        if (!std::isdigit(static_cast<unsigned char>(s[i])))
            return false;
        i++;
    }
    return true;
}

bool	is_Space(char c)
{
	return ((c >= 9 && c <= 13) || c == 32);
}

bool	is_ValidMethod(std::vector<std::string> &allowedMethods,
		const std::string &method)
{
	for (size_t i = 0; i < allowedMethods.size(); i++)
	{
		if (allowedMethods[i] == method)
			return (true);
	}
	return (false);
}

bool	validate_Method(const std::string &method)
{
	return (method == "GET" || method == "POST" || method == "DELETE");
}

void	transform(std::string &string)
{
	for (size_t i = 0; i < string.length(); i++)
	{
		if (string[i] >= 65 && string[i] <= 90)
			string[i] += 32;
	}
}

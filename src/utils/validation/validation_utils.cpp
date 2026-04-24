/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   validation_utils.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 01:32:35 by rafael            #+#    #+#             */
/*   Updated: 2026/04/15 02:57:31 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <utils/utils.hpp>

bool	is_Number(const std::string &string)
{
    if (string.empty())
        return false;
    for (size_t i = 0; i < string.size(); i++)
    {
        while(i < string.size() &&  is_Space(string[i]))
            i++;
        if (i < string.size() && string[i] == '+')
            i++;
        if (!std::isdigit(string[i]))
            return false;
    }
    return true;
}

bool	is_Space(char c)
{
	return ((c >= 9 && c <= 13) || c == 32);
}

bool is_ValidMethod(std::vector<std::string> &allowedMethods, const std::string &method)
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
    return method == "GET" || method == "POST" || method == "DELETE";
}

void transform(std::string &string)
{
	for (size_t i = 0; i < string.length(); i++)
	{
		if (string[i] >= 65 && string[i] <= 90)
		  	string[i] += 32;
	}
}


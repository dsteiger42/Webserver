/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   validation_utils.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 01:32:35 by rafael            #+#    #+#             */
/*   Updated: 2026/03/24 02:59:09 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <utils/utils.hpp>

bool is_Number(const std::string &string)
{
    if (string.empty())
        return false;
    for (size_t i = 0; i < string.size(); i++)
    {
        if (string[i] == '+')
            i++;
        if (!std::isdigit(string[i]))
            return false;
    }
    return true;
}

bool is_ValidMethod(std::vector<std::string> &allowedMethods, const std::string &method)
{
    for (size_t i = 0; i < allowedMethods.size(); i++)
    {
        if (allowedMethods[i] == method)
            return true;
    }
    return false;
}

bool validate_Method(const std::string &method)
{
    return method == "GET" || method == "POST" || method == "DELETE";
}
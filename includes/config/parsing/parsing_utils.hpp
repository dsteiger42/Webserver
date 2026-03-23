/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_utils.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: raamorim <raamorim@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/23 17:43:15 by raamorim          #+#    #+#             */
/*   Updated: 2026/03/23 20:19:24 by raamorim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSING_UTILS_HPP
#define PARSING_UTILS_HPP

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

std::vector<std::string> Tokenize(const std::string &filename);
bool countBraces(std::vector<std::string> &tokens);

#endif
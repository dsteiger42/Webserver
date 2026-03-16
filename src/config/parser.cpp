/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dsteiger <dsteiger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 15:17:16 by dsteiger          #+#    #+#             */
/*   Updated: 2026/03/16 15:42:14 by dsteiger         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <config/parser.hpp>

void parse_all(const std::string &filename, t_config &config);

void parse_listen(const std::vector<std::string> &tokens, size_t &i, t_config &config);
void parse_server_name(const std::vector<std::string> &tokens, size_t &i, t_config &config);
void parse_root(const std::vector<std::string> &tokens, size_t &i, t_config &config);
void parse_index(const std::vector<std::string> &tokens, size_t &i, t_config &config);
void parse_client_body_buffer_size(const std::vector<std::string> &tokens, size_t &i, t_config &config);

void parse_server_block(const std::vector<std::string> &tokens, size_t &i, t_config &config);



std::vector<std::string> tokenize(const std::string &filename);
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: raamorim <raamorim@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 15:17:16 by dsteiger          #+#    #+#             */
/*   Updated: 2026/03/16 21:36:06 by raamorim         ###   ########.fr       */
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



void parse_mimeTypes(t_MimeTypes& MimeTypes, size_t &i, std::vector<std::string> tokens)
{
    i++; //anda para {
    if (tokens[i] != "{")
        throw std::runtime_error("Expected '{'");
    i++;
    while(tokens[i] != "}")
    {
        std::string type =  tokens[i];
        std::cout << "type: " << type << std::endl;
        i++;
        size_t prev = i;
        while(tokens[i] != ";")
        {
            std::string value = tokens[i];
            if (i > prev)
                MimeTypes.types[type] += value;
            else
                MimeTypes.types[type] = value;
            i++;
        }
        i++;
    }
    i++;
}
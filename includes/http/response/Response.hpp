/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_utils.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 01:31:55 by rafael            #+#    #+#             */
/*   Updated: 2026/03/24 02:59:09 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <config/model/types.hpp>
# include <cstring>
# include <iostream>
# include <map>
# include <sstream>
# include <string>

class Response
{
  private:
	ErrorPages _errorPages;
	int _statusCode;
	std::string _httpVersion;
	std::string _statusMessage;
	std::string _filePath;
	std::map<std::string, std::string> _headers;
	std::string _body;

  public:
	Response();
	Response(ErrorPages &ErrorPages);
	int get_StatusCode() const;
	const std::string get_Body() const;
	const std::string &get_Header(std::string &key);
	std::string get_StatusMessage(int code) const;
	std::string get_FilePath() const;
	void set_HttpVersion(const std::string &version);
	void set_StatusCode(int code);
	void set_StatusMessage(std::string message);
	void set_Body(std::string body);
	void set_Header(std::string key, std::string value);
	bool has_Header(std::string key) const;
	void prepare_Headers();
	std::string build_StatusLine();
	void build_Header(std::string &result);
	void build_Body(std::string &result);
	std::string serialize();
};

#endif

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 01:31:55 by rafael            #+#    #+#             */
/*   Updated: 2026/04/15 21:52:02 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <config/model/types.hpp>
# include <cstdlib>
# include <cstring>
# include <http/buffer/CircularBuffer.hpp>
# include <iostream>
# include <map>
# include <sstream>
# include <utils/utils.hpp>
# include <vector>

enum	State
{
	READING_HEADER,
	READING_BODY,
	READING_CHUNKED,
	DONE
};


# define MAX_HEADER_SIZE 8192

class Request
{
  private:
	std::string _method;  // GET, POST, DELETE
	std::string _path;    // /index.html
	std::string _version; // HTTP/1.1
	std::string _body;
	std::string _query;
	std::map<std::string, std::string> _headers;
	State _state;
	size_t _contentLength;
	size_t _statusCode;
	size_t _maxBodySize;
	bool _validRequest;
	CircularBuffer _buffer;
	
  public:
	void set_MaxBodySize(size_t max);
 	Request();
	const std::string &get_Method() const;
	const std::string &get_Path() const;
	const std::string &get_Version() const;
	const std::string &get_Body() const;
	const std::string &get_Header(const std::string &key) const;
	const std::string get_Query() const;
	size_t get_statusCode() const;
	bool get_validRequest() const;
	bool is_Done() const;
	void reset();
	void split_PathQuery(const std::string &path);
	std::string extract_HeaderFromBuffer(size_t size);
	void validate_Request();
	bool process_Header();
	bool process_Body();
	bool process_Chunked();
	bool consume_CRLF();
	void appendToBody(size_t size);
	void determine_NextState();
	void parse_RequestLine(std::string &line, std::istringstream &split);
	void parse_Headers(std::string &line, std::istringstream &split);
	void parse_Header(const std::string &headerStr);
	void advanceParsing();
	void fill_Buffer(const std::string &request, size_t pos);
    std::string get_Leftover();
};

#endif

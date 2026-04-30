/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 01:31:55 by rafael            #+#    #+#             */
/*   Updated: 2026/04/30 00:53:42 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <http/response/Response.hpp>

Response::Response() : _errorPages(), _statusCode(200),
	_httpVersion("HTTP/1.1"), _statusMessage("OK"), _filePath("")
{
}

Response::Response(ErrorPages &ErrorPages) : _errorPages(ErrorPages),
	_statusCode(200), _httpVersion("HTTP/1.1"), _statusMessage("OK"),
	_filePath("")
{
}

int Response::get_StatusCode() const
{
	return (this->_statusCode);
}

std::string Response::get_StatusMessage(int code) const
{
	switch (code)
	{
	case 200:
		return ("OK");
	case 201:
		return ("Created");
	case 204:
		return ("No Content");
	case 301:
		return ("Moved Permanently");
	case 302:
		return ("Found");
	case 400:
		return ("Bad Request");
	case 403:
		return ("Forbidden");
	case 404:
		return ("Not Found");
	case 405:
		return ("Method Not Allowed");
	case 408:
		return ("Request Timeout");
	case 409:
		return ("Conflict");
	case 413:
		return ("Payload Too Large");
	case 431:
		return ("Request Header Fields Too Large");
	case 500:
		return ("Internal Server Error");
	case 501:
		return ("Not Implemented");
	case 502:
		return ("Bad Gateway");
	case 503:
		return ("Service Unavailable");
	case 504:
		return "Gateway Time-out";
	case 505:
		return "HTTP Version Not Supported";
	default:
		return "Unknown";
	}
}

std::string Response::get_FilePath() const
{
	return (this->_filePath);
}

const std::string Response::get_Body() const
{
	return (this->_body);
}

const std::string &Response::get_Header(std::string &key)
{
	static const std::string empty = "";
	if (_headers.find(key) != _headers.end())
		return (_headers[key]);
	return (empty);
}

void Response::set_StatusCode(int code)
{
	this->_statusCode = code;
	this->_statusMessage = get_StatusMessage(code);
	std::map<int,
		std::string>::const_iterator it = _errorPages.error_pages.find(code);
	if (it != _errorPages.error_pages.end())
		this->_filePath = it->second;
	else
		this->_filePath = "Unknown";
}

void Response::set_HttpVersion(const std::string &version)
{
	this->_httpVersion = version;
}

void Response::set_StatusMessage(std::string message)
{
	this->_statusMessage = message;
}
void Response::set_Body(std::string body)
{
	this->_body = body;
}
void Response::set_Header(std::string key, std::string value)
{
	this->_headers[key] = value;
}

bool Response::has_Header(std::string key) const
{
	if (_headers.find(key) != _headers.end())
		return true;
	return false;
}

void Response::prepare_Headers()
{
	size_t	size;

	if (!has_Header("Content-Type"))
		set_Header("Content-Type", "application/octet-stream");
	if (!has_Header("Content-Length"))
	{
		size = this->_body.length();
		std::stringstream ss;
		ss << size;
		std::string length = ss.str();
		set_Header("Content-Length", length);
	}
	if (!has_Header("Connection"))
		set_Header("Connection", "close");
}

std::string Response::build_StatusLine()
{
	std::stringstream ss;
	ss << this->_httpVersion << " " << this->_statusCode << " " << this->_statusMessage << "\r\n";
	return ss.str();
}

void Response::build_Header(std::string &result)
{
	for (std::map<std::string,
		std::string>::iterator it = _headers.begin(); it != _headers.end(); it++)
	{
		result += it->first + ": " + it->second + "\r\n";
	}
	result += "\r\n";
}

void Response::build_Body(std::string &result)
{
	result += this->_body;
}

std::string Response::serialize()
{
	prepare_Headers();
	std::string response;
	response = build_StatusLine();
	build_Header(response);
	build_Body(response);
	return (response);
}

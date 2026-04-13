/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 01:31:55 by rafael            #+#    #+#             */
/*   Updated: 2026/04/10 19:27:39 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <http/request/Request.hpp>

Request::Request() : _buffer(MAX_HEADER_SIZE + MAX_BODY_SIZE), _state(READING_HEADER),
	_contentLength(0), _statusCode(0), _validRequest(false)
{
}

const std::string &Request::get_Method() const
{
	return (this->_method);
}

const std::string &Request::get_Path() const
{
	return (this->_path);
}

const std::string &Request::get_Version() const
{
	return (this->_version);
}

const std::string &Request::get_Body() const
{
	return (this->_body);
}

const std::string get_EmptyString()
{
	static std::string empty;
	return (empty);
}

const std::string &Request::get_Header(const std::string &key) const
{
	static std::string empty;
	std::map<std::string, std::string>::const_iterator it = _headers.find(key);
	if (it != _headers.end())
		return (it->second);
	return (empty);
}

const std::string Request::get_Query() const
{
	return (this->_query);
}

size_t Request::get_statusCode() const
{
	return (this->_statusCode);
}

bool Request::get_validRequest() const
{
	return (this->_validRequest);
}

void Request::reset()
{
	_method.clear();
	_path.clear();
	_version.clear();
	_body.clear();
	_query.clear();
	_headers.clear();
	_contentLength = 0;
	_statusCode = 0;
	_validRequest = false;
	_state = READING_HEADER;
}

bool Request::is_Done() const
{
	return (this->_state == DONE);
}

void Request::fill_Buffer(const std::string &request, size_t len)
{
	size_t	written;
	size_t	bytesWritten;

	written = 0;
	while (written < len)
	{
		if (_state == READING_HEADER && _buffer.get_Size() + (len
				- written) > MAX_HEADER_SIZE)
		{
			_statusCode = 431;
			_validRequest = false;
			return ;
		}
		bytesWritten = _buffer.write(request.data() + written, len - written);
		if (bytesWritten == 0)
		{
			advanceParsing();
			if (_buffer.is_Full())
			{
				break ;
			}
			continue ;
		}
		written += bytesWritten;
		advanceParsing();
	}
}

void Request::validate_Request()
{
	if (_version == "" || _headers.find("host") == _headers.end())
	{
		_statusCode = 400;
		_validRequest = false;
	}
	if (_headers.find("content-length") != _headers.end()
		&& _headers.find("transfer-encoding") != _headers.end())
	{
		_statusCode = 400;
		_validRequest = false;
	}
	if (_headers.find("transfer-encoding") != _headers.end())
	{
		_statusCode = 501;
		_validRequest = false;
	}
}

std::string Request::extract_HeaderFromBuffer(size_t size)
{
	std::string header(size, '\0');
	_buffer.peek(reinterpret_cast<char *>(&header[0]), size);
	_buffer.consume(size);
	return (header);
}

void Request::determine_NextState()
{
	long	length;

	std::map<std::string,
		std::string>::iterator it = _headers.find("content-length");
	if (it != _headers.end())
	{
		length = std::atol(it->second.c_str());
		if (length > MAX_BODY_SIZE)
		{
			_statusCode = 413;
			_validRequest = false;
			return ;
		}
		_contentLength = length;
		if (_contentLength > 0)
		{
			_state = READING_BODY;
			return ;
		}
	}
	_state = DONE;
}

bool Request::process_Header()
{
	size_t	pos;
	size_t	headersize;

	pos = _buffer.find("\r\n\r\n");
	if (pos == std::string::npos)
		return (false);
	headersize = pos + 4;
	if (_buffer.get_Size() < headersize)
		return (false);
	std::string header = extract_HeaderFromBuffer(headersize);
	parse_Header(header);
	validate_Request();
	determine_NextState();
	return (true);
}

bool Request::process_Body()
{
    size_t  remaining;
    size_t  available;
    size_t  toRead;
    size_t  oldSize;

    remaining = _contentLength - _body.size();
    if (remaining == 0)
    {
        _state = DONE;
        return (true);
    }
    available = _buffer.get_Size();
    /*
    ** Lê exactamente min(remaining, available) bytes.
    ** Se available > remaining, os bytes extra pertencem ao próximo
    ** request (keep-alive) ou são ruído do pipe — ignoramos, não
    ** rejeitamos. O RFC HTTP manda ler Content-Length bytes e parar.
    ** Se available < remaining, ainda estamos a espera de mais dados
    ** — retornamos false para esperar o próximo recv().
    */
    toRead = std::min(remaining, available);
    if (toRead == 0)
        return (false);
    oldSize = _body.size();
    _body.resize(oldSize + toRead);
    _buffer.read(&_body[oldSize], toRead);
    if (_body.size() == _contentLength)
        _state = DONE;
    return (true);
}

void Request::parse_RequestLine(std::string &line, std::istringstream &split)
{
	if (std::getline(split, line))
	{
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);
		std::istringstream rl(line);
		if (!(rl >> _method >> _path >> _version))
		{
			_statusCode = 400;
			_validRequest = false;
			return ;
		}
		_validRequest = true;
		split_PathQuery(_path);
	}
}

static bool	is_UniqueHeader(const std::string &key)
{
	return (key == "content-length" || key == "host"
		|| key == "transfer-encoding");
}

static bool is_validHeader(const std::string &key)
{
	for (size_t i = 0; i < key.size(); i++)
	{
		if (!std::isalnum(key[i]) && key[i] != '-')
			return false;
	}
	return true;	
}

void Request::parse_Headers(std::string &line, std::istringstream &split)
{
	size_t	pos;

	while (std::getline(split, line))
	{
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);
		if (line.empty())
			break ;
		if (line.size() > MAX_HEADER_SIZE)
		{
			_validRequest = false;
			return ;
		}
		pos = line.find(':');
		if (pos == std::string::npos)
		{
			_statusCode = 400;
			_validRequest = false;
			return ;
		}
		std::string key = line.substr(0, pos);
		transform(key);
		if (!is_validHeader(key))
		{
			_validRequest = false;
			_statusCode = 400;
			return ;
		}
		std::string value = line.substr(pos + 1);
		if (key == "content-length" && !is_Number(value))
		{
			_validRequest = false;
			_statusCode = 400;
			return ;
		}
		while (!value.empty() && (value[0] == ' ' || value[0] == '\t'))
			value.erase(value.begin());
		if (_headers.count(key) && is_UniqueHeader(key))
		{
			_statusCode = 400;
			_validRequest = false;
			return ;
		}
		while (!value.empty() && (is_Space(value[0])))
			value.erase(value.begin());
		_headers[key] = value;
	}
}

static bool has_BareLF(const std::string &header)
{
    for (size_t i = 0; i < header.size(); i++)
    {
        if (header[i] == '\n' && (i == 0 || header[i - 1] != '\r'))
            return (true);
    }
    return (false);
}

void Request::parse_Header(const std::string &headerStr)
{
	if (has_BareLF(headerStr))
	{
        _statusCode = 400;
        _validRequest = false;
        return ;
    }
	std::istringstream split(headerStr);
	std::string line;
	parse_RequestLine(line, split);
	parse_Headers(line, split);

}

void Request::advanceParsing()
{
	bool	progressed;

	progressed = true;
	while (progressed)
	{
		progressed = false;
		if (_state == READING_HEADER)
			progressed = process_Header();
		else if (_state == READING_BODY)
			progressed = process_Body();
		else if (_state == DONE)
			return ;
	}
}

void Request::split_PathQuery(const std::string &path)
{
	size_t	pos;

	pos = path.find("?");
	if (pos != std::string::npos && pos + 1 <= path.size())
	{
		std::string path1;
		path1 = path.substr(0, pos);
		_query = path.substr(pos + 1);
		_path = path1;
	}
	else
	{
		_path = path;
		_query = "";
	}
}
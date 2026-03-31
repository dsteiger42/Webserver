#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <cstring>
#include <http/buffer/CircularBuffer.hpp>
#include <config/model/types.hpp>
#include <utils/utils.hpp>

# include <iostream>
# include <map>
# include <sstream>
# include <vector>
#include <fstream>
#include <cstdlib>

enum	State
{
	READING_HEADER,
	READING_BODY,
	DONE
};

#define MAX_BODY_SIZE 4096
#define MAX_HEADER_SIZE 8192

class Request
{
  private:
  	CircularBuffer	_buffer;
	std::string _method;  // GET, POST, DELETE
	std::string _path;    // /index.html
	std::string _version; // HTTP/1.1
	std::string _body;
	std::string _query;
	std::map<std::string, std::string> _headers;
    State _state;
    size_t _contentLength;
	size_t _statusCode;
	bool _validRequest;
  public:
	Request();
	const std::string& get_Method() const;
	const std::string& get_Path() const;
	const std::string& get_Version() const;
	const std::string& get_Body() const;
	const std::string& get_Header(const std::string &key) const;
    const std::string get_Query() const;
	size_t get_statusCode() const;
	bool get_validRequest() const;
	bool is_Done() const;
	
	void reset();
	
	void split_PathQuery(const std::string& path);
	std::string extract_HeaderFromBuffer(size_t size);
	void validate_Request();
	
	bool process_Header();
	bool process_Body();
	
	void determine_NextState();
	
	void parse_RequestLine(std::string &line, std::istringstream &split);
	void parse_Headers(std::string &line, std::istringstream &split);
    void parse_Header(const std::string &headerStr);
    void advanceParsing();
	void parse(const std::string &request);
	
	void fill_Buffer(const std::string &request, size_t pos);
};

#endif

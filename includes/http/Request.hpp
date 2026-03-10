#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <cstring>
# include <http/CircularBuffer.hpp>
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

class Request
{
  private:
  	CircularBuffer	Buffer;
	std::string Method;  // GET, POST, DELETE
	std::string Path;    // /index.html
	std::string Version; // HTTP/1.1
	std::string Body;
	std::map<std::string, std::string> Headers;
    State state;
    size_t contentLength;

  public:
	Request();
	const std::string getMethod() const;
	const std::string getPath() const;
	const std::string getVersion() const;
	const std::string getBody() const;
	const std::string getHeader(const std::string &key) const;
    bool isDone() const;
	void reset();
	void parseRequestLine(std::string &line, std::istringstream &split);
	void parseHeaders(std::string &line, std::istringstream &split);
    void parseHeader(const std::string &headerStr);
    void advanceParsing();
	void parse(const std::string &request);
	std::string extractHeaderFromBuffer(size_t size);
	void validateRequest();
	bool processHeader();
	bool processBody();
	void determineNextState();
	void fillBuffer(const std::string &request, size_t pos);
};

#endif

/* GET /index.html HTTP/1.1\r\n
Host: localhost\r\n
Content-Length: 5\r\n
\r\n
Hello */
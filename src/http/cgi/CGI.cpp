#include <http/cgi/CGI.hpp>
CGI::CGI() {}

CGI::~CGI() {}

Response CGI::execute(const Request& req)
{
    (void)req;
    Response res;
    res.setStatusCode(200);
    res.setHeader("Content-Type", "text/plain");    
    res.setBody("CGI OK - execute() foi chamado!");
    return res;
}
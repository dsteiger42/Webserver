#include <http/Router.hpp>

Router::Router() 
{
    Methods.push_back("GET");
    Methods.push_back("POST");
    Methods.push_back("DELETE");
}

Response Router::makeErrorCode(size_t code)
{
    Response res;
    res.setStatusCode(code);
    std::stringstream ss;
    ss << "<h1>" << code << " " << res.getStatusMessage() << "<h1>";
    res.setBody(ss.str());
    return res;
}

Response Router::handleRequest(const Request& request)
{
    Response response;
    if (!validateMethod(request.getMethod()))
        return makeErrorCode(405);
    if (request.getPath().find("..") != std::string::npos)
        return makeErrorCode(403);
    if (!validatePath(request.getPath()))
        return makeErrorCode(400);

    return response;
}

bool Router::validateMethod(const std::string &method)
{
    for (size_t i = 0; i < Methods.size(); i++)
    {
        if (method == Methods[i])
            return true;
    }
    return false;
}

bool Router::validatePath(const std::string &path)
{
    if (path.empty())
        return false; 
    if (path[0] != '/')
        return false; 
    if (path.find("'\'") != std::string::npos || path.find("\0") != std::string::npos || path.find(":") != std::string::npos || path.find("*") != std::string::npos)
        return false; 
    return true;
}

Response Router::handleeRequest(const std::string request)
{
    Response response;
    if (!validateMethod(request))
        return makeErrorCode(405);
    if (request.find("..") != std::string::npos)
        return makeErrorCode(403);
    if (!validatePath(request))
        return makeErrorCode(400);
    return response;
}


void testRequest(const std::string &raw)
{
    Router router;
    Request req;

    req.parse(raw);

    Response res = router.handleRequest(req);

    std::cout << "RAW REQUEST:\n" << raw << std::endl;
    std::cout << "Method: " << req.getMethod() << std::endl;
    std::cout << "Path: " << req.getPath() << std::endl;
    std::cout << "Status code: " << res.getStatusCode() << std::endl;
    std::cout << "Body: " << res.getBody() << std::endl;
    std::cout << "--------------------------" << std::endl;
}

int main()
{
    // método inválido
    testRequest(
        "POSTT /index.html HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "\r\n"
    );

    // path com directory traversal
    testRequest(
        "GET /../secret.txt HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "\r\n"
    );

    // path inválido
    testRequest(
        "GET index.html HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "\r\n"
    );

    // request válida
    testRequest(
        "GET /index.html HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "\r\n"
    );

    return 0;
}
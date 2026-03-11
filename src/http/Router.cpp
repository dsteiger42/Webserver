#include <http/Router.hpp>

Router::Router() 
{    
    Methods.push_back("GET");
    Methods.push_back("POST");
    Methods.push_back("DELETE");
    Path = "";
    Query = "";
    Method = "";
    DocumentRoot = "./www";
}

std::string Router::getPath()
{
    return (this->Path);
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
Response Router::makeErrorCode(size_t code)
{
    Response res;
    res.setStatusCode(code);
    std::stringstream ss;
    ss << "<h1>" << code << " " << res.getStatusMessage() << "<h1>";
    res.setBody(ss.str());
    return res;
}

bool Router::validatePath(const std::string &path)
{
    if (path.empty())
        return false; 
    if (path[0] != '/')
        return false; 
    if (path.find('\\') != std::string::npos || path.find('\0') != std::string::npos || path.find(':') != std::string::npos || path.find('*') != std::string::npos)
        return false; 
    return true;
}

void Router::splitPathQuery(const std::string& path)
{
    size_t pos = path.find("?");
    if (pos != std::string::npos)
    {
        Path = path.substr(0, pos);
        Query = path.substr(pos + 1);
    }
    else
        Path = path;    
}

Response Router::handleRequest(const Request& request)
{
    Response response;
    if (!validateMethod(request.getMethod()))
        return makeErrorCode(405);
    splitPathQuery(request.getPath());
    if (!validatePath(Path))
        return makeErrorCode(400);
    if (!buildFinalPath(Path))
        return makeErrorCode(403);
    return response;
}

std::vector<std::string> Router::splitPath(std::string& path)
{
    std::vector<std::string> tokens;
    size_t start = 0;
    while (start < path.size())
    {
        size_t pos = path.find('/', start);
        if (pos == std::string::npos)
            pos = path.size();
        if (pos > start)
            tokens.push_back(path.substr(start, pos - start));
        start = pos + 1;
    }
    return tokens;
}

bool Router::buildFinalPath(std::string& path)
{
    std::vector<std::string> tokens = splitPath(path);
    std::vector<std::string> final;
    for (size_t i = 0; i < tokens.size(); i++)
    {
        if (tokens[i] == "..")
        {
            if (final.size() == 0)
                return false;
            else
                final.pop_back();
        }
        else if (tokens[i] == "" || tokens[i] == ".")
            continue;
        else
            final.push_back(tokens[i]);
    }
    this->Path = "";
    for (size_t i = 0; i < final.size(); i++)
    {
        this->Path += final[i];
        std::cout << "Path: " << Path << std::endl;
        if (i + 1 < final.size())
            this->Path += "/";
    }
    return true;
}


void testRequest(const std::string &raw)
{
    Router router;
    Request req;

    req.parse(raw);
    Response res = router.handleRequest(req);

    std::cout << "RAW REQUEST:\n" << raw << std::endl;
    std::cout << "Method: " << req.getMethod() << std::endl;
    std::cout << "Original Path: " << req.getPath() << std::endl;
    std::cout << "Normalized Path: " << router.getPath() << std::endl;
    std::cout << "Status code: " << res.getStatusCode() << std::endl;
    std::cout << "Body: " << res.getBody() << std::endl;
    std::cout << "--------------------------" << std::endl;
}

int main()
{
    /* // método inválido
    testRequest(
        "POSTT /index.html HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "\r\n"
    );

    // directory traversal
    testRequest(
        "GET /../secret.txt HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "\r\n"
    );

    // path inválido (sem /)
    testRequest(
        "GET index.html HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "\r\n"
    );

    // request válida simples
    testRequest(
        "GET /index.html HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "\r\n"
    );

    // query string
    testRequest(
        "GET /search?q=webserv&lang=pt HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "\r\n"
    );

    // path com múltiplos /
    testRequest(
        "GET //images///logo.png HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "\r\n"
    );

    // path com .
    testRequest(
        "GET /images/./logo.png HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "\r\n"
    );

    // path com ..
    testRequest(
        "GET /images/../index.html HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "\r\n"
    ); */

    // path complexo (normalização real)
    testRequest(
        "GET /a/b/../c/./d//file.txt HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "\r\n"
    );

    return 0;
}
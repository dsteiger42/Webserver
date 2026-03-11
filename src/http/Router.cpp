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
    AbsolutePath = "";
}

std::string Router::getPath() const
{
    return (this->Path);
}

std::string Router::getQuery() const
{
    return this->Query;
}
std::string Router::getMethod() const
{
    return this->Method;
}
std::string Router::getAbsolutePath() const
{
    return this->AbsolutePath;
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
    AbsolutePath = DocumentRoot + Path;
    if (AbsolutePath.compare(0, DocumentRoot.size(), DocumentRoot) != 0)
        return makeErrorCode(403);
    if (isDirectory(AbsolutePath))
    {
        std::string index = AbsolutePath + "/index.html";
        if (checkFile(index))
        {
            AbsolutePath = index;
            Path += "/index.html";
        }
        else
            return makeErrorCode(403);
    }
    if (!checkFile(AbsolutePath)) //if it's not a directory but the file doens't exist
        return makeErrorCode(404);
    return response;
}

bool Router::checkFile(const std::string& index)
{
    struct stat info;
    if (stat(index.c_str(), &info) == 0 && S_ISREG(info.st_mode) && access(index.c_str(), R_OK | F_OK) == 0)
        return true;
    return false;
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
    this->Path = "/";
    for (size_t i = 0; i < final.size(); i++)
    {
        this->Path += final[i];
        if (i + 1 < final.size())
            this->Path += "/";
    }
    return true;
}

bool Router::isDirectory(const std::string& absolutePath)
{
    struct stat info;

    if (stat(absolutePath.c_str(), &info) != 0)
        return false;
    if (!S_ISDIR(info.st_mode))
        return false;
    if (access(absolutePath.c_str(), R_OK) != 0)
        return false;
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

#include <http/Router.hpp>

int main()
{
    // router e request de teste
    Router router;

    // 1️⃣ Path com múltiplos /
    testRequest(
        "GET //images///logo.png HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "\r\n"
    );

    // 2️⃣ Path com . (current dir)
    testRequest(
        "GET /images/./logo.png HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "\r\n"
    );

    // 3️⃣ Path com .. (parent dir)
    testRequest(
        "GET /images/../index.html HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "\r\n"
    );

    // 4️⃣ Path complexo (normalização real)
    testRequest(
        "GET /a/b/../c/./d//file.txt HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "\r\n"
    );

    // 5️⃣ Query string
    testRequest(
        "GET /search?q=webserv&lang=pt HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "\r\n"
    );

    // 6️⃣ Diretório existente com index.html
    testRequest(
        "GET / HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "\r\n"
    );

    // 7️⃣ Diretório sem index.html (deve retornar 403)
    testRequest(
        "GET /emptydir HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "\r\n"
    );

    // 8️⃣ Path malformado (sem / inicial)
    testRequest(
        "GET index.html HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "\r\n"
    );

    // 9️⃣ Directory traversal (deve ser bloqueado)
    testRequest(
        "GET /../secret.txt HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "\r\n"
    );

    return 0;
}
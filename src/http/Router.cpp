#include <http/Router.hpp>

Router::Router() 
{   
    Path = "";
    Query = "";
    Method = "";
    DocumentRoot = "./www/";
    AbsolutePath = "";
    cgi = new CGI();
    cgi->setRouter(this);
}
Router::~Router()
{
    delete cgi;
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

std::string Router::getDocumentRoot() const
{
    return this->DocumentRoot;
}

bool Router::validateMethod(const std::string &method)
{
    return method == "GET" || method == "POST" || method == "DELETE";
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

std::vector<std::string> Router::splitPath(const std::string& path)
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
    this->Path = "/";
    for (size_t i = 0; i < final.size(); i++)
    {
        this->Path += final[i];
        if (i + 1 < final.size())
            this->Path += "/";
    }
    return true;
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
    {
        std::cout << "buildfinalpath" << std::endl;
        return makeErrorCode(403);
    }
    AbsolutePath = DocumentRoot + Path;
    if (!isInsideRoot(AbsolutePath))
    {
        std::cout << "insideroot" << std::endl;
        return makeErrorCode(403);
    }
    if (isCGI(request.getPath()))
        return cgi->execute(request);
    if (isDirectory(AbsolutePath))
    {
        std::string index = AbsolutePath + "/index.html";
        std::cout << "Trying index: " << index << std::endl;
        if (checkFile(index))
        {
            AbsolutePath = index;
            if (Path == "/")
                Path = "/index.html";
            else
                Path += "/index.html";
        }
        else
        {
            std::cout << "else" << std::endl;
            return makeErrorCode(403);
        }
    }
    if (!checkFile(AbsolutePath)) //if it's not a directory but the file doens't exist
        return makeErrorCode(404);
    std::string content;
    if (!readFile(AbsolutePath, content))
        return makeErrorCode(500);
    response.setBody(content);
    std::string MimeType = getMimeType(getExtension(AbsolutePath));
    response.setHeader("Content-Type", MimeType);
    return response;
}

bool Router::isCGI(const std::string& path)
{
    if (path.empty())
        return false;
    //temporario, mais tarde com base no web.conf
    if (path.compare(0, 9, "/cgi-bin/") == 0)
        return true;
    return false;
}

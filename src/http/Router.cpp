#include <http/Router.hpp>

Router::Router() 
{    
    Path = "";
    Query = "";
    Method = "";
    DocumentRoot = "./www/";
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

bool Router::isInsideRoot(const std::string& path)
{
    if (path.compare(0, DocumentRoot.size(), DocumentRoot) != 0)
        return false;

    if (path.size() > DocumentRoot.size() &&
        path[DocumentRoot.size()] != '/')
        return false;

    return true;
}
bool Router::checkFile(const std::string& index)
{
    struct stat info;
    if (stat(index.c_str(), &info) == 0 && S_ISREG(info.st_mode) && access(index.c_str(), R_OK | F_OK) == 0)
        return true;
    return false;
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

Response Router::makeErrorCode(size_t code)
{
    Response res;
    res.setStatusCode(code);
    std::stringstream ss;
    ss << "<h1>" << code << " " << res.getStatusMessage() << "</h1>";    res.setBody(ss.str());
    return res;
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
    if (!isInsideRoot(AbsolutePath))
         return makeErrorCode(403);
    //if (isCGI(request.getPath()))
        //return TODO
    if (isDirectory(AbsolutePath))
    {
        std::string index = AbsolutePath + "/index.html";
        if (checkFile(index))
        {
            AbsolutePath = index;
            if (Path == "/")
                Path = "/index.html";
            else
                Path += "/index.html";
        }
        else
            return makeErrorCode(403);
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
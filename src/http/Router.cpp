#include <http/Router.hpp>

Router::Router(const t_parser& parser) 
{   
    MimeTypes = parser.MimeTypes;
    Locations = parser.Location;
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

Response Router::redirect(int redirectCode, std::string redirectUrl)
{
    Response response;
    response.setStatusCode(redirectCode);
    response.setHeader("Location", redirectUrl);
    response.setBody("Redirecting");
    response.setHeader("Content-Type", "text/plain");
    response.setHeader("Content-Length", "11");
    return response;
}

// std::string generateAutoIndex(std::string &AbsolutePath, std::string &Path)
// {
//     std::string html;
//     DIR *dir = opendir(AbsolutePath.c_str());
//     if (!dir)
//     {
//         //500
//         std::cout << "Couldn't opendir" << std::endl;
//         return NULL;
//     }
// /*     1. abrir diretoria
//     2. para cada entry:
//         - ignorar "." e ".."
//         - construir full path
//         - fazer stat
//         - identificar tipo (dir/file)
//         - extrair tamanho
//         - extrair data
//         - guardar tudo num vector

//     3. ordenar vector

//     4. gerar HTML:
//         - header
//         - tabela
//         - loop no vector
//         - footer

//     5. return HTML */
// }


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
    t_Location& loc = matchLocation(Path);
    if (loc.hasRedirect)
        return redirect(loc.redirectCode, loc.redirectUrl);
    if (!isValidMethod(loc.allowedMethods, request.getMethod()))
        return makeErrorCode(405);
    //setar DocRoot para loc root ou server root caso nao haja loc root
    if (!loc.root.empty())
        DocumentRoot = loc.root;
    else
        DocumentRoot = "./www"; // trocar server.root;
    if (loc.cgiPass)    
        return (cgi->execute(request));
    AbsolutePath = DocumentRoot + Path;
    if (!isInsideRoot(AbsolutePath, DocumentRoot))
       return makeErrorCode(403);
    if (isDirectory(AbsolutePath))
    {
        //se tiver autoindex on retorna um generateautoindex(absolutepath)
        //Para listar diretórios quando autoindex = on.
        std::string index = AbsolutePath + "/index.html";
        if (checkFile(index))
            AbsolutePath = index;
        else
            return makeErrorCode(403);
    }
    //try files
    if (!checkFile(AbsolutePath)) //if it's not a directory but the file doens't exist
        return makeErrorCode(404);
    std::string content;
    if (!readFile(AbsolutePath, content))
        return makeErrorCode(500);
    response.setBody(content);
    std::string MimeType = getMimeType(getExtension(AbsolutePath), MimeTypes.types);
    response.setHeader("Content-Type", MimeType);
    return response;
}


t_Location& Router::matchLocation(const std::string &path)
{
    t_Location* bestMatch = NULL;
    size_t bestLength = 0;

    for (size_t i = 0; i < Locations.size(); i++)
    {
        t_Location& loc = Locations[i];
        if (path.compare(0, loc.path.size(), loc.path) == 0)
        {
            if (loc.path.size() > bestLength)
            {
                bestLength = loc.path.size();
                bestMatch = &loc;
            }
        }
    }
    if (!bestMatch)
    {
        for (size_t i = 0; i < Locations.size(); i++)
        {
            if (Locations[i].path == "/")
                return Locations[i];
        }
        return Locations[0];
    }
    return *bestMatch;
}
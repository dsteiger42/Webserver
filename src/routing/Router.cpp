#include <http/routing/Router.hpp>
#include <http/routing/autoindex.hpp>

Router::Router(ServerConfig &sc) : _config(sc)
{   
    _path = "";
    _query = "";
    _method = "";
    _documentRoot = sc.config.root; //"./www/"
    _absolutePath = "";
    cgi.setRouter(this);
}
Router::~Router()
{
}

Router::Router(const Router& other)
    : _config(other._config),
      _path(other._path),
      _query(other._query),
      _method(other._method),
      _documentRoot(other._documentRoot),
      _absolutePath(other._absolutePath),
      cgi(other.cgi)
{
    cgi.setRouter(this);
}

Router& Router::operator=(const Router& other)
{
    if (this != &other)
    {
        _config = other._config;
        _path = other._path;
        _query = other._query;
        _method = other._method;
        _documentRoot = other._documentRoot;
        _absolutePath = other._absolutePath;
        cgi = other.cgi;
        cgi.setRouter(this);
    }
    return *this;
}
std::string Router::get_Path() const
{
    return (this->_path);
}

std::string Router::get_Query() const
{
    return (this->_query);
}
std::string Router::get_Method() const
{
    return (this->_method);
}
std::string Router::get_AbsolutePath() const
{
    return (this->_absolutePath);
}

std::string Router::get_DocumentRoot() const
{
    return (this->_documentRoot);
}

bool Router::validate_Path(const std::string &path)
{
    if (path.empty())
        return false; 
    if (path[0] != '/')
        return false; 
    if (path.find('\\') != std::string::npos || path.find('\0') != std::string::npos || path.find(':') != std::string::npos || path.find('*') != std::string::npos)
        return false; 
    return true;
}

void Router::split_PathQuery(const std::string& path)
{
    size_t pos = path.find("?");
    if (pos != std::string::npos && pos + 1 <= path.size())
    {
        std::string Path1;
        Path1 = path.substr(0, pos);
        _query = path.substr(pos + 1);
        _path = Path1;
    }
    else
    {
        _path = path;    
        _query = "";
    }
}

std::vector<std::string> Router::split_Path(const std::string& path)
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

bool Router::build_FinalPath(std::string& path)
{
    std::vector<std::string> tokens = split_Path(path);
    std::vector<std::string> final;

    bool hasTrailingSlash = (path.size() > 0 && path[path.size() - 1] == '/');
    for (size_t i = 0; i < tokens.size(); i++)
    {
        if (tokens[i] == "..")
        {
            if (final.size() == 0)
                return false;
            final.pop_back();
        }
        else if (tokens[i] == "" || tokens[i] == ".")
            continue;
        else
            final.push_back(tokens[i]);
    }
    this->_path = "/";
    for (size_t i = 0; i < final.size(); i++)
    {
        this->_path += final[i];
        if (i + 1 < final.size())
            this->_path += "/";
    }
    if (hasTrailingSlash && this->_path.size() > 1)
        this->_path += "/";
    return true;
}

Response Router::redirect(int redirectCode, std::string redirectUrl)
{
    Response response(_config.errorPages);
    response.set_StatusCode(redirectCode);
    if (redirectUrl.empty())
        return make_ErrorCode (redirectCode);
    response.set_Header("Location", redirectUrl);
    response.set_Body("Redirecting");
    response.set_Header("Content-Type", "text/plain");
    response.set_Header("Content-Length", "11");
    return response;
}

Response Router::handle_GET(const Request& request, Location& location)
{
    Response response(_config.errorPages);
    if (location.hasRedirect)
        return redirect(location.redirectCode, location.redirectUrl);
    if (!is_ValidMethod(location.allowedMethods, request.get_Method()))
        return make_ErrorCode(405);
    if (!location.root.empty())
        _documentRoot = location.root;
    else
        _documentRoot = _config.config.root;
    if (location.cgiPass)
        return (cgi.execute(request));
    _absolutePath = _documentRoot + _path;
    if (!is_InsideRoot(_absolutePath, _documentRoot))
        return make_ErrorCode(403);
    if (is_Directory(_absolutePath))
    {
        std::string index = _absolutePath + _config.config.index;
        if (check_File(index))
            _absolutePath = index;
        else if (location.autoIndex)
        {
            std::string html;
            if (!generateAutoIndex(_absolutePath, _path, html))
                return make_ErrorCode(500);
            response.set_Body(html);
            response.set_Header("Content-Type", get_MimeType(get_Extension(".html"), _config.mimeTypes.types));
            return response;
        }
        else
            return make_ErrorCode(403);
    }    
    if (!check_File(_absolutePath)) //if it's not a directory but the file doens't exist
        return make_ErrorCode(404);
    std::string content;
    if (!read_File(_absolutePath, content))
        return make_ErrorCode(500);
    response.set_StatusCode(200);
    response.set_Body(content);
    std::string MimeType = get_MimeType(get_Extension(_absolutePath), _config.mimeTypes.types);
    response.set_Header("Content-Type", MimeType);
    return response;
}

Response Router::handle_DELETE(const Request& request, Location& location)
{
    (void)request;
    Response response(_config.errorPages);
    if (!location.root.empty())
        _documentRoot = location.root;
    else
        _documentRoot = _config.config.root;
    _absolutePath = _documentRoot + _path;
    if (!is_InsideRoot(_absolutePath, _documentRoot))
        return make_ErrorCode(403);
    if (!check_File(_absolutePath))
        return make_ErrorCode(404);
    if (is_Directory(_absolutePath))
        return make_ErrorCode(403);
    if (std::remove(_absolutePath.c_str()) != 0)
        return make_ErrorCode(500);
    response.set_StatusCode(204);
    response.set_Body("File deleted");
    std::string MimeType = get_MimeType(get_Extension(_absolutePath), _config.mimeTypes.types);
    response.set_Header("Content-Type", MimeType);
    response.set_Header("Content-Length", "12");
    return response;
}
Response Router::handle_POST(const Request& request, Location& location)
{
    Response response(_config.errorPages);
    if (!location.root.empty())
        _documentRoot = location.root;
    else
        _documentRoot = _config.config.root;
    if (location.cgiPass)
        return (cgi.execute(request));
    size_t maxSize = _config.config.client_max_body_size;
    if (request.get_Body().size() > maxSize)
        return make_ErrorCode(413);
    _absolutePath = _documentRoot + _path;
    if (!is_InsideRoot(_absolutePath, _documentRoot))
        return make_ErrorCode(403);
    if (is_Directory(_absolutePath))
        return make_ErrorCode(403);
    std::ofstream file(_absolutePath.c_str(), std::ios::binary);
    if (!file.is_open())
        return make_ErrorCode(500);
    file << request.get_Body();
    file.close();
    response.set_StatusCode(201);
    response.set_Body("File uploaded");
    std::string MimeType = get_MimeType(get_Extension(_absolutePath), _config.mimeTypes.types);
    response.set_Header("Content-Type", MimeType);
    return response;
}
Response Router::handle_Request(const Request& request)
{
    if (!request.get_validRequest())
        return make_ErrorCode(400);
    if (!validate_Method(request.get_Method()))
        return make_ErrorCode(405);
    if (request.get_Body().size() > _config.config.client_body_buffer_size)
        return make_ErrorCode(413);
    split_PathQuery(request.get_Path());
    if (!validate_Path(_path))
        return make_ErrorCode(400);
    if (!build_FinalPath(_path))
        return make_ErrorCode(403);
    Location& loc = matchLocation(_path);
    if (!is_ValidMethod(loc.allowedMethods, request.get_Method()))
        return make_ErrorCode(405);
    if (request.get_Method() == "GET")
        return handle_GET(request, loc);
    else if (request.get_Method() == "DELETE")
        return handle_DELETE(request, loc);
    else if (request.get_Method() == "POST")
        return handle_POST(request, loc);
    return make_ErrorCode(405);
}


Location& Router::matchLocation(const std::string &path)
{
    if (_config.location.empty())
        throw std::runtime_error("No locations configured");
    Location* bestMatch = NULL;
    size_t bestLength = 0;

    for (size_t i = 0; i < _config.location.size(); i++)
    {
        Location& loc = _config.location[i];
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
        for (size_t i = 0; i < _config.location.size(); i++)
        {
            if (_config.location[i].path == "/")
                return _config.location[i];
        }
        return _config.location[0];
    }
    return (*bestMatch);
}
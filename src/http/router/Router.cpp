#include <http/router/Router.hpp>

Router::Router(Parser& Parser) : _parser(Parser) 
{   
    _path = "";
    _query = "";
    _method = "";
    _documentRoot = "./www/";
    _absolutePath = "";
    cgi = new CGI();
    cgi->setRouter(this);
}
Router::~Router()
{
    delete cgi;
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
    Response response(_parser.errorPages);
    response.set_StatusCode(redirectCode);
    if (redirectUrl.empty())
        return make_ErrorCode (redirectCode);
    response.set_Header("Location", redirectUrl);
    response.set_Body("Redirecting");
    response.set_Header("Content-Type", "text/plain");
    response.set_Header("Content-Length", "11");
    return response;
}

static Item createItem(const std::string &name, const struct stat &st)
{
    Item current;
    current.name = name;
    current.isDir = S_ISDIR(st.st_mode);
    current.size = st.st_size;
    current.lastModification = st.st_mtime;
    return current;
}

bool comparaByName(const Item &a, const Item &b)
{
    return (a.name < b.name);
}

void generateHTML(std::string& html, std::string& Path, std::vector<Item> &items)
{
    html += "<html>\n";
    html += "<head><title>Index of " + Path + "</title></head>\n";
    html += "<body>\n";
    html += "<h1>Index of " + Path + "</h1>\n";
    html += "<table>\n";
     for (size_t i = 0; i < items.size(); i++)
    {
        html += "<tr>";
        // -------- LINK --------
        html += "<td><a href=\"";
        html += items[i].name;
        if (items[i].isDir)
            html += "/";
        html += "\">";
        html += items[i].name;
        if (items[i].isDir)
            html += "/";
        html += "</a></td>";
        // -------- TAMANHO --------
        html += "<td>";
        if (items[i].isDir)
        {
            html += "-";
        }
        else
        {
            std::ostringstream oss;
            oss << items[i].size;
            html += oss.str();
        }
        html += "</td>";
        html += "</tr>\n";
    }
    // =========================
    // 5. FECHAR HTML
    // =========================
    html += "</table>\n";
    html += "</body>\n";
    html += "</html>\n";
}


bool generateAutoIndex(std::string &AbsolutePath, std::string &Path, std::string &html)
{
    std::vector<std::string> all;
    std::vector<Item> items;
    DIR *dir = opendir(AbsolutePath.c_str());
    if (!dir)
    {
        std::cout << "<h1>500 Internal Server Error</h1>" << std::endl;
        return false;
    }
    struct dirent *entry;
    while((entry = readdir(dir)) != NULL)
    {
        std::string name = entry->d_name;
        if (name == "." || name == "..")
            continue;
        std::string fullPath = AbsolutePath + "/" + name;
        struct stat st;
        if (stat(fullPath.c_str(), &st) == -1)
            continue;
        items.push_back(createItem(name, st));
    }
    std::sort(items.begin(), items.end(), comparaByName);
    generateHTML(html, Path, items);
    return true;
}

Response Router::make_ErrorCode(size_t code)
{
    Response res(_parser.errorPages);
    res.set_StatusCode(code);
    std::string path = _documentRoot + res.get_StatusMessage();
    std::string page;
    if (!read_File(path, page))
    {
        std::stringstream ss;
        ss << "<h1>" << code << " " << "Error Ocurred" << "</h1>";
        res.set_Body(page);
        return res;
    }
    res.set_Body(page);
    return res;
}

Response Router::handle_Request(const Request& request)
{
    Response response(_parser.errorPages);
    if (!validate_Method(request.get_Method()))
        return make_ErrorCode(405);
    split_PathQuery(request.get_Path());
    if (!validate_Path(_path))
        return make_ErrorCode(400);
    if (!build_FinalPath(_path))
        return make_ErrorCode(403);
    Location& loc = matchLocation(_path);
    if (loc.hasRedirect)
        return redirect(loc.redirectCode, loc.redirectUrl);
    if (!is_ValidMethod(loc.allowedMethods, request.get_Method()))
        return make_ErrorCode(405);
    if (!loc.root.empty())
        _documentRoot = loc.root;
    else
        _documentRoot = _parser.config.root;
    if (loc.cgiPass)
        return (cgi->execute(request));
    _absolutePath = _documentRoot + _path;
    if (!is_InsideRoot(_absolutePath, _documentRoot))
       return make_ErrorCode(403);
    if (is_Directory(_absolutePath))
    {
        std::string index = _absolutePath + _parser.config.index;
        if (check_File(index))
            _absolutePath = index;
        else if (loc.autoIndex)
        {
            std::string html;
            if (!generateAutoIndex(_absolutePath, _path, html))
                return make_ErrorCode(500);
            response.set_Body(html);
            response.set_Header("Content-Type", get_MimeType(get_Extension(".html"), _parser.mimeTypes.types));
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
    response.set_Body(content);
    std::string MimeType = get_MimeType(get_Extension(_absolutePath), _parser.mimeTypes.types);
    response.set_Header("Content-Type", MimeType);
    return response;
}


Location& Router::matchLocation(const std::string &path)
{
    if (_parser.location.empty())
        throw std::runtime_error("No locations configured");
    Location* bestMatch = NULL;
    size_t bestLength = 0;

    for (size_t i = 0; i < _parser.location.size(); i++)
    {
        Location& loc = _parser.location[i];
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
        for (size_t i = 0; i < _parser.location.size(); i++)
        {
            if (_parser.location[i].path == "/")
                return _parser.location[i];
        }
        return _parser.location[0];
    }
    return (*bestMatch);
}
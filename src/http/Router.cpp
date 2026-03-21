#include <http/Router.hpp>

Router::Router(t_parser& parser) : Parser(parser) 
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
    this->Path = "/";
    for (size_t i = 0; i < final.size(); i++)
    {
        this->Path += final[i];
        if (i + 1 < final.size())
            this->Path += "/";
    }
    if (hasTrailingSlash && this->Path.size() > 1)
        this->Path += "/";
    return true;
}

Response Router::redirect(int redirectCode, std::string redirectUrl)
{
    Response response(Parser.ErrorPages);
    response.setStatusCode(redirectCode);
    response.setHeader("Location", redirectUrl);
    response.setBody("Redirecting");
    response.setHeader("Content-Type", "text/plain");
    response.setHeader("Content-Length", "11");
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

Response Router::makeErrorCode(size_t code)
{
    Response res(Parser.ErrorPages);
    res.setStatusCode(code);
    std::stringstream ss;
    ss << "<h1>" << code << " " << res.getStatusMessage() << "</h1>";
    res.setBody(ss.str());
    return res;
}

Response Router::handleRequest(const Request& request)
{
    Response response(Parser.ErrorPages);
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
        std::string index = AbsolutePath + "/index.html";
        if (checkFile(index))
            AbsolutePath = index;
        else if (loc.autoIndex)
        {
            std::string html;
            if (!generateAutoIndex(AbsolutePath, Path, html))
                return makeErrorCode(500);
            response.setBody(html);
            response.setHeader("Content-Type", getMimeType(getExtension(".html"), Parser.MimeTypes.types));
            return response;
        }
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
    std::string MimeType = getMimeType(getExtension(AbsolutePath), Parser.MimeTypes.types);
    response.setHeader("Content-Type", MimeType);
    return response;
}


t_Location& Router::matchLocation(const std::string &path)
{
    if (Parser.Location.empty())
        throw std::runtime_error("No locations configured");
    t_Location* bestMatch = NULL;
    size_t bestLength = 0;

    for (size_t i = 0; i < Parser.Location.size(); i++)
    {
        t_Location& loc = Parser.Location[i];
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
        for (size_t i = 0; i < Parser.Location.size(); i++)
        {
            if (Parser.Location[i].path == "/")
                return Parser.Location[i];
        }
        return Parser.Location[0];
    }
    return *bestMatch;
}
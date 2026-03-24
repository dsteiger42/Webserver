#include <http/router/autoindex.hpp>

static Item createItem(const std::string &name, const struct stat &st)
{
    Item current;
    current.name = name;
    current.isDir = S_ISDIR(st.st_mode);
    current.size = st.st_size;
    current.lastModification = st.st_mtime;
    return current;
}

static bool comparaByName(const Item &a, const Item &b)
{
    return (a.name < b.name);
}

static void generateHTML(std::string& html, std::string& Path, std::vector<Item> &items)
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

    html += "</table>\n";
    html += "</body>\n";
    html += "</html>\n";
}

// ==========================
// FUNÇÃO PÚBLICA
// ==========================
bool generateAutoIndex(std::string &AbsolutePath, std::string &Path, std::string &html)
{
    std::vector<Item> items;

    DIR *dir = opendir(AbsolutePath.c_str());
    if (!dir)
    {
        std::cout << "<h1>500 Internal Server Error</h1>" << std::endl;
        return false;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
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

    closedir(dir);

    std::sort(items.begin(), items.end(), comparaByName);

    generateHTML(html, Path, items);

    return true;
}
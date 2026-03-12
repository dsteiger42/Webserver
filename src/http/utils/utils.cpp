#include <http/utils/utils.hpp>

bool readFile(const std::string& path, std::string& outContent)
{
    outContent.clear(); //limpa a string

    std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);
    if (!file.is_open())
        return false;
    file.seekg(0, std::ios::end); //avanca para o fim
    std::ifstream::pos_type size = file.tellg(); //verifica o size
    if (size < 0)
        return false;
    outContent.resize(size); //aloca espaco para size bytes
    file.seekg(0, std::ios::beg);//volta ao inicio
    file.read(&outContent[0], size); //le 
    if (!file)
        return false;
    file.close();
    return true;
}


bool isDirectory(const std::string& absolutePath)
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

bool isInsideRoot(const std::string& path)
{
    if (path.compare(0, 6, "./www/") != 0)
        return false;

    if (path.size() > 6 &&
        path[6] != '/')
        return false;

    return true;
}
bool checkFile(const std::string& index)
{
    struct stat info;
    if (stat(index.c_str(), &info) == 0 && S_ISREG(info.st_mode) && access(index.c_str(), R_OK | F_OK) == 0)
        return true;
    return false;
}

Response makeErrorCode(size_t code)
{
    Response res;
    res.setStatusCode(code);
    std::stringstream ss;
    ss << "<h1>" << code << " " << res.getStatusMessage() << "</h1>";    res.setBody(ss.str());
    return res;
}

bool isExecutable(const std::string& path)
{
    return access(path.c_str(), X_OK) == 0;
}

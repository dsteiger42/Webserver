#include <http/utils/utils.hpp>

bool read_File(const std::string& path, std::string& outContent)
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


bool is_Directory(const std::string& absolutePath)
{
    struct stat info;

    if (stat(absolutePath.c_str(), &info) != 0)
        return false;
    if (!S_ISDIR(info.st_mode))
        return false;
    /* if (access(absolutePath.c_str(), R_OK) != 0)
        return false; */
    return true;
}

bool is_InsideRoot(const std::string& path, const std::string &DocumentRoot)
{
    if (path.compare(0, DocumentRoot.size(), DocumentRoot) != 0)
        return false;
    return true;
}
bool check_File(const std::string& index)
{
    struct stat info;
    if (stat(index.c_str(), &info) == 0 && S_ISREG(info.st_mode) && access(index.c_str(), R_OK | F_OK) == 0)
        return true;
    return false;
}


bool is_Executable(const std::string& path)
{
    return access(path.c_str(), X_OK) == 0;
}

bool validate_Method(const std::string &method)
{
    return method == "GET" || method == "POST" || method == "DELETE";
}

bool is_Number(std::string &string)
{
    if (string.empty())
        return false;
    for (size_t i = 0; i < string.size(); i++)
    {
        if (string[i] == '+')
            i++;
        if (!std::isdigit(string[i]))
            return false;
    }
    return true;
}

bool is_ValidMethod(std::vector<std::string> &allowedMethods, const std::string &method)
{
    for (size_t i = 0; i < allowedMethods.size(); i++)
    {
        if (allowedMethods[i] == method)
            return true;
    }
    return false;
}
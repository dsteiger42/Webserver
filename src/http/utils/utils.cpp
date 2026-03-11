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

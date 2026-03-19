#include <iostream>
#include <dirent.h>

void testeReaddir(const std::string &path)
{
    DIR* dir = opendir(path.c_str());
    if (!dir)
    {
        std::cout << "Não consegui abrir o diretório: " << path << std::endl;
        return;
    }

    std::cout << "Conteúdo do diretório: " << path << std::endl;

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL)
    {
        std::string name = entry->d_name;

        // Ignorar "." e ".."
        if (name == "." || name == "..")
            continue;

        std::cout << "  " << name << std::endl;
    }

    closedir(dir);
}

int main()
{
    // Teste com o diretório atual
    testeReaddir("./");
    return 0;
}

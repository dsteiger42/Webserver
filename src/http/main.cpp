#include <iostream>
#include <vector>
#include <string>

#include <http/Request.hpp>

int main()
{
    Request req;

    // criar body grande
    std::string bigBody(20000, 'B');

    std::string request =
        "POST /upload HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Length: 20000\r\n"
        "\r\n" +
        bigBody;

    std::cout << "Request size: " << request.size() << "\n";

    // simular chunks de rede
    size_t chunkSize = 128;
    std::vector<std::string> chunks;

    for (size_t i = 0; i < request.size(); i += chunkSize)
        chunks.push_back(request.substr(i, chunkSize));

    std::cout << "\n===== SIMULACAO =====\n";

    for (size_t i = 0; i < chunks.size(); i++)
    {
        std::cout << "Chunk " << i << " (" << chunks[i].size() << " bytes)\n";

        try
        {
            req.parse(chunks[i]);
        }
        catch (const std::exception& e)
        {
            std::cout << "Erro: " << e.what() << "\n";
            return 1;
        }

        if (req.isDone())
        {
            std::cout << "\n→ Request completa!\n";
            break;
        }
    }

    std::cout << "\n===== RESULTADO =====\n";
    std::cout << "Method: " << req.getMethod() << "\n";
    std::cout << "Path: " << req.getPath() << "\n";
    std::cout << "Body size: " << req.getBody().size() << "\n";

    return 0;
}
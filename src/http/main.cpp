#include <iostream>
#include <http/Request.hpp>
#include <http/Response.hpp>

int main()
{
    // 1. Simular um pedido HTTP real
    std::string rawRequest =
        "POST /hello HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "User-Agent: TestClient\r\n"
        "Content-Length: 13\r\n"
        "\r\n"
        "Hello, Rafael";

    // 2. Criar o Request e fazer parsing
    Request req;
    req.parse(rawRequest);

    if (!req.isDone())
    {
        std::cout << "❌ Erro: Request não terminou o parsing.\n";
        return 1;
    }

    // 3. Mostrar o que foi parseado
    std::cout << "===== REQUEST PARSED =====\n";
    std::cout << "Method:  " << req.getMethod() << "\n";
    std::cout << "Path:    " << req.getPath() << "\n";
    std::cout << "Version: " << req.getVersion() << "\n";
    std::cout << "Body:    " << req.getBoddy() << "\n";

    std::cout << "Headers:\n";
    std::cout << "  Host: " << req.getHeader("Host") << "\n";
    std::cout << "  User-Agent: " << req.getHeader("User-Agent") << "\n";
    std::cout << "  Content-Length: " << req.getHeader("Content-Length") << "\n";

    // 4. Criar uma Response baseada no Request
    Response res;
    res.setHttpVersion(req.getVersion());
    res.setStatusCode(200);
    res.setHeader("Content-Type", "text/plain");
    res.setBody("Recebi isto: " + req.getBoddy());

    // 5. Serializar a resposta
    std::string httpResponse = res.serialize();

    // 6. Mostrar a resposta final
    std::cout << "\n===== HTTP RESPONSE =====\n";
    std::cout << httpResponse << "\n";

    return 0;
}

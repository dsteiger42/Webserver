#include <iostream>
#include <http/Request.hpp>
#include <http/Router.hpp>

void testRequest(const std::string &raw)
{
    Router router;
    Request req;

    req.parse(raw);
    Response res = router.handleRequest(req);

    std::cout << "RAW REQUEST:\n" << raw << std::endl;
    std::cout << "Method: " << req.getMethod() << std::endl;
    std::cout << "Original Path: " << req.getPath() << std::endl;
    std::cout << "Normalized Path: " << router.getPath() << std::endl;
    std::cout << "Status code: " << res.getStatusCode() << std::endl;
    std::cout << "Body: " << res.getBody() << std::endl;
    std::cout << "--------------------------" << std::endl;
}

int main()
{
    // 1️⃣ Path inválido (sem / inicial) -> 400
    testRequest(
        "GET index.html HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "\r\n"
    );

    // 2️⃣ Directory traversal -> 403
    testRequest(
        "GET /../secret.txt HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "\r\n"
    );

    // 3️⃣ Diretório sem index.html -> 404
    testRequest(
        "GET /emptydir HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "\r\n"
    );

    // 4️⃣ Path com caracteres inválidos -> 400
    testRequest(
        "GET /bad*path:illegal HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "\r\n"
    );

    // 5️⃣ Symlink fora do DocumentRoot (se existir) -> 403
    testRequest(
        "GET /symlink_to_secret HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "\r\n"
    );

    return 0;
}
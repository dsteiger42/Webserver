#include <iostream>
#include <http/Router.hpp>
#include <http/Request.hpp>
#include <http/utils/mime.hpp> // getMimeType, getExtension

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

    std::string mime = getMimeType(getExtension(router.getAbsolutePath()));
    std::cout << "MIME type: " << mime << std::endl;

    std::cout << "--------------------------" << std::endl;
}

int main()
{
    // HTML válido
    testRequest(
        "GET /index.html HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "\r\n"
    );

    // CSS válido
    testRequest(
        "GET /styles.css HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "\r\n"
    );

    // JS válido
    testRequest(
        "GET /app.js HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "\r\n"
    );

    // PNG válido
    testRequest(
        "GET /logo.png HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "\r\n"
    );

    // JPG válido
    testRequest(
        "GET /photo.jpg HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "\r\n"
    );

    // Arquivo inexistente -> 404
    testRequest(
        "GET /doesnotexist.txt HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "\r\n"
    );

    // Path inválido (sem / inicial) -> 400
    testRequest(
        "GET index.html HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "\r\n"
    );

    return 0;
}
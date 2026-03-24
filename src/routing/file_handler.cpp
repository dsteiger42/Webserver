#include <http/routing/file_handler.hpp>

Response Router::make_ErrorCode(size_t code)
{
    Response res(_parser.errorPages);
    res.set_StatusCode(code);
    std::string path = _documentRoot + res.get_FilePath();
    std::string page;
    if (!read_File(path, page))
    {
        std::stringstream ss;
        ss << "<h1>" << code << " " << "Error Ocurred" << "</h1>";
        res.set_Body(page);
        return res;
    }
    res.set_Body(page);
    return res;
}
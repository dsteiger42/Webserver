#include <utils/http/mime.hpp>

std::string get_Extension(std::string file)
{
    if (file.empty())
        return "";
    std::string extension;
    int pos = -1;
    for (int end = file.size() - 1; end >= 0; end--)
    {
        if (file[end] == '.')
        {
            pos = end;
            break;
        }
    }
    if (pos != -1)
        extension = file.substr(pos + 1);
    return extension;
}

std::string get_MimeType(const std::string& extension, std::map<std::string, std::string> mimeTypes)
{
    std::map<std::string, std::string>::const_iterator it = mimeTypes.find(extension);
    if (it != mimeTypes.end())
        return it->second;
    return ("application/octet-stream");
}
#include <http/utils/mime.hpp>

std::string getExtension(std::string file)
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

std::string getMimeType(const std::string& extension)
{
    std::map<std::string, std::string> table;
    if (table.empty())
    {
        table["html"] = "text/html";
        table["htm"]  = "text/html";
        table["css"]  = "text/css";
        table["js"]   = "application/javascript";
        table["json"] = "application/json";
        table["png"]  = "image/png";
        table["jpg"]  = "image/jpeg";
        table["jpeg"] = "image/jpeg";
        table["gif"]  = "image/gif";
        table["ico"]  = "image/x-icon";
        table["svg"]  = "image/svg+xml";
        table["txt"]  = "text/plain";
        table["pdf"]  = "application/pdf";
        table["zip"]  = "application/zip";
        table["xml"]  = "application/xml";
        table["mp4"]  = "video/mp4";
        table["mp3"]  = "audio/mpeg";
    }
    std::map<std::string, std::string>::const_iterator it = table.find(extension);
    if (it != table.end())
        return it->second;
    return ("application/octet-stream");
}
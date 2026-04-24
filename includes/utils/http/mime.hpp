#ifndef MIME_HPP
#define MIME_HPP

#include <string>
#include <map>
#include <config/model/types.hpp>
#include <utils/utils.hpp>

bool sanitize_Filename(std::string &filename);
std::string get_Extension(std::string file);
std::string get_MimeType(const std::string& extension, const std::map<std::string, std::string> &mimeTypes);
bool is_acceptableExtension(const std::string &path, Location &location);


#endif
#ifndef MIME_HPP
#define MIME_HPP

#include <string>
#include <map>
#include <config/model/types.hpp>

std::string get_Extension(std::string file);
std::string get_MimeType(const std::string& extension, std::map<std::string, std::string>);
bool is_acceptableExtension(const std::string &path, Location &location);


#endif
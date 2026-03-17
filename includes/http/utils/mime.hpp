#ifndef MIME_HPP
#define MIME_HPP

#include <string>
#include <map>

std::string getExtension(std::string file);
std::string getMimeType(const std::string& extension, std::map<std::string, std::string>);


#endif
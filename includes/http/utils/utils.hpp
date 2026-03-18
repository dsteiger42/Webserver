#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <fstream> //std::ios::binary
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <http/Response.hpp>
#include <vector>

bool readFile(const std::string& path, std::string& outContent);
bool isDirectory(const std::string& absolutePath);
bool isInsideRoot(const std::string& path, const std::string &DocumentRoot);
bool checkFile(const std::string& index);
Response makeErrorCode(size_t code);
bool isExecutable(const std::string& path);
bool validateMethod(const std::string &method);
bool isNumber(std::string &string);
bool isValidMethod(std::vector<std::string> &allowedMethods, const std::string &method);
#endif
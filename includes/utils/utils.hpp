#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <fstream> //std::ios::binary
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <http/response/Response.hpp>
#include <vector>

bool read_File(const std::string& path, std::string& outContent);
bool is_Directory(const std::string& absolutePath);
bool is_InsideRoot(const std::string& path, const std::string &DocumentRoot);
bool check_File(const std::string& index);
bool is_Executable(const std::string& path);
bool validate_Method(const std::string &method);
bool is_Number(const std::string &string);
bool is_ValidMethod(std::vector<std::string> &allowedMethods, const std::string &method);
void transform(std::string &string);
#endif
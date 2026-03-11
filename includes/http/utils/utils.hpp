#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <fstream> //std::ios::binary

bool readFile(const std::string& path, std::string& outContent);

#endif
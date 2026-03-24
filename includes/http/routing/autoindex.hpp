#ifndef AUTOINDEX_HPP
#define AUTOINDEX_HPP

#include <dirent.h>
#include <sys/stat.h>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <vector>

struct Item
{
    std::string name;
    bool isDir;
    size_t size;
    time_t lastModification;
};

bool generateAutoIndex(std::string &AbsolutePath, std::string &Path, std::string &html);

#endif
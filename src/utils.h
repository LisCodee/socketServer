#ifndef __UTIL_H__
#define __UTIL_H__

#include "iostream"
#include "fstream"
#include "string"
#include "vector"

namespace utils
{
    bool writeToFile(std::fstream* fs, std::string content);
    std::string getMicroTimeStr();
    std::vector<std::string> split(const std::string&, char deli=' ');
    std::string& upper(std::string& );
}

#endif
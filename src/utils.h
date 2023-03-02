#ifndef __UTIL_H__
#define __UTIL_H__

#include "iostream"
#include "fstream"
#include "string"

namespace utils
{
    bool writeToFile(std::fstream* fs, std::string content);
    std::string getMicroTimeStr();
}

#endif
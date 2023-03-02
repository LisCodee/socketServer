//
// Created by Lijingxin on 2023/3/1.
//

#include "utils.h"
#include "chrono"

bool utils::writeToFile(std::fstream* fs, std::string content)
{
    if(fs->is_open())
    {
        *fs << content.c_str();
        fs->flush();
        return true;
    }
    return false;
}

long long utils::getMicroseconds()
{
    auto now = std::chrono::system_clock::now();
    auto now_us = std::chrono::time_point_cast<std::chrono::microseconds>(now);
    auto value = now_us.time_since_epoch();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(value);
    return static_cast<long long>(duration.count());
}
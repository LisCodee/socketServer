//
// Created by Lijingxin on 2023/3/1.
//

#include "utils.h"
#include "chrono"
#include "iomanip"
#include "sstream"


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

std::string utils::getMicroTimeStr()
{
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    auto timer = std::chrono::system_clock::to_time_t(now);
    std::tm bt = *std::localtime(&timer);
    std::stringstream timeStr;
    timeStr << std::put_time(&bt, "%Y%m%d%H%M%S")<< ms.count();
    return timeStr.str();
}
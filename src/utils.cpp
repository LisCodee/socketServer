//
// Created by Lijingxin on 2023/3/1.
//

#include "utils.h"

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
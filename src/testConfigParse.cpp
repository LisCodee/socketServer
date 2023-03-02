//
// Created by Lijingxin on 2023/3/2.
//

#include "ConfigParse.h"
#include "iostream"

int main()
{
    ConfigParse cp("./config.cfg", ' ');
    std::string res = cp.get("aa");
    std::cout << res << "\n";
}
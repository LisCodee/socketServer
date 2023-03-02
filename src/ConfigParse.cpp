//
// Created by Lijingxin on 2023/3/1.
//

#include "ConfigParse.h"
#include "iostream"
#include "utils.h"
#include "vector"

ConfigParse::ConfigParse(const std::string &configPath, char deli) : m_configPath(configPath), m_cDeli(deli) {
    parse();
}

void ConfigParse::parse() {
    std::ifstream iStream(m_configPath, std::ios::in);
    if (iStream.is_open()) {
        std::string line, key, value;
        while (std::getline(iStream, line)) {
            utils::upper(line);
            std::vector<std::string> items = utils::split(line, m_cDeli);
            if (items.size() < 2) {
                std::cout << "Invalid content in configFile！" << std::endl;
                exit(-1);
            }
            m_dict.insert(std::make_pair(items[0], items[1]));
        }
        iStream.close();
    }
}

std::string ConfigParse::get(const std::string & key) const {
    std::string findKey = key;
    utils::upper(findKey);
    auto iter = m_dict.find(findKey);
    if(iter != m_dict.end())
        return iter->second;
    return "";
}
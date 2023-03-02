//
// Created by Lijingxin on 2023/3/1.
//

#ifndef SOCKETSERVER_CONFIGPARSE_H
#define SOCKETSERVER_CONFIGPARSE_H

#include "string"
#include "map"

class ConfigParse{
public:
    explicit ConfigParse(const std::string&, char deli=' ');
    ~ConfigParse(){};

    ConfigParse(const ConfigParse&) = delete;
    ConfigParse& operator=(const ConfigParse&) = delete;

public:
    std::string get(const std::string&) const;
private:
    void parse();
private:
    std::map<std::string, std::string>  m_dict;
    std::string                         m_configPath;
    char                                m_cDeli;
};


#endif //SOCKETSERVER_CONFIGPARSE_H

#include "mprpcconfig.h"

#include <iostream>
#include <string>

std::unordered_map<std::string, std::string> MprpcConfig::m_configMap;
void MprpcConfig::LoadConfigFile(const char* config_file)
{
    FILE *pf = fopen(config_file,"r");
    if(nullptr == pf){
        std::cout<<config_file << "is not exist!" <<std::endl;
        exit(EXIT_FAILURE);
    }

    while(!feof(pf))
    {
        char buf[512] = {0};
        fgets(buf,512,pf);

        std::string read_buf(buf);
        //去掉字符串前、后多余的空格
        Trim(read_buf);
        if(read_buf[0] == '#' || read_buf.empty())
        {
            continue;
        }
        //解析配置项
        int idx = read_buf.find('=');
        if(idx == -1)
        {
            continue;
        }
        std::string key;
        std::string value;
        key = read_buf.substr(0,idx);
        Trim(key);
        int endidx = read_buf.find('\n',idx);
        value = read_buf.substr(idx+1, endidx - idx-1);
        Trim(value);
        m_configMap.insert({key,value});
    }
    
}

std::string MprpcConfig::Load(const std::string& key)
{
    if(m_configMap.count(key)){
        return m_configMap[key];
    }else{
        return "";
    }
}

//去掉字符串前后的空格
void MprpcConfig::Trim(std::string &src_buf)
{
    int idx = src_buf.find_first_not_of(' ');
    if(idx != -1)
    {
        //说明字符串前面有空格
        src_buf = src_buf.substr(idx, src_buf.size() -idx);
    }
    //去掉字符串后面多余的空格
    idx = src_buf.find_last_not_of(' ');
    if(idx != -1)
    {
         src_buf = src_buf.substr(0,idx + 1);
    }
}
// const std::unordered_map<std::string, std::string>& MprpcConfig::GetMap()
// {
//     return m_configMap;
// }
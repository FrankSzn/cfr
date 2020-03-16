#ifndef CFR_UTILITY_H
#define CFR_UTILITY_H

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <unistd.h>

std::vector<std::string> split(const std::string& s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}



std::string getCwd(){
    //获取当前工作目录
    std::string path;
    path = getcwd(nullptr,0);
    return path;
}

int str_to_int(std::string str){
    int result = atoi(str.c_str());
    return result;
}


#endif //CFR_UTILITY_H

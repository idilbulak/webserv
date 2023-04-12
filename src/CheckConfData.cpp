#include "../inc/Config.hpp"
#include <unistd.h>


void Config::CheckPort(std::string port)
{
    try
    {
        std::stoi(port);
    }
    catch(const std::exception& e){
        throw std::invalid_argument("Port Error!");
        }
}

void Config::CheckServerName (std::string serverName)
{
    if (check_word(serverName))
       throw std::invalid_argument("CheckServerName error");
}





void Config::CheckClientMaxBodySize (std::string maxBodysize){
    std::string txt;
    if(maxBodysize.back() != 'm')
        txt = maxBodysize;
    else
        txt = maxBodysize.substr(0, maxBodysize.size() - 1);
    for (int i = 0; i < txt.length(); i++)
    {
        if (!isdigit(txt[i]))
            throw std::invalid_argument("CheckClientMaxBodySize Error!");
    }
}
 

void Config::CheckLocation(std::vector<Location> location)
{
    for (int i = 0; location.size() > i ; i++)
    {
        CheckPath(location[i].path);
        CheckAllow(location[i].methods);
      }
    
}

void Config::CheckPath(std::string path){
    if (path.empty())
        throw std::invalid_argument("Path Error");
}

void Config::CheckAllow(std::vector<std::string> methods){
    for (int i = 0; i < methods.size(); i++)
    {
        if (!(methods[i].compare("PUT") == 0 || methods[i].compare("GET") == 0 || methods[i].compare("POST") == 0 || methods[i].compare("DELETE") == 0))
            throw std::invalid_argument("Methods error");
    }
}






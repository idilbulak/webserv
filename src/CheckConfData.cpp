#include "../inc/Config.hpp"
#include <unistd.h>


void Config::CheckPort(std::string port)
{
    try
    {
        std::stoi(port);
    }
    catch(const std::exception& e)
    {
        throw std::invalid_argument("Port Error!");
    }
}

void Config::CheckServerName (std::string serverName)
{
    if (check_word(serverName))
       throw std::invalid_argument("CheckServerName error");
}





void Config::CheckClientMaxBodySize (std::string maxBodysize){
    if(maxBodysize.back() != 'm')
        throw std::invalid_argument("CheckClientMaxBodySize(without M) Error!");
    std::string txt = maxBodysize.substr(0, maxBodysize.size() - 1);
    try
    {
        std::stoi(txt);
    }
    catch(const std::exception& e)
    {
        throw std::invalid_argument("CheckClientMaxBodySize(convert int) Error!");
    }
    
}

void Config::CheckLocation(std::vector<Location> location)
{
    for (int i = 0; location.size() > i ; i++)
    {
        CheckPath(location[i].path);
        CheckAllow(location[i].methods);
        CheckRoot(location[i].root);
        CheckCgiExit(location[i].cgi_ext,location[i].cgi_path);
        CheckUploadDir(location[i].upload_dir);
        CheckIndex(location[i].index);
        CheckReturn(location[i].redirect_cd, location[i].redirect_url);
    }
    
}

void Config::CheckPath(std::string path){
    if (path.empty())
        throw std::invalid_argument("Path Error");
}

void Config::CheckAllow(std::vector<std::string> methods){
    // for (int i = 0; i < methods.size(); i++)
    // {
    //     if (!(methods[i].compare("GET") == 0 || methods[i].compare("POST") == 0 || methods[i].compare("DELETE") == 0))
    //     {
    //         throw std::invalid_argument("Methods error");}
    // }
}

void Config::CheckRoot(std::string root){
    
    if (root.empty())
        return;
    if (access(root.c_str(), F_OK) == -1) 
        throw std::invalid_argument("root path error!");
    
}

//What do i have check? i don't know..
void Config::CheckCgiExit(std::string cgi_ext,std::string cgi_path){
    if (cgi_ext.empty() && cgi_path.empty())
        return;
}

void Config::CheckUploadDir(std::string	upload_dir){
    // if (upload_dir.empty())
    //     return;
    // std::cout << upload_dir << std::endl;
    // if (access(upload_dir.c_str(), F_OK) == -1) 
    //     throw std::invalid_argument("upload direction error!");
}

//What do i have check?
void Config::CheckIndex(std::vector<std::string> index){

}

//What do i have check?
void Config::CheckReturn(int redirect_cd, std::string redirect_url){

}


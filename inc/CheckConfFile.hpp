#ifndef CHECKCONFFILE_HPP
#define CHECKCONFFILE_HPP

#include <vector>
#include <iostream>
#include <fstream>


class CheckConfFile
{
private:
    bool serverBrackets;
    bool locationBrackets;
public:
    CheckConfFile(const std::string path);
    ~CheckConfFile();


    void checkBrackets(std::vector<std::string> tokens);
    bool checkServerKey(std::string word);
    bool checkLocationKey(std::string word);
    void checkKey(const std::string path);
    std::vector<std::string> split(std::string str, std::string delimiter) ;

    //getter setter
    void setServerBrackets(bool isOpen);
    void setLocationBrackets(bool isOpen);
    bool getLocationBrackets();
    bool getServerBrackets();

};







#endif
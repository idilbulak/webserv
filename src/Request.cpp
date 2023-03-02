#include "../inc/Request.hpp"



std::vector<std::string> Request::split(std::string str, std::string delimiter) {
    std::vector<std::string> words;
    size_t pos = 0;
    std::string token;
    while ((pos = str.find(delimiter)) != std::string::npos) {
        token = str.substr(0, pos);
        if (!token.empty()) {
            words.push_back(token);
        }
        str.erase(0, pos + delimiter.length());
    }
    if (!str.empty()) {
        words.push_back(str);
    }
    return words;
}


Request::~Request(){}


std::vector<std::string> Request::split_crlf(std::string str) {
    std::vector<std::string> words;
    size_t pos = 0;
    std::string delimiter = "\r\n";

    while ((pos = str.find(delimiter)) != std::string::npos) {
        std::string token = str.substr(0, pos);
        if (!token.empty()) {
            words.push_back(token);
        }
        str.erase(0, pos + delimiter.length());
    }

    if (!str.empty()) {
        words.push_back(str);
    }

    return words;
}

void Request::findLocation(std::string buff, Config conf)
{

    for (int i = 0; conf.servers.size() > i; i++)
    {

        if (_reqPort.compare(conf.servers[i].port) == 0)
        {

            for (int x = 0; conf.servers[i].locations.size() > x; x++)
            {

                if (buff.compare(conf.servers[i].locations[x].path) == 0)
                {
                    _location = conf.servers[i].locations[x];
                }
            }
            
        }
    }
    
}


Request::Request(std::string buff, Config conf){

    // std::cout << "buff is[" << buff<<"]" << std::endl << std::endl;
    std::vector<std::string> splitBuff = split_crlf(buff);
    _reqMethod = split(splitBuff[0], " ")[0];
  
    _reqHost= split(split(splitBuff[1], " ")[1], ":")[0];
    std::vector<std::string> findPort = split(splitBuff[1], ":");
    // std::cout << splitBuff[1] << std::endl;

    if (findPort.size() == 3)
        _reqPort = findPort[2];
    else
        _reqPort = "80";

    std::cout << "info start:" << std::endl << "method is " << _reqMethod <<  "\n host is " << _reqHost << "\n port is[" << _reqPort << "]" << findPort.size() << std::endl;
      
    findLocation(split(splitBuff[0], " ")[1], conf);
    // std::cout << "info start:" << std::endl << "method is " << _reqMethod <<  "\n host is " << _reqHost << "\n port is[" << _reqPort << "]" << std::endl;
    if (_location.path.empty())
        std::cout << "wrong way" << std::endl;
    else
        std::cout << "[" <<  _location.path << "]"<< std::endl;
    

}

// std::string _request += buff;
// std::string _request += buff;
// std::string _request += buff;
// std::string _request += buff;

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

std::vector<std::string> split_crlf(std::string str) {
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

Request::~Request(){}

Request::Request(std::string buff){

    std::cout << "buff is[" << buff<<"]" << std::endl << std::endl;
        std::vector<std::string> splitBuff = split_crlf(buff);
        _reqMethod = split(splitBuff[0], " ")[0];
        _reqUri = split(splitBuff[0], " ")[1];
    
        _reqHost= split(split(splitBuff[1], " ")[1], ":")[0]; //burda sikinti var
        std::vector<std::string> findPort = split(splitBuff[1], ":");
        std::cout << splitBuff[0] << std::endl;

        if (findPort.size() == 3)
            _reqPort = findPort[2];
        else
            _reqPort = "80";
        std::cout << "info start:" << std::endl << "method is " << _reqMethod <<  "\n host is " << _reqHost << "\n port is[" << _reqPort  << "]" << "url: " << _reqUri << std::endl;
}

std::string Request::response(Config cf) {
    VirtualServer server;
    for(int i=0; i<cf.servers.size(); i++) {
        if(cf.servers[i].port == _reqPort)
            server = cf.servers[i];
    }
    Response res(server);
    if(_reqMethod.empty())
        return (res.errRes(405));
    else if(_reqMethod.compare("GET") == 0)
        return (res.getRes(_reqUri));
    else if(_reqMethod.compare("POST") == 0)
        return (res.getRes(_reqUri));
    else
       return (res.errRes(500)); 
}

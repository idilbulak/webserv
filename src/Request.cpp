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

Request::Request(std::string buff){
    std::vector<std::string> splitBuff = split(buff, "\n");
    _reqMethod = split(splitBuff[0], " ")[0];
  
    _reqHost= split(split(splitBuff[1], " ")[1], ":")[0];
    std::vector<std::string> findPort = split(split(splitBuff[1], " ")[1], ":");
  
    if (findPort.size() == 2)
        _reqPort = findPort[1];
    else
        _reqPort = "80";

    std::cout << "info start:" << std::endl << "method is " << _reqMethod <<  "\n host is " << _reqHost << "\n port is  [" << _reqPort << "hghgh]" << std::endl;
}

std::string Request::response(Config cf) {
    Response res;
    // cf.display();
    for (int i=0; i<cf.servers.size(); i++) {
        if (_reqPort == cf.servers[i].port) {
            for (int i=0; i<cf.servers[i].locations.size(); i++) {
                if (_reqUri == cf.servers[i].locations[i].path) {
                    
                }
            }
        }
    }
    if (_reqPort == "4242") {
        res.errResponse(403);
    }
    if (_reqPort == "80") {
        res.errResponse(500);
    }
    // if (_reqMethod.compare("GET"))
    //     res.getResponse(403);
    return res.getRes();
}
// std::string _request += buff;
// std::string _request += buff;
// std::string _request += buff;
// std::string _request += buff;

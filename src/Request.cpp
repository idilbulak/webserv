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

    //look every server
    for (int i = 0; conf.servers.size() > i; i++)
    {
        //find which port it is
        if (_reqPort.compare(conf.servers[i].port) == 0)
        {
            //look every location
            for (int x = 0; conf.servers[i].locations.size() > x; x++)
            {
                //find which location it is
                if (buff.compare(conf.servers[i].locations[x].path) == 0)
                {
                    _location = conf.servers[i].locations[x];
                }
            }
            
        }
    }
    
}


Request::Request(std::string buff, Config conf){

    //split crlf code
    std::vector<std::string> splitBuff = split_crlf(buff);
    
    //find method
    _reqMethod = split(splitBuff[0], " ")[0];
  
    //find host
    _reqHost= split(split(splitBuff[1], " ")[1], ":")[0];

    //find port
    std::vector<std::string> findPort = split(splitBuff[1], ":");
    if (findPort.size() == 3)
        _reqPort = findPort[2];
    else
        _reqPort = "80";

    //info
    // std::cout << "info start:" << std::endl << "method is " << _reqMethod <<  "\n host is " << _reqHost << "\n port is[" << _reqPort << "]" << findPort.size() << std::endl;
      

    //findLocation
    findLocation(split(splitBuff[0], " ")[1], conf);


    //if _location.path.empty, wrong anything
    if (_location.path.empty())
        std::cout << "wrong way" << std::endl;
    else
        std::cout << "[" <<  _location.path << "]"<< std::endl;
    

}


//GETTER
Location Request::getLocation() { return _location;}
std::string Request::getMethod(){return _reqMethod;}
std::string Request::getHost(){return _reqHost;}
std::string Request::getPort(){return _reqPort;}


// std::string _request += buff;
// std::string _request += buff;
// std::string _request += buff;
// std::string _request += buff;

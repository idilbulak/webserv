#include "../inc/Request.hpp"

Request::Request(std::string buff) {

    // std::cout << "buff is[ " << buff << " ]" << std::endl << std::endl;
        _req.body = findBody(buff);
        std::vector<std::string> splitBuff = split_crlf(buff);
        _req.method = split(splitBuff[0], " ")[0];
        _req.uri = split(splitBuff[0], " ")[1];
    
        _req.host= split(split(splitBuff[1], " ")[1], ":")[0]; //burda sikinti var
        std::vector<std::string> findPort = split(splitBuff[1], ":");
        // std::cout << splitBuff[0] << std::endl;

        if (findPort.size() == 3)
            _req.port = findPort[2];
        else
            _req.port = "80";
        // std::cout << "info start:" << std::endl << "method is " << _req.method <<  "\n host is " << _req.host << "\n port is[" << _req.port  << "]" << "url: " << _req.uri << std::endl;
}

Request::~Request() {}

std::string Request::response(Config cf) {
    makeCodeMap();
    // find the server block
    this->_server = findServer(cf);
    if (_server.port.empty())
        return(errRes(500)); //internal server error
    // find the location block 
    if(!findLocation(_req.uri, &_loc))
        return(errRes(404));
    // check if allowed method 
    if(!ValidMethod(_loc, _req.method))
        return(errRes(405)); //method not allowed
    //auto index on ise directory listing yapilabilecek bir link koy
    // if autoindex is enabled put a directory listing
    //check if root directory exist
    if(!fileExists(_loc.root.c_str()))
        return(errRes(415)); //unsopported media type
    std::cout <<"requri: "<< _req.uri <<std::endl;
    // choose the .html path
    setFilePath();
    std::cout << _indxFile << std::endl;
    if(_indxFile.empty())
        return(errRes(401));
    else if(hasCgiExtension(_indxFile))
        return(cgiRes(_req));
    else
        return(generate());
}

// if uri ends with '/', first search in index, then try cgi scripts.
void Request::setFilePath() {
    if(endsWithSlash(_req.uri))
    {
        if(checkIndx())
            return ;
        else if (fileExists(_loc.cgi_path.c_str())) {
                _indxFile = _loc.cgi_path;
                return ;
        }
    }
    else if (hasCgiExtension(_req.uri)) {
        if (fileExists(_loc.cgi_path.c_str())) {
            _indxFile = _loc.cgi_path;
            return ;
        }
        else if (checkIndx()) {
            return ;
        }
    }
}

VirtualServer Request::findServer(Config cf) {
    VirtualServer server;
    for(int i=0; i<cf.servers.size(); i++) {
        if(cf.servers[i].port == _req.port)
            server = cf.servers[i];
    }
    return (server);
}

bool Request::findLocation(std::string reqUri, Location* loc) {
    for (int i = 0; i < _server.locations.size(); i++) {
        if (_server.locations[i].path.compare(reqUri) == 0) {
            *loc = _server.locations[i];
            return true;
        }
    }
    return false;
}

bool Request::ValidMethod(Location loc, std::string method) {
	return std::find(loc.methods.begin(), loc.methods.end(), method) != loc.methods.end();
}

bool Request::fileExists(const char* filename) {
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

bool Request::endsWithSlash(const std::string& str) {
    if (str.length() == 0) {
        return false;
    }
    return (str[str.length() - 1] == '/');
}

bool Request::checkIndx() {
    for (int i = 0; i < _loc.index.size(); i++) {
        std::string filename = _loc.root + "/" + _loc.index[i];
        if (fileExists(filename.c_str()))
            _indxFile = filename;
            return true;
    }
    return false;
}

bool Request::hasCgiExtension(const std::string& filename) {
    std::string extension = ".cgi";
    std::string fileExtension = "";
    size_t dotPos = filename.find_last_of(".");
    if (dotPos != std::string::npos)
        fileExtension = filename.substr(dotPos);
    return (fileExtension == extension);
}

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

std::string Request::findBody(std::string buff) {
    std::string req_body;

    // Find the start of the request body
    size_t body_start = buff.find("\r\n\r\n");
    if (body_start != std::string::npos)
        body_start += 4;

    // Extract the request body
    if (buff.find("Content-Length:") != std::string::npos) {
        size_t length_start = buff.find("Content-Length:") + 16;
        size_t length_end = buff.find("\r\n", length_start);
        std::string length_str = buff.substr(length_start, length_end - length_start);
        int content_length = std::atoi(length_str.c_str());
        if (content_length > 0) {
            req_body = buff.substr(body_start, content_length);
        }
    }
    return req_body;
}

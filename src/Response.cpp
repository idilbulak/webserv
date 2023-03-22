#include "../inc/Response.hpp"

Response::Response(std::string buff, Config cf) : _cf(cf), _req(HttpRequest(buff)){
	makeCodeMap();
}

Response::~Response(void) {

}

std::string	Response::generate() {
	// if (findServer(_req).port.empty())
    //     return(errRes(500));
	if(!findLocation(_req.getUri(), &_loc, findServer(_req)))
		// std::cout << _req.getUri()<< std::endl;
        return(codeRes(404));
	if(!validMethod(_loc, _req.getMethod()))
        return(codeRes(405));
	// return?
	// if(!fileExists(_loc.root.c_str()))
    //     return(codeRes(415));
	setIndxFile();
	std::cout << "index file path:\t" << _indxFile << std::endl;
	if(_indxFile.empty())
        return(codeRes(401));
    else if(_cgiOn)
        return(cgiRes());
    else {
		return(chooseMethod());
	}
}

std::string Response::chooseMethod() {
		if(_req.getMethod().compare("GET") == 0)
        	return (getRes());
		else if(_req.getMethod().compare("POST") == 0)
			return (postRes());
		else if(_req.getMethod().compare("DELETE") == 0)
		    return (delRes());
		else
			return (codeRes(502));
}

std::string Response::getRes() {
	if(_version.empty())
 	   _version = "HTTP/1.1";
	_code = 200;
	// _type = "Content-Type: text/html; charset=UTF-8";
	_filename = _indxFile;
    return res();
}

std::string Response::postRes() {
	// upload file
	std::cout << _loc.upload_dir << std::endl;
	if (_loc.upload_dir.empty())
		return(codeRes(503));
	if (_req.getBody().empty())
		return(codeRes(200));
	else if (_req.getCntName().compare("file") == 0) {
		if (_req.getCntFileName().empty())
			return(codeRes(404));
		if(!fileExists(_loc.upload_dir.c_str())) {
			std::cout << "No upload_dir" << std::endl;
			return(codeRes(500));
		}
		std::string path = _loc.upload_dir + "/" + _req.getCntFileName();
		std::ofstream outfile(path);
    	if (outfile.is_open()) {
        outfile << _req.getContent();
        outfile.close();
        std::cout << "File created successfully.\n" << _indxFile;
		if(_indxFile.empty() == 0) {
			_version = "HTTP/1.1";
			_code = 200;
			_type = "Content-Type: text/html; charset=UTF-8";
			_filename = _indxFile;
			return res();
		}
		return(codeRes(201));
		} 
	}
	else {
		std::cout << "Failed to create file.\n";
		return(codeRes(415));
	}
	if(_indxFile.empty() == 0) {
		_version = "HTTP/1.1";
		_code = 200;
		_type = "Content-Type: text/html; charset=UTF-8";
		_filename = _indxFile;
		return res();
	}
	return(codeRes(200));
}

std::string Response::delRes() {
	if (_loc.upload_dir.empty())
		return(codeRes(503));
	if(!fileExists(_loc.upload_dir.c_str())) {
		std::cout << "No upload_dir" << std::endl;
		return(codeRes(500));
	}
	std::string path = _loc.upload_dir + "/" + _req.getFileToDelete();
	if (std::remove(path.c_str()) == 0) {
    std::cout << "Deleted file " << path << std::endl;
	} else {
		std::cout << "Failed to delete file: " << path << std::endl;
		return(codeRes(404));
	}
    return codeRes(204);
}

void Response::extractCgiRes()
{
	size_t versionPos = _cgiRes.find("HTTP/");
    if (versionPos != std::string::npos)
    {
        size_t versionEndPos = _cgiRes.find(" ", versionPos);
        if (versionEndPos != std::string::npos)
            _version = _cgiRes.substr(versionPos, versionEndPos - versionPos);
    }
	else
		_version = "HTTP/1.1";
    size_t typePos = _cgiRes.find("Content-type: ");
	std::cout <<typePos <<std::endl;
    if (typePos != std::string::npos)
    {
        size_t typeEndPos = _cgiRes.find("\r\n", typePos);
        if (typeEndPos != std::string::npos)
            _type = _cgiRes.substr(typePos + 14, typeEndPos - typePos - 14);
    }
	else
		_type = "text/html";
    size_t bodyPos = _cgiRes.find("\r\n\r\n");
    if (bodyPos != std::string::npos)
        _body = _cgiRes.substr(bodyPos + 4);
	size_t lengthPos = _cgiRes.find("Content-length: ");
    if (lengthPos != std::string::npos)
    {
        size_t lengthEndPos = _cgiRes.find("\r\n", lengthPos);
        if (lengthEndPos != std::string::npos)
        {
            std::string length_str = _cgiRes.substr(lengthPos + 16, lengthEndPos - lengthPos - 16);
            _contentLength = atoi(length_str.c_str());
        }
    }
	else
		_contentLength = _body.size();
}


std::string Response::cgiRes() {
	_cgiRes = Cgi(_indxFile, _req).execute();
	extractCgiRes();
	return(chooseMethod());
	std::cout << "cgires:" << _cgiRes << std::endl;
	std::cout << "body: " << _body << std::endl;
	std::cout << "type: " << _type << std::endl;
	std::cout << "version: " << _version << std::endl;
	std::cout << "length: " << _contentLength << std::endl;
	//if cgi status is not 200 return error
	//then check methods and send to there...
	// _version = "HTTP/1.1 ";
	// return res();
}

//making a map for status codes
void Response::makeCodeMap() {
	_codeMap.insert(std::pair<int, std::string>(100,"100 Continue"));
	_codeMap.insert(std::pair<int, std::string>(101,"101 Switching Protocols"));
	_codeMap.insert(std::pair<int, std::string>(200,"200 OK"));
	_codeMap.insert(std::pair<int, std::string>(201,"201 Created"));
	_codeMap.insert(std::pair<int, std::string>(202,"202 Accepted"));
	_codeMap.insert(std::pair<int, std::string>(203,"203 Non-Authoritative Information"));
	_codeMap.insert(std::pair<int, std::string>(204,"204 No Content"));
	_codeMap.insert(std::pair<int, std::string>(205,"205 Reset Content"));
	_codeMap.insert(std::pair<int, std::string>(300,"300 Multiple Choices"));
	_codeMap.insert(std::pair<int, std::string>(301,"301 Moved Permanently"));
	_codeMap.insert(std::pair<int, std::string>(302,"302 Found"));
	_codeMap.insert(std::pair<int, std::string>(303,"303 See Other"));
	_codeMap.insert(std::pair<int, std::string>(305,"305 Use Proxy"));
	_codeMap.insert(std::pair<int, std::string>(306,"306 (Unused)"));
	_codeMap.insert(std::pair<int, std::string>(307,"307 Temporary Redirect"));
	_codeMap.insert(std::pair<int, std::string>(400, "400 Bad Request"));
	_codeMap.insert(std::pair<int, std::string>(401, "401 Unauthorized"));
	_codeMap.insert(std::pair<int, std::string>(402, "402 Payment Required"));
	_codeMap.insert(std::pair<int, std::string>(403, "403 Forbidden"));
	_codeMap.insert(std::pair<int, std::string>(404, "404 Not Found"));
	_codeMap.insert(std::pair<int, std::string>(405, "405 Method Not Allowed"));
	_codeMap.insert(std::pair<int, std::string>(408, "408 Request Timeout"));
	_codeMap.insert(std::pair<int, std::string>(409, "409 Conflict"));
	_codeMap.insert(std::pair<int, std::string>(410, "410 Gone"));
	_codeMap.insert(std::pair<int, std::string>(413, "413 Payload Too Large"));
	_codeMap.insert(std::pair<int, std::string>(414, "414 URI Too Long"));
	_codeMap.insert(std::pair<int, std::string>(415, "415 Unsupported Media Type"));
	_codeMap.insert(std::pair<int, std::string>(429, "429 Too Many Requests"));
	_codeMap.insert(std::pair<int, std::string>(500, "500 Internal Server Error"));
	_codeMap.insert(std::pair<int, std::string>(501, "501 Not Implemented"));
	_codeMap.insert(std::pair<int, std::string>(502, "502 Bad Gateway"));
	_codeMap.insert(std::pair<int, std::string>(503, "503 Service Unavailable"));
	_codeMap.insert(std::pair<int, std::string>(504, "504 Gateway Timeout"));
	_codeMap.insert(std::pair<int, std::string>(505, "505 HTTP Version Not Supported"));
}

// Returns an HTTP response string that includes the HTTP version, status code, content type,
// and the content of a file specified by the filename argument. (date and time?)
std::string	Response::res() {
// 	std::cout << _res.version << "|" << std::endl;
// 	std::cout << _res.type << std::endl;
// // exit(1);
	// if(_res.version.compare("HTTP/1.1") == 0){
	// 	std::cout << _res.code << std::endl;
	// 	return(errRes(505)); //version not supported
	// 	/////////////buraya
	// 	}
	// if(_res.type.compare("Content-Type: text/html; charset=UTF-8") == 0 || _res.type.compare("Content-Type: text/html") == 0)
	// 	return(errRes(503)); //service unavailable
	// if (_codeMap.find(_res.code) != _codeMap.end())
	// 	return(errRes(404)); //not found
	std::string res = _version + " ";
		res += statuscode(_code) + CRFL;
		res += _type + CRFL;
		res += CRFL;
		if(_body.empty())
			res += read_html_file(_filename);
		else if (_body.empty() == 0)
			res += _body;
		else
			return(codeRes(404)); 
	std::cout << RED << getTime() << RESET << "\tResponse: \n" << CYAN << res << RESET  << std::endl;
	return (res);
}

// Read the contents of an HTML file into a string
std::string Response::read_html_file(const std::string& fileName) {
	std::ifstream file(fileName);
	if (!file.is_open()) {
		throw std::runtime_error("Failed to open file: " + fileName);
	}
	std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	return content;
}

VirtualServer Response::findServer(HttpRequest req) {
    VirtualServer server;
    for(int i=0; i<_cf.servers.size(); i++) {
        if(_cf.servers[i].port == req.getPort())
            server = _cf.servers[i];
    }
    return (server);
}

bool Response::findLocation(std::string reqUri, Location* loc, VirtualServer server) {
    for (int i = 0; i < server.locations.size(); i++) {
		// std::string	root = server.root;
        if (server.locations[i].path.compare(reqUri) == 0) {
            *loc = server.locations[i];
			if (loc->root.empty())
				loc->root = server.root;
            return true;
        }
    }
    return false;
}

bool Response::validMethod(Location loc, std::string method) {
	return std::find(loc.methods.begin(), loc.methods.end(), method) != loc.methods.end();
}

bool Response::fileExists(const char* filename) {
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

void Response::setIndxFile() {
    if(checkIndx())
        _cgiOn = 0;
    else if (fileExists(_loc.cgi_path.c_str())) {
		std::cout << _loc.cgi_path << "bu path" << std::endl;
        _indxFile = _loc.cgi_path;
        _cgiOn = 1;
    }
	else if (_loc.autoindex) {
		std::cout << _loc.autoindex << std::endl;
		AutoIndex listing(_loc.root);
		std::string dst= _loc.root + "/listing.html";
		moveFile("listing.html", dst);
		_indxFile = _loc.root + "/listing.html";
		_cgiOn = 0;
	}
	// else if (_loc.redirect_url)
	// autoindex of check etmeti unutmaaaaaaaaaa!!!!!!!!!!
	else
		std::cout << RED << getTime() << RESET << "\tPage not found. \n" << RESET  << std::endl; 
}

bool Response::checkIndx() {
    for (int i = 0; i < _loc.index.size(); i++) {
		std::cout << _loc.root << std::endl;
        std::string filename = _loc.root + "/" + _loc.index[i];
        if (fileExists(filename.c_str())) {
            _indxFile = filename;
			std::cout << filename << std::endl;
            return true;
		}
    }
    return false;
}

std::string Response::statuscode(int cd) {
	return _codeMap[cd];
}

// Returns an HTTP error response string
std::string Response::codeRes(int err) {
	_code = err;
	_version = "HTTP/1.1";
	_type = "Content-Type: text/html; charset=UTF-8";
	_filename = std::to_string(err/100) + "xx_html/" + std::to_string(err) + ".html";
	return (res());
}

void Response::moveFile(const std::string& source_path, const std::string& destination_path) {
    int result = std::rename(source_path.c_str(), destination_path.c_str());
    if (result != 0) {
        std::cerr << "Error: Failed to move file from " << source_path << " to " << destination_path << std::endl;
    }
}

// std::string Request::findValidFile(Location loc) {
//     for (int i = 0; i < loc.index.size(); i++) {
//         std::string filename = loc.root + "/" + loc.index[i];
//         if (fileExists(filename.c_str()))
//             return filename;
//     }
//     return "";
// }

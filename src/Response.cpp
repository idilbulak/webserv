#include "../inc/Response.hpp"

Response::~Response(void) {}

Response::Response(std::string buff, Config cf, std::string port) : _cf(cf), _req(HttpRequest(buff, port)){
	makeCodeMap();
	_server = findServer();
}

bool Response::fileExists(std::string name) {
	std::string filename = _server.root + "/" + name;
    struct stat buffer;
    if (stat(filename.c_str(), &buffer) == 0) {
		return true;
	}
	return (false);
}

bool Response::checkIndx() {
    for (int i = 0; i < _loc.index.size(); i++) {
		std::string path;
		if(!_loc.root.empty())
			path = _loc.root + _loc.index[i];
		else
			path = _loc.index[i];
        if (fileExists(path)) {
			_indxFile = _server.root + "/" + path;
            return true;
		}
    }
    return false;
}

bool Response::folderExists(const std::string folder_to_check) {
    std::string path = _server.root + "/" + folder_to_check;
    DIR* dir = opendir(path.c_str());

    if (dir) {
        // The folder exists
        closedir(dir);
		return true;
    } else {
        // The folder does not exist
        return false;
    }
}

std::string	Response::generate() {
	if (!findLocation(&_loc, _req.getUri().path)) {
		_code = 404;
		return errRes("Location block not found");
	}
	if(!validMethod(_req.getMethod())) {
		_code = 405;
		return(errRes("Not allowed method"));
	}
	if(_req.getVersion().compare("HTTP/1.1")) {
		_code = 505;
		return(errRes("HTTP Version Not Supported"));
	}
	_cgiOn = 0;
	if (!_loc.cgi_path.empty()) {
		std::string cgiPath;
		if (!_loc.root.empty())
			cgiPath = _loc.root + "/" + _loc.cgi_path;
		else
			cgiPath = _loc.cgi_path;
		std::string fullpath = _server.root + "/" + cgiPath;
		_typePath = pathType(fullpath);
		// if path folder
		if (_typePath == 2) {
			if(folderExists(_loc.cgi_path)) {
				// correct cgi.
				// if(_ext.empty() || _ext.substr(1).compare(_loc.cgi_ext)) {
					// _indxFile = findFirstFileWithExtension(_loc.cgi_path, _loc.cgi_ext);
					// _cgiOn = 1;
				// }
			}
		}
		// if path file
		// ////////////////
		else if (_typePath == 1) {
			if(fileExists(cgiPath)) {
				// correct cgi 
				// if(_ext.empty() || _ext.substr(1).compare(_loc.cgi_ext)) {
					_indxFile = fullPath;
					_cgiOn = 1;
				// }
			}
		}
	}
	if(!_file.empty()) {
		_cgiOn = 1;
	}
	return (chooseMethod());
}

std::string	Response::cgiOff() {
	if (!checkIndx()) {
		if (_loc.autoindex == 1)
		{
			_code = 200;
			_type = 0;
			_body = AutoIndex(_loc.root).generate_html_page();
			return res();
		}
		else
		{
			_code = 404;
			return (errRes("Cgi script not found and autoindex off"));
		}
	}
	else {
		_code = 200;
		_body = read_html_file(_indxFile);
		return res();
	}
}

std::string Response::chooseMethod() {
		if(_req.getMethod().compare("GET") == 0)
        	return (getRes());
		else if(_req.getMethod().compare("POST") == 0)
			return (postRes());
		else if(_req.getMethod().compare("DELETE") == 0)
		    return (delRes());
		else if(_req.getMethod().compare("PUT") == 0)
		    return (putRes());
		else {
			_code = 501;
			return errRes("Not a valid method");
		}
}

int Response::writeContent(const std::string &_body, const std::string &path) {
	std::ofstream	file;

	if (pathType(path)) {
		file.open(path.c_str());
		file << _body;
		file.close();
		return (204);
	}
	else {
		file.open(path.c_str(), std::ofstream::out | std::ofstream::trunc);
		if (file.is_open() == false)
			return (403);
		file << _body;
		file.close();
		return (201);
	}
}

std::string Response::putRes(void) {
    _code = writeContent(_body, this->_req.getUri().path);

    if (!(_code == 201 || _code == 204))
        return this->errRes("Write failed");
    _type = 0;
    return res();
}

std::string Response::getRes() {
	if (!_cgiOn)
		return cgiOff();
	_cgiRes = Cgi(_server.root + _loc.cgi_path, *this).execute();
	parseCgiResponse();
	if (_cgiCode == 302)
    {
        _type = 2;
        _code = 302;
        return res();
    }
    if (_cgiCode != 200)
    {
        _code = _cgiCode;
        return errRes("Cgi error");
    }
	_code = _cgiCode;
	if (!_file.empty()) {
		if (!readContent(_indxFile))
            	return (errRes("Can't open file"));
	}
	return res();
}

int Response::readFile(const std::string &path) {
    std::ifstream file(path.c_str());
	if (!file.is_open())
		return (0);
    std::stringstream buffer;
    buffer << file.rdbuf();
    _body = buffer.str();
	file.close();
	return (1);
}

bool Response::readContent(const std::string &path) {
    if (readFile(path)) {
        _type = 1;
        _code = 200;
        return (1);
    }
    else {
        _type = 2;
        _code = 403;
        return (0);
    }
}

// 2:dir 1:file 0:other?
int Response::pathType(const std::string& path) {
    struct stat s;
    if (stat(path.c_str(), &s) == 0) {
        if (s.st_mode & S_IFDIR)
            return 2;
        else if (s.st_mode & S_IFREG)
            return 1;
        else
            return 0;
    }
    else {
        return 0;
    }
}

std::string Response::postRes() {
	if (!_cgiOn)
		return cgiOff();
	_cgiRes = Cgi(_loc.cgi_path, *this).execute();
	// burda bastirabilirsin??
	parseCgiResponse();
	if (_cgiCode == 302)
    {
        _type = 2;
        _code = 302;
        return res();
    }
    if (_cgiCode != 200)
    {
        _code = _cgiCode;
        return errRes("Cgi error");
    }
	_code = _cgiCode;
	_type = 2;
	return res();
}

std::string Response::delRes() {
	if (pathType(_req.getUri().path) != 0) {
		if (remove(_req.getUri().path.c_str()) == 0)
			_code = 204;
		else
			_code = 403;
	}
	else
		_code = 404;
	if (_code == 403 || _code == 404)
		return (errRes("Can't delete"));
    return res();
}

void Response::parseCgiResponse(void)
{
    std::vector<std::string> lines;
    size_t tmpPos;
    lines = splitFromCrlf(_cgiRes);
    for (size_t i = 0; i < lines.size(); i++) {
        if ((tmpPos = lines[i].find("Status: ")) != std::string::npos)
            _cgiCode = atoi(lines[i].substr(tmpPos + 7, lines[i].length()).c_str());
        else if ((tmpPos = lines[i].find("Content-type: ")) != std::string::npos)
            _cgiType = lines[i].substr(tmpPos + 14, lines[i].length());   
        else if ((tmpPos = lines[i].find(":")) != std::string::npos)
            _otherHeaders[lines[i].substr(0, tmpPos)] = lines[i].substr(tmpPos + 2, lines[i].length());
    }
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

std::string Response::res() {
	_header += "HTTP/1.1 " + _codeMap[_code] + CRLF;
	// date gerekir mi?
    // _header += "Server: " + this->_server.name + CRLF;
    if (this->_type != 2) {
        _header += "Content-Length: " + itos(this->_body.size()) + CRLF;
        _header += "Content-Type: " + (this->_type == 1 ? getMimeType(this->_req.getUri().path.substr(this->_req.getUri().path.find_last_of('.') + 1, this->_req.getUri().path.length())) : "text/html") + CRLF;
    }
    _header += CRLF;
	// std::cout <<"header: " <<_header + _body << std::endl;
	// std::cout <<"body: " <<_body << std::endl;
	return(_header + _body);
}

std::string Response::errRes(std::string err)
{
    // std::cout << err << std::endl; //????
	int code = _code;
    std::string path = this->_server.error_pages[this->_code];
    if (!readContent(path))
        _body = "<!DOCTYPE html>\n<html><title>Errorpage</title><body><h1>ERROR" + itos(_code) + "</h1></body></html>";
    this->_type = 0;
	_code = code;
    return res();
}

VirtualServer Response::findServer() {
    VirtualServer server;
    for(int i=0; i<_cf.servers.size(); i++) {
		// std::cout << _cf.servers[i].port << " bla " << _req.getPort() << std::endl;
        if(_cf.servers[i].port == _req.getPort()) {
            server = _cf.servers[i];
			break ;
		}
    }
    return (server);
}

bool Response::findLocation(Location* loc, std::string str) {
    if(str.empty())
        return false;
    // check if the full path is existing in one of the location blocks.
    for (int i = 0; i < _server.locations.size(); i++) {
        if (_server.locations[i].path.compare(str) == 0) {
            *loc = _server.locations[i];
            // _path = str;
            return true;
        }
	}
    // if we cant find the lcoation block, extract the string that is next to the last slash
    std::string::size_type last_slash_pos = str.find_last_of('/');
    std::string::size_type last_dot_pos = str.find_last_of('.');
    if (last_slash_pos != std::string::npos) {
        _path.clear();
        _file.clear();
        _ext.clear();
        _path = str.substr(0, last_slash_pos);
        _file = _req.getUri().path.substr(last_slash_pos + 1);
        if (last_dot_pos != std::string::npos)
            _ext = str.substr(last_dot_pos + 1);
    }
    if (!_ext.empty()) {
		std::string newPath = "~\\." + _ext + "$";
		for (int i = 0; i < _server.locations.size(); i++) {
			if (_server.locations[i].path.compare(newPath) == 0) {
				*loc = _server.locations[i];
				return true;
            }
		}
    }
    for (int i = 0; i < _server.locations.size(); i++) {
        if (_server.locations[i].path.compare(_path) == 0) {
            std::string filePath;
            if(!_server.locations[i].root.empty())
                filePath = _server.locations[i].root + "/" + _file;
            else
                filePath = _file;
            if fileExists(filePath) {
                *loc = _server.locations[i];
                _indxFile = filePath;
                return true;
            }
        }
	
    }
    return findLocation(loc, _path);
}


std::string getPath(const std::string& filePath) {
    // Find the last occurrence of a forward slash
    std::size_t slashIndex = filePath.find_last_of('/');
    if (slashIndex == std::string::npos) {
        // No forward slash found, return the original string
        return filePath;
    }
    // Extract the directory path up to the last forward slash
    return filePath.substr(0, slashIndex);
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

bool Response::validMethod(std::string method) {
	return std::find(_loc.methods.begin(), _loc.methods.end(), method) != _loc.methods.end();
}

std::string Response::getCgiRes() {return _cgiRes;}
// std::map<std::string, std::string> Response::getOtherHeaders() {return _otherHeaders;}
VirtualServer Response::getServer() {return _server;}
HttpRequest Response::getReq() {return _req;}

std::string findFirstFileWithExtension(const std::string& directory, const std::string& extension) {
    // Open the specified directory
    DIR* dir = opendir(directory.c_str());
    if (dir == NULL) {
        std::cerr << "Failed to open directory: " << directory << std::endl;
        return "";
    }
    struct dirent* entry;
    // Iterate through the directory entries
    while ((entry = readdir(dir)) != NULL) {
        // Get the current entry's filename as a string
        std::string filename(entry->d_name);
        // Check if the filename has the specified extension
        if (filename.size() > extension.size() &&
            filename.substr(filename.size() - extension.size()) == extension) {

            // Close the directory and return the first found file with the specified extension
            closedir(dir);
            return directory + "/" + filename;
        }
    }
    // If no file with the specified extension is found, close the directory and return an empty string
    closedir(dir);
    return "";
}

std::string Response::itos(int value) {
    std::ostringstream ss;
    ss << value;
    return ss.str();
}

std::string Response::getMimeType(const std::string &ext) {
    if (ext == "html")
		return "text/html";
	if (ext == "css")
		return "text/css";
	if (ext == "js")
		return "text/javascript";
	if (ext == "jpeg" || ext == "jpg")
		return "image/jpeg";
	if (ext == "png")
		return "image/png";
	if (ext == "bmp")
		return "image/bmp";
	if (ext == "ico")
		return "image/x-icon";
	return "text/html";
}
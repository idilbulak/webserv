#include "../inc/Response.hpp"
// /*
// sets location directive and requested file path if exists
// fisrt check if the full path is one of the location directives.
// RECURSIVE(if not parse it from last seen "/".
// check if the left side of the "/" is in one of the location directives.
// if yes put the right side to _file.
// if not put the left to function again)
// */
bool Response::findLocation(std::string str) {
    if (str.empty()) {
        return false;
    }
    for (size_t i = 0; i < _server.locations.size(); i++) {
        if (_server.locations[i].path.compare(str) == 0) {
            _loc = _server.locations[i];
            return true;
        }
    }
    std::string path, file;
    std::string::size_type last_slash_pos = str.find_last_of('/');
	if (last_slash_pos == 0) {
		path = "/";
		for (size_t i = 0; i < _server.locations.size(); i++) {
            if (_server.locations[i].path.compare(path) == 0) {
                _loc = _server.locations[i];
                _file = _req.getUri().path.substr(1);
                return true;
            }
        }
	}
    else if (last_slash_pos != std::string::npos) {
        path = str.substr(0, last_slash_pos);
        file = _req.getUri().path.substr(last_slash_pos + 1);
        for (size_t i = 0; i < _server.locations.size(); i++) {
            if (_server.locations[i].path.compare(path) == 0) {
                _loc = _server.locations[i];
                _file = file;
                return true;
            }
        }
        return findLocation(path);
    }
    return false;
}

bool Response::fileExists(std::string name) {
	std::string filename = _server.root + "/" + name;
    struct stat buffer;
    if (stat(filename.c_str(), &buffer) == 0 && S_ISREG(buffer.st_mode)) {
		return true;
	}
	return (false);
}

bool Response::folderExists(const std::string folder_to_check) {
    std::string path = _server.root + "/" + folder_to_check;
    DIR* dir = opendir(path.c_str());
    if (dir) {
        // The folder exists
        closedir(dir);
		return true;
    }
    return false;
}

// finding the extension of file like ".py"
std::string Response::findExtension() {
	std::string::size_type last_dot_pos = _req.getUri().path.find_last_of('.');
	if (last_dot_pos != std::string::npos)
		return _req.getUri().path.substr(last_dot_pos);
	return "";
}

// check if the there is a location block in the conf for specific extension
void Response::CheckExtensionBlock(Location *_loc) {
	std::string ext = findExtension();
	if (!ext.empty()) {
		std::string newPath = "~\\" + ext + "$";
		for (size_t i = 0; i < _server.locations.size(); i++) {
			if (_server.locations[i].path.compare(newPath) == 0) {
					*_loc = _server.locations[i];
			}
		}
	}
}

// adds location's root directory to the path if exists
std::string	Response::formatPath(std::string path) {
    if (!_loc.root.empty())
		return  _loc.root + "/" + path;
	return path;
}

std::string getFirstFileInFolder(const std::string& folderPath) {
    DIR* dir;
    struct dirent* ent;
    std::string firstFile;

    if ((dir = opendir(folderPath.c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_REG) { // Check if the entry is a regular file
                firstFile = ent->d_name;
                break;
            }
        }
        closedir(dir);
    } else {
        std::cerr << "Could not open directory: " << folderPath << std::endl;
        return "";
    }

    return firstFile;
}

// sets the file to be served by the get method. if the file requested is present, serves it else dafult index.
void	Response::setIndxFile() {
	if (_file.empty()) {
		for (size_t i = 0; i < _loc.index.size(); i++) {
			if (fileExists(formatPath(_loc.index[i]))) {
				_indxFile =  _server.root + "/" + formatPath(_loc.index[i]);
			}
		}
	}
	else {
		if(fileExists(_file) && !checkExtension())
			_indxFile = _server.root + "/" + _file;
		else if (!_loc.root.empty() && fileExists(formatPath(_file))) {
			std::string file = formatPath(_file);
			if (!checkExtension())
				_indxFile = _server.root + "/" + file;
		}
		else if(folderExists(_file)) {
			for (size_t i = 0; i < _loc.index.size(); i++) {
				if (fileExists(_file + "/" + _loc.index[i]))
					_indxFile =  _server.root + "/" + _file + "/" + _loc.index[i];
			}
		}
		else if (!_loc.root.empty() && folderExists(formatPath(_file))) {
			for (size_t i = 0; i < _loc.index.size(); i++) {
				if (fileExists(formatPath(_file) + "/" + _loc.index[i]))
					_indxFile =  _server.root + "/" + formatPath(_file) + "/" + _loc.index[i];
			}
		}
	}
}

void	Response::setCgiPath() {
	_cgiOn = 0;
	if(!_file.empty() && checkExtension()) {
		_cgiPath = _server.root + "/" + formatPath(_file);
		if (fileExists(formatPath(_file)))
			_cgiOn = 1;
	}
	else if(!_loc.cgi_path.empty() && _file.empty()) {
		_cgiPath = _server.root + "/" + formatPath(_loc.cgi_path);
		if (fileExists(formatPath(_loc.cgi_path)))
			_cgiOn = 1;
	}
	else if(!_loc.cgi_path.empty() && _req.getMethod().compare("POST") == 0) {
		_cgiPath = _server.root + "/" + formatPath(_loc.cgi_path);
		if (fileExists(formatPath(_loc.cgi_path)))
			_cgiOn = 1;
	}
}

long long Response::megabytesToBytes(int megabytes) {
  long long bytes = megabytes;
  bytes *= 1024;
  bytes *= 1024;
  return bytes;
}

bool Response::checkExtension() {
	std::string ext = findExtension();
	if(!ext.empty()) {
		for (size_t i = 0; i < _server.cgi_ext.size(); i++) {
			if (ext.compare(_server.cgi_ext[i]) == 0)
				return true;
		}
	}
	if (ext.empty()) {
		for (size_t i = 0; i < _server.cgi_ext.size(); i++) {
			if (_server.cgi_ext[i].compare(".") == 0)
				return true;
		}
	}
	return false;
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

// Read the contents of an HTML file into a string
std::string Response::read_html_file(const std::string& fileName) {
	std::ifstream file(fileName);
	if (!file.is_open()) {
		return "";
	}
	std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	return content;
}

VirtualServer Response::findServer() {
    VirtualServer server;
    for(size_t i=0; i<_cf.servers.size(); i++) {
        if(_cf.servers[i].port == _req.getPort()) {
            server = _cf.servers[i];
			break ;
		}
    }
    return (server);
}

bool Response::validMethod(std::string method) {
	return std::find(_loc.methods.begin(), _loc.methods.end(), method) != _loc.methods.end();
}

VirtualServer Response::getServer() {return _server;}
HttpRequest Response::getReq() {return _req;}

std::string findFirstFileWithExtension(const std::string& directory, const std::string& extension) {
    DIR* dir = opendir(directory.c_str());
    if (dir == NULL) {
        std::cerr << "Failed to open directory: " << directory << std::endl;
        return "";
    }
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        std::string filename(entry->d_name);
        if (filename.size() > extension.size() &&
            filename.substr(filename.size() - extension.size()) == extension) {
            closedir(dir);
            return directory + "/" + filename;
        }
    }
    closedir(dir);
    return "";
}

std::string Response::itos(int value) {
    std::ostringstream ss;
    ss << value;
    return ss.str();
}

bool Response::readContent(const std::string path) {
    if (fileExists(path)) {
        _type = 1;
        _code = 200;
		_body = read_html_file(_server.root + "/" + path);
        return (1);
    }
    else {
        _type = 2;
        _code = 404;
        return (0);
    }
}
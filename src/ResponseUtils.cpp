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
    for (int i = 0; i < _server.locations.size(); i++) {
        if (_server.locations[i].path.compare(str) == 0) {
            _loc = _server.locations[i];
            return true;
        }
    }
    std::string path, file;
    std::string::size_type last_slash_pos = str.find_last_of('/');
	if (last_slash_pos == 0) {
		path = "/";
		for (int i = 0; i < _server.locations.size(); i++) {
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
        for (int i = 0; i < _server.locations.size(); i++) {
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
		for (int i = 0; i < _server.locations.size(); i++) {
			if (_server.locations[i].path.compare(newPath) == 0) {
				// _loc.clear
					*_loc = _server.locations[i];
					// _file.clear();
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
		for (int i = 0; i < _loc.index.size(); i++) {
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
			for (int i = 0; i < _loc.index.size(); i++) {
				if (fileExists(_file + "/" + _loc.index[i]))
					_indxFile =  _server.root + "/" + _file + "/" + _loc.index[i];
			}
		}
		else if (!_loc.root.empty() && folderExists(formatPath(_file))) {
			for (int i = 0; i < _loc.index.size(); i++) {
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
		for (int i = 0; i < _server.cgi_ext.size(); i++) {
			if (ext.compare(_server.cgi_ext[i]) == 0)
				return true;
		}
	}
	if (ext.empty()) {
		for (int i = 0; i < _server.cgi_ext.size(); i++) {
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
	// if (!fileExists(fileName))
	// 	return "";
	std::ifstream file(fileName);
	if (!file.is_open()) {
		return "";
	}
	std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	return content;
}

VirtualServer Response::findServer() {
    VirtualServer server;
    for(int i=0; i<_cf.servers.size(); i++) {
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

// std::string getPath(const std::string& filePath) {
//     // Find the last occurrence of a forward slash
//     std::size_t slashIndex = filePath.find_last_of('/');
//     if (slashIndex == std::string::npos) {
//         // No forward slash found, return the original string
//         return filePath;
//     }
//     // Extract the directory path up to the last forward slash
//     return filePath.substr(0, slashIndex);
// }

// void Response::deductPathForFolder() {
// 	// _folder = _server.root + "/";
//   size_t pos = _req.getUri().path.find(_loc.path);
//   if (pos != std::string::npos) {
//     _folder = _req.getUri().path.substr(pos + _loc.path.length() + 1);
//   }
// }

// int Response::readFile(const std::string &path) {
//     std::ifstream file(path.c_str());
// 	if (!file.is_open())
// 		return (0);
//     std::stringstream buffer;
//     buffer << file.rdbuf();
//     _body = buffer.str();
// 	file.close();
// 	return (1);
// }

// // bu error icin gibi
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
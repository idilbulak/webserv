#include "../inc/Request.hpp"

// Returns an HTTP response string that includes the HTTP version, status code, content type,
// and the content of a file specified by the filename argument. (date and time?)
std::string	Request::res() {
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
	_res.res = _res.version + " ";
		_res.res += statuscode(_res.code) + CRFL;
		_res.res += _res.type + CRFL;
		_res.res += CRFL;
		if(_res.body.empty())
			_res.res += read_html_file(_res.filename);
		else if (_res.body.empty() == 0)
			_res.res += _res.body;
		else
			return(errRes(404)); 
	std::cout << RED << getTime() << RESET << "\tResponse: \n" << CYAN << _res.res << RESET  << std::endl;
	return (_res.res);
}

// Returns an HTTP error response string
std::string Request::errRes(int err) {
	_res.code = err;
	_res.version = "HTTP/1.1";
	_res.type = "Content-Type: text/html; charset=UTF-8";
	_res.filename = std::to_string(err/100) + "xx_html/" + std::to_string(err) + ".html";
	return (res());
}

std::string Request::cgiRes() {
	std::string buff = executeCgi();
	// Find the index of the empty line
	size_t pos = buff.find("\r\n\r\n");
	// If the empty line was found, extract the substring that follows it
	if (pos != std::string::npos)
	{
		_res.body = buff.substr(pos + 4);
		// Find the "Content-type" header and extract the value
		size_t typePos = buff.find("Content-type:");
		if (typePos != std::string::npos)
		{
			size_t endPos = buff.find("\r\n", typePos);
			_res.type = buff.substr(typePos + 14, endPos - (typePos + 14));
		}
		size_t codePos = buff.find("Status: ");
		if (codePos == 0)
			_res.code = 201;
		else if (codePos != std::string::npos)
		{
			size_t endPos = buff.find("\r\n", codePos);
			_res.type = buff.substr(codePos + 8, endPos - (codePos + 8));
		}
	}
	_res.version = "HTTP/1.1 ";
	return (res());
}

std::string Request::getRes() {
    _res.version = "HTTP/1.1";
	_res.code = 200;
	_res.type = "Content-Type: text/html; charset=UTF-8";
	_res.filename = _indxFile;
    return res();
}

std::string Request::postRes() {
	// upload file
	_res.version = "HTTP/1.1";
	_res.code = 201;
	_res.type = "Content-Type: text/html; charset=UTF-8";
	_res.filename = _indxFile;
	return res();
}


std::string Request::generate() {
	if(_req.method.compare("GET") == 0)
        return (getRes());
    else if(_req.method.compare("POST") == 0)
        return (postRes());
	// else if(_req.method.compare("DELETE") == 0)
    //     return (getDel());
	else
        return (errRes(502));
}

//making a map for status codes
void Request::makeCodeMap() {
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

std::string Request::statuscode(int cd) {
	return _codeMap[cd];
}

// Read the contents of an HTML file into a string
std::string Request::read_html_file(const std::string& fileName) {
	std::ifstream file(fileName);
	if (!file.is_open()) {
		throw std::runtime_error("Failed to open file: " + fileName);
	}
	std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	return content;
}

// stat function to get information about the file specified by filename.
// If the function returns a value of 0, it means that the file exists,
// so the function returns true. Otherwise, it means that the file does not exist,
// so the function returns false.
// bool Request::fileExists(const char* filename) {
//     struct stat buffer;
//     return (stat(filename, &buffer) == 0);
// }

// std::string Request::findValidFile(Location loc) {
//     for (int i = 0; i < loc.index.size(); i++) {
//         std::string filename = loc.root + "/" + loc.index[i];
//         if (fileExists(filename.c_str()))
//             return filename;
//     }
//     return "";
// }

// static int writeContent(const std::string &content, const std::string &path)
// {
// 	std::ofstream	file;

// 		file.open(path.c_str(), std::ofstream::out | std::ofstream::trunc);
// 		if (file.is_open() == false)
// 			return (403);
// 		return (201);
// }

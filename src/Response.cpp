#include "../inc/Response.hpp"

Response::~Response(void) {}

Response::Response(std::string buff, Config cf, std::string port) : _cf(cf), _buff(buff) {
	_req = HttpRequest(buff, port);
	makeCodeMap();
	_body.clear();
	_server = findServer();
}

std::string	Response::generate() {
	if(_req.getVersion().compare("HTTP/1.1")) {
		_code = 505;
		return(errRes("HTTP Version Not Supported"));
	}
	if (!findLocation(_req.getUri().path)) {
			_code = 404;
			return errRes("Location block not found");
	}
	if(!_file.empty())
		CheckExtensionBlock(&_loc);
	if(!_loc.redirect_url.empty()) {
		_req.setUriPath(_loc.redirect_url);
		return generate();
	}
	if(!validMethod(_req.getMethod())) {
		_code = 405;
		return(errRes("Not allowed method"));
	}
	if(!_req.getBody().empty() && !_loc.max_body_size.empty()) {
		_loc.max_body_size.erase(_loc.max_body_size.size() - 1);
		size_t size = std::stoi(_loc.max_body_size);
		if(_req.getBody().length() > size) {
			_code = 413;
			return(errRes("Request Entity Too Large"));
		};
	}
	return (chooseMethod());
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

std::string Response::getRes() {
	setIndxFile();
	if (!_indxFile.empty()) {
		_code = 200;
		_body = read_html_file( _indxFile);
		_type = 1;
		return res();
	}
	setCgiPath();
	if (!_cgiOn)
		return cgiOff();
	_cgiRes = Cgi(_cgiPath, *this).execute();
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
	_type = 1;
	_body = _cgiResBody; 
	return res();
}

std::string	Response::cgiOff() {
	if (_loc.autoindex == 1) {
		_code = 200;
		_type = 0;
		_body = AutoIndex(_server.root + "/" + _loc.root).generate_html_page();
		return res();
	} else {
		_code = 404;
		return (errRes("Cgi script not found and autoindex off"));
	}
}

std::string getFilenameFromHeader(const std::string& header) {
    size_t filenamePos = header.find("filename=");
    if (filenamePos == std::string::npos) {
        return "";
    }

    // Extract the filename string from the header
    std::string filename;
    size_t start = header.find('"', filenamePos);
    size_t end = header.find('"', start + 1);
    if (start != std::string::npos && end != std::string::npos) {
        filename = header.substr(start + 1, end - start - 1);
    }
    return filename;
}

void Response::parseMultiPartBody() {
    size_t cdHeaderPos = _req.getBody().find("Content-Disposition: ");
    if (cdHeaderPos == std::string::npos) {
        return;
    }
	size_t start = _req.getBody().find("filename=\"", cdHeaderPos);
    if (start == std::string::npos) {
        return;
    }
    size_t end = _req.getBody().find('"', start + 10);
    if (start != std::string::npos && end != std::string::npos) {
        _file = _req.getBody().substr(start + 10, end - start - 10);
    }
    start = _req.getBody().find("\r\n\r\n", cdHeaderPos);
    if (start != std::string::npos) {
        _body = _req.getBody().substr(start + 4);
    }
    size_t lastLinePos = _body.rfind("------WebKitFormBoundary");
    if (lastLinePos != std::string::npos) {
        _body = _body.substr(0, lastLinePos);
    }
}

std::string Response::postRes() {
	static int i = 0;
	setCgiPath();
	if (!_cgiOn) {
		if (_file.empty()) {
			if (!_req.getHeaders()["Content-Disposition"].empty()) {
				_file = getFilenameFromHeader(_req.getHeaders()["Content-Disposition"]);
				_body = _req.getBody();
			}
			else if (_req.getHeaders()["Content-Type"].find("multipart/form-data") != std::string::npos) {
				parseMultiPartBody();
			}
			else {
				_file = "newfile" + itos(i);
				i++;
				_body = _req.getBody();

			}
		}
		std::string filename;
		if (_loc.upload_dir.empty())
			filename = _server.root + "/" + formatPath(_file);
		else
			filename = _loc.upload_dir + "/" + _file;
		std::ofstream outFile(filename.c_str());
		if (!outFile) {
			std::cerr << "Error: Unable to open file for writing: " << filename << std::endl;
			_code = 500;
			return (errRes("Internal Server Error"));
		}
		outFile << _body;
		outFile.close();
		_type = 0;
		_code = 200;
		return res();
	}
	_cgiRes = Cgi(_cgiPath, *this).execute();
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
	_body = _cgiResBody;
	std::string filename;
	if (_loc.upload_dir.empty())
		filename = _server.root + "/" + formatPath(_file);
	else
		filename = _loc.upload_dir + "/" + _file;
	std::ofstream outFile(filename.c_str());
    if (!outFile) {
        std::cerr << "Error: Unable to open file for writing: " << filename << std::endl;
		_code = 500;
		return (errRes("Internal Server Error"));
    }
    outFile << _cgiResBody;
    outFile.close();
	_type = 0;
	return res();
}

int Response::writeContent(const std::string &_body, std::string path) {
	if (_loc.upload_dir.empty())
		return 404;
	path = _loc.upload_dir + "/" + path;
	std::ofstream	file(path, std::ofstream::trunc);
	if (fileExists(path)) {
		if (file.is_open()) {
        	file << _body;
			file.close();
			return 200;
		}
	}
	else {
		if (file.is_open()) {
        	file << _body;
			file.close();
			return 201;
		}
	}
	return (404);
}

std::string Response::putRes(void) {
    _code = writeContent(_req.getBody(), _file);
    if (!(_code == 201 || _code == 200))
        return this->errRes("Write failed");
    _type = 0;
    return res();
}

std::string decoded(std::string encoded_str)
{
	 std::string decoded_str = "";
    for (std::string::size_type i = 0; i < encoded_str.length(); i++) {
        if (encoded_str[i] == '%' && i + 3 < encoded_str.length() && isxdigit(encoded_str[i+1]) && isxdigit(encoded_str[i+2])) {
            // Convert the next two hexadecimal characters to integer value
            int hex_value = std::stoi(encoded_str.substr(i+1, 2), nullptr, 16);
            // Convert the integer value to corresponding ASCII character
            char decoded_char = static_cast<char>(hex_value);
            decoded_str += decoded_char;
            i += 2; // Skip the next two characters since they are already processed
        } else {
            decoded_str += encoded_str[i];
        }
    }
	return decoded_str;
}

std::string Response::delRes() {
	std::string filename;
    size_t pos = _req.getQueryStr().find("=");
    if (pos != std::string::npos) {
        pos += 1; // Skip "filename="
            filename = _req.getQueryStr().substr(pos);
    }
	filename = decoded(filename);
	if (pathType(_loc.upload_dir + "/" + filename) == 1) {
		if (remove((_loc.upload_dir + "/" + filename).c_str()) == 0)
			_code = 200;
		else
			_code = 403;
	}
	else
		_code = 404;
	if (_code == 403 || _code == 404)
		return (errRes("Can't delete"));
    return res();
}

std::string Response::errRes(std::string err)
{
	int code = _code;
    std::string path = this->_server.error_pages[this->_code];
	std::cerr << "[ERROR] " << err << std::endl;
    if (!readContent(path))
        _body = "<!DOCTYPE html>\n<html><title>Errorpage</title><body><h1>ERROR 404</h1></body></html>";
    this->_type = 0;
	_code = code;
    return res();
}

std::string Response::res() {
	_header += "HTTP/1.1 " + _codeMap[_code] + CRLF;
    // _header += "Server: " + this->_server.name + CRLF;
    if (this->_type != 2) {
        _header += "Content-Length: " + itos(this->_body.size()) + CRLF;
        _header += "Content-Type: " + (this->_type == 1 ? getMimeType(this->_req.getUri().path.substr(this->_req.getUri().path.find_last_of('.') + 1, this->_req.getUri().path.length())) : "text/html") + CRLF;
    }
    _header += CRLF;
	return(_header + _body);
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
	if (_cgiCode ==  0)
		_cgiCode = 200;
	std::string delimiter = "\r\n\r\n"; // Blank line that separates headers and body
    size_t bodyStart = _cgiRes.find(delimiter);
    if (bodyStart != std::string::npos) {
        bodyStart += delimiter.length();
		_cgiResBody = _cgiRes.substr(bodyStart);
	}
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

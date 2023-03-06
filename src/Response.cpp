#include "../inc/Response.hpp"

Response::Response(VirtualServer server) :_server(server) {
}

Response::~Response(void) {
}

// Returns an HTTP response string that includes the HTTP version, status code, content type,
// and the content of a file specified by the filename argument. (date and time?)
std::string	Response::res(std::string vs, int code, std::string type, std::string filename) {
	std::string res = vs + " ";
		res += statuscode(code) + CRFL;
		res += type + CRFL;
		res += CRFL;
		res += read_html_file(filename);
	std::cout << res << std::endl;
	return (res);
}

// Returns an HTTP error response string
std::string Response::errRes(int err) {
	this->_statusCode = err;
	const std::string& filename = std::to_string(err/100) + "xx_html/" + std::to_string(err) + ".html";
	//type can be dynamic?
	return (res("HTTP/1.1", err, "Content-Type: text/html; charset=UTF-8", filename));
}

std::string Response::cgiRes(Location loc, std::string body, std::string method) {
	std::cout << "idil" << std::endl;
	std::string res = Cgi(_server, loc, body, method).execute();
	std::cout << "res: " << res << std::endl;

	size_t pos = res.find("\r\n");
	if (pos != std::string::npos)
	{
		_resBody = res.substr(pos + 4);
	}


	// std::cout <<  "    bu1" << _resCode << std::endl;
	// std::cout <<  "    bu2" << _resType << std::endl;
	std::cout <<  "    bu3" << _resBody << std::endl;
	std::string response = "HTTP/1.1 ";
		response += statuscode(200) + CRFL;
		response += "Content-Type: text/html; charset=UTF-8\r\n";
		response += CRFL;
		response += _resBody;
	return (response);
}

std::string Response::getRes(std::string uri, std::string body) {
    Location loc;
	//find the loc block from conf file with uri
    if (findLocation(uri, &loc)) {
		// check if that block allows the method
        if (hasValidMethod(loc , "GET")) {
			//add cgi in here
            if (!loc.cgi_ext.empty())
                return cgiRes(loc, body, "GET"); // TODO: replace with CGI
            //check if the block redirects 
			else if (loc.redirect_cd && !loc.redirect_url.empty())
                return errRes(loc.redirect_cd);
			//if no html files? or can be checked in parser?
            else if (loc.root.empty())
                return errRes(404);
            else {
				//redirects to another page with get
                std::string filename = findValidFile(loc);
                if (!filename.empty())
                    return res("HTTP/1.1", 200, "Content-Type: text/html; charset=UTF-8", filename);
            	return errRes(404); //not found
            }
        } else
            return errRes(405); //method not allowed
    } else
        return errRes(404); //not found
}

std::string Response::postRes(std::string uri, std::string body) {
	Location loc;
	//find the loc block from conf file with uri
    if (findLocation(uri, &loc)) {
		// check if that block allows the method
        if (hasValidMethod(loc , "POST")) {
			//add cgi in here
            if (!loc.cgi_ext.empty())
                return cgiRes(loc, body, "POST"); // TODO: replace with CGI
			//check if the block redirects 
            else if (loc.redirect_cd && !loc.redirect_url.empty())
                return errRes(loc.redirect_cd);
			//if no html files? or can be checked in parser?
            else if (loc.root.empty())
                return errRes(404);
            else {
				// download file in here
				// return res("HTTP/1.1", 201, "Content-Type: text/html; charset=UTF-8", filename);
                //redirects to another page with get
				std::string filename = findValidFile(loc);
				if (fileExists(filename.c_str()))
					return res("HTTP/1.1", 200, "Content-Type: text/html; charset=UTF-8", filename);
				return errRes(404); //not found
			}
        } else
            return errRes(405); //method not allowed
    } else
        return errRes(404); //not found
}

//making a map for status codes
std::string Response::statuscode(int cd) {
	std::map<int, std::string> codeMap;
	codeMap.insert(std::pair<int, std::string>(100,"100 Continue"));
	codeMap.insert(std::pair<int, std::string>(101,"101 Switching Protocols"));
	codeMap.insert(std::pair<int, std::string>(200,"200 OK"));
	codeMap.insert(std::pair<int, std::string>(201,"201 Created"));
	codeMap.insert(std::pair<int, std::string>(202,"202 Accepted"));
	codeMap.insert(std::pair<int, std::string>(203,"203 Non-Authoritative Information"));
	codeMap.insert(std::pair<int, std::string>(204,"204 No Content"));
	codeMap.insert(std::pair<int, std::string>(205,"205 Reset Content"));
	codeMap.insert(std::pair<int, std::string>(300,"300 Multiple Choices"));
	codeMap.insert(std::pair<int, std::string>(301,"301 Moved Permanently"));
	codeMap.insert(std::pair<int, std::string>(302,"302 Found"));
	codeMap.insert(std::pair<int, std::string>(303,"303 See Other"));
	codeMap.insert(std::pair<int, std::string>(305,"305 Use Proxy"));
	codeMap.insert(std::pair<int, std::string>(306,"306 (Unused)"));
	codeMap.insert(std::pair<int, std::string>(307,"307 Temporary Redirect"));
	codeMap.insert(std::pair<int, std::string>(400, "400 Bad Request"));
	codeMap.insert(std::pair<int, std::string>(401, "401 Unauthorized"));
	codeMap.insert(std::pair<int, std::string>(402, "402 Payment Required"));
	codeMap.insert(std::pair<int, std::string>(403, "403 Forbidden"));
	codeMap.insert(std::pair<int, std::string>(404, "404 Not Found"));
	codeMap.insert(std::pair<int, std::string>(405, "405 Method Not Allowed"));
	codeMap.insert(std::pair<int, std::string>(408, "408 Request Timeout"));
	codeMap.insert(std::pair<int, std::string>(409, "409 Conflict"));
	codeMap.insert(std::pair<int, std::string>(410, "410 Gone"));
	codeMap.insert(std::pair<int, std::string>(413, "413 Payload Too Large"));
	codeMap.insert(std::pair<int, std::string>(414, "414 URI Too Long"));
	codeMap.insert(std::pair<int, std::string>(415, "415 Unsupported Media Type"));
	codeMap.insert(std::pair<int, std::string>(429, "429 Too Many Requests"));
	codeMap.insert(std::pair<int, std::string>(500, "500 Internal Server Error"));
	codeMap.insert(std::pair<int, std::string>(501, "501 Not Implemented"));
	codeMap.insert(std::pair<int, std::string>(502, "502 Bad Gateway"));
	codeMap.insert(std::pair<int, std::string>(503, "503 Service Unavailable"));
	codeMap.insert(std::pair<int, std::string>(504, "504 Gateway Timeout"));
	codeMap.insert(std::pair<int, std::string>(505, "505 HTTP Version Not Supported"));
	return codeMap[cd];
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

// stat function to get information about the file specified by filename.
// If the function returns a value of 0, it means that the file exists,
// so the function returns true. Otherwise, it means that the file does not exist,
// so the function returns false.
bool Response::fileExists(const char* filename) {
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

bool Response::findLocation(std::string reqUri, Location* loc) {
    for (int i = 0; i < _server.locations.size(); i++) {
        if (_server.locations[i].path.compare(reqUri) == 0) {
            *loc = _server.locations[i];
            return true;
        }
    }
    return false;
}

bool Response::hasValidMethod(Location loc, std::string method) {
	return std::find(loc.methods.begin(), loc.methods.end(), method) != loc.methods.end();
}

std::string Response::findValidFile(Location loc) {
    for (int i = 0; i < loc.index.size(); i++) {
        std::string filename = loc.root + "/" + loc.index[i];
        if (fileExists(filename.c_str()))
            return filename;
    }
    return "";
}

// static int writeContent(const std::string &content, const std::string &path)
// {
// 	std::ofstream	file;

// 		file.open(path.c_str(), std::ofstream::out | std::ofstream::trunc);
// 		if (file.is_open() == false)
// 			return (403);
// 		return (201);
// }

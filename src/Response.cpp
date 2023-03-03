#include "../inc/Response.hpp"

Response::Response(VirtualServer server) :_server(server) {
}

Response::~Response(void) {
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

std::string Response::errRes(int err) {

	this->_statusCode = err;
	const std::string& filename = std::to_string(err/100) + "xx_html/" + std::to_string(err) + ".html";
	std::string res = "HTTP/1.1 ";
		res += statuscode(err) + CRFL;
		res += "Content-Type: text/html; charset=UTF-8\r\n";
		res += "\r\n";
		res += read_html_file(filename);
	return (res);
}

bool fileExists(const char* filename) {
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

std::string Response::getRes(std::string reqUri) {
	Location loc;
	for(int i=0; i<_server.locations.size(); i++) {
		if(_server.locations[i].path.compare(reqUri) == 0) {
			loc = _server.locations[i];
			for(int i=0; i<loc.methods.size(); i++) {
				if(loc.methods[i].compare("GET") == 0) {
					if(!loc.cgi_ext.empty())
						return(errRes(401)); ///cgi ile degistir
					else if(loc.redirect_cd && !loc.redirect_url.empty())
						return(errRes(loc.redirect_cd));
					else if (loc.root.empty())
						return(errRes(404));
					else {
						for(int i=0; i<loc.index.size(); i++) {
							std::string filename = loc.root + "/" + loc.index[i];
							if (fileExists(filename.c_str())) {
								std::string res = "HTTP/1.1 ";
									res += "200 OK\r\n";
									res += "Content-Type: text/html; charset=UTF-8\r\n";
									res += "\r\n";
									res += read_html_file(filename);
								return (res);
							}
						}
					}
				}
			return(errRes(405));
			}
		}
	}
	return (errRes(404));
}

static int writeContent(const std::string &content, const std::string &path)
{
	std::ofstream	file;

		file.open(path.c_str(), std::ofstream::out | std::ofstream::trunc);
		if (file.is_open() == false)
			return (403);
		return (201);
	}

std::string Response::postRes(std::string reqUri, std::string reqBody) {
	Location loc;
	for(int i=0; i<_server.locations.size(); i++) {
		if(_server.locations[i].path.compare(reqUri) == 0) {
			loc = _server.locations[i];
			for(int i=0; i<loc.methods.size(); i++) {
				if(loc.methods[i].compare("POST") == 0) {
					if(!loc.cgi_ext.empty())
						return(errRes(401)); ///cgi ile degistir
					else if(loc.redirect_cd && !loc.redirect_url.empty())
						return(errRes(loc.redirect_cd));
					else if (loc.root.empty())
						return(errRes(404));
					else {
						int cd = writeContent(reqBody, reqUri);
						if(cd != 201)
							return(errRes(cd));
						for(int i=0; i<loc.index.size(); i++) {
							std::string filename = loc.root + "/" + loc.index[i];
							if (fileExists(filename.c_str())) {
								std::string res = "HTTP/1.1 ";
									res += "201 Created\r\n";
									res += "Content-Type: text/html; charset=UTF-8\r\n";
									res += "\r\n";
									res += read_html_file(filename);
								return (res);
							}
						}
					}
			}
			return(errRes(405));
			}
		}
	}
	return (errRes(404));
}

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
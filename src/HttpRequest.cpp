#include "../inc/HttpRequest.hpp"

// =====================DAN==============================================
HttpRequest::HttpRequest() {}
HttpRequest::~HttpRequest(void) {}
std::string HttpRequest::find(const std::string& buff, std::string lookfor, std::string end) {
	std::string contentType;
	// Find the start of the Content-Type header
	std::string contentTypeHeaderStart = lookfor;
	std::string::size_type contentTypeHeaderPos = buff.find(contentTypeHeaderStart);
	if (contentTypeHeaderPos != std::string::npos) {
		// Extract the value of the Content-Type header
		std::string contentTypeHeaderValue = buff.substr(contentTypeHeaderPos + contentTypeHeaderStart.length());
		std::string::size_type contentTypeHeaderValueEndPos = contentTypeHeaderValue.find(end);
		if (contentTypeHeaderValueEndPos != std::string::npos) {
			contentType = contentTypeHeaderValue.substr(0, contentTypeHeaderValueEndPos);
		}
	}
	return contentType;
}

int hexToDec(const std::string& hexStr) {
    int decimal = 0;
    for (size_t i = 0; i < hexStr.length(); ++i) {
        char c = hexStr[i];
        int digit = 0;
        if (c >= '0' && c <= '9') {
            digit = c - '0';
        } else if (c >= 'a' && c <= 'f') {
            digit = c - 'a' + 10;
        } else if (c >= 'A' && c <= 'F') {
            digit = c - 'A' + 10;
        } else {
            // invalid hexadecimal character
            return -1;
        }
        decimal += digit * std::pow(16, hexStr.length() - i - 1);
    }
    return decimal;
}

   
bool HttpRequest::isComplete(std::string buff) {
        if (buff.find("multipart/form-data") != std::string::npos && buff.find("--\r\n") == std::string::npos)
            return false;
	if (buff.find("Content-Length: ") != std::string::npos) {

		size_t contentLength = std::stoi(find(buff, "Content-Length: ", "\r\n"));
        // if (contentLength > 4096)
        //     return true;
		size_t headerLength = buff.find("\r\n\r\n") + 4;
		if (contentLength + headerLength > buff.size())
			return false;
	}
    else if (buff.find("Transfer-Encoding: chunked") != std::string::npos && buff.find("0\r\n\r\n") == std::string::npos ) {
        return false;
    }
	else if (buff.find("\r\n\r\n") == std::string::npos)
		return false;
	return true;
}


// =========================IDIL============================================

HttpRequest::HttpRequest(std::string buff, std::string port) : _buff(buff), _port(port) {
	parseHeader();
	parseBody();
    // std::cout << "method " << _method << std::endl;
    // std::cout << "uri path " << _uri.path << std::endl;
    // std::cout << "uri qs" << _uri.queryStr << std::endl;
    // std::cout << "uri protocol" << _uri.protocol << std::endl;
    // std::cout << "port " << _port << std::endl;
    // std::cout << "version " << _version << std::endl;
    // std::cout << "body " << _body << std::endl;
    // std::cout << "body " << _headers["User-Agent"] << std::endl;
}

void HttpRequest::parseHeader() {
	// parse first line
	std::string firstLine = getFirstLine();
	std::vector<std::string> tokensFirtLine = split(firstLine, ' ');
	this->_method = tokensFirtLine[0]; 
	this->_uri = parseUri(tokensFirtLine[1]);
	this->_version = tokensFirtLine[2];
	// parse other lines and put them in to map<str,str>
	std::vector<std::string> lines = getLines();
	for (std::vector<std::string>::iterator it = lines.begin() + 1; it != lines.end(); ++it) {
        if (it->compare("\r\n\r\n") == -1)
            break ;
        std::string::size_type pos = it->find(": ");
        if (pos != std::string::npos) {
            std::string headerName = it->substr(0, pos);
            std::string headerValue = it->substr(pos + 2);
            this->_headers[headerName] = headerValue;
        }
    }
    if (_headers["Content-Type"].find("multipart/form-data") != std::string::npos) {
        multipartFormData = true;
    }
}

std::string parseChunked(std::string body) {
	std::string newBody;
	while (body.compare("0\r\n\r\n") != 0) {
		size_t hex_end = body.find("\r\n");
		std::string hex = body.substr(0, hex_end);
		int length = hexToDec(hex);
		newBody.append(body.substr(hex_end + 2, length));
		body = body.substr(hex_end + length + 2 + 2);
	}
	return newBody;
}

void HttpRequest::parseBody() {
	// Find the start of the request body
	size_t body_start = _buff.find("\r\n\r\n");
	if (body_start != std::string::npos)
		body_start += 4;
	// Extract the request body by checking content length
	if (_buff.find("Content-Length:") != std::string::npos) {
		size_t length_start = _buff.find("Content-Length:") + 16;
		size_t length_end = _buff.find("\r\n", length_start);
		std::string length_str = _buff.substr(length_start, length_end - length_start);
		int content_length = std::atoi(length_str.c_str());
		if (content_length > 0)
			_body = _buff.substr(body_start, content_length);
	}
    if (_buff.find("Transfer-Encoding: chunked") != std::string::npos && _buff.find("\r\n\r\n0\r\n\r\n") == std::string::npos){
        _body = parseChunked(_buff.substr(body_start));
    }
}

// http_URL = "http:" "//" host [ ":" port ] [ abs_path [ "?" query ]]
// https://www.example.com/path/to/resource?param1=value1&param2=value2#section1
Uri HttpRequest::parseUri(std::string token) {
    Uri uri;
    if (token.length() == 0)
        return uri;
    std::string::iterator uriEnd = token.end();
    std::string::iterator queryStart = std::find(token.begin(), uriEnd, '?');
    std::string::iterator protocolStart = token.begin();
    std::string::iterator protocolEnd = std::find(protocolStart, uriEnd, ':');
    if (protocolEnd != uriEnd) {
        std::string prot = std::string(protocolEnd, protocolEnd + 3);
        if (prot == "://") {
            uri.protocol = std::string(protocolStart, protocolEnd);
            protocolEnd += 3;
        } else {
            protocolEnd = token.begin();
        }
    } else {
        protocolEnd = token.begin();
    }
    std::string::iterator hostStart = protocolEnd;
    std::string::iterator pathStart = std::find(hostStart, uriEnd, '/');
    std::string::iterator hostEnd = std::find(protocolEnd, (pathStart != uriEnd) ? pathStart : queryStart, ':');
    uri.host = std::string(hostStart, hostEnd);
    if ((hostEnd != uriEnd) && (*(hostEnd++) == ':')) {
        std::string::iterator portEnd = (pathStart != uriEnd) ? pathStart : queryStart;
        uri.port = std::string(hostEnd, portEnd);
    }
    if (pathStart != uriEnd)
        uri.path = std::string(pathStart, queryStart);
    if (queryStart != uriEnd)
        uri.queryStr = std::string(queryStart + 1, uriEnd);
    return uri;
}

void HttpRequest::setUriPath(std::string path) {
    _uri.path = path;
}

Uri HttpRequest::getUri() {return _uri;}
std::string HttpRequest::getMethod() {return _method;}
std::string HttpRequest::getVersion() {return _version;}
std::string HttpRequest::getBody() {return _body;}
std::string HttpRequest::getQueryStr() {return _uri.queryStr;}
std::string HttpRequest::getPort() {return _port;}
std::map<std::string, std::string> HttpRequest::getHeaders() {return _headers;}

std::string HttpRequest::getFirstLine() {
    size_t pos = _buff.find("\r\n");
    if (pos == std::string::npos) {
        return _buff; // return the whole message if there are no line breaks
    }
    return _buff.substr(0, pos);
}

std::string trim(const std::string& s) {
    std::string::const_iterator left = s.begin();
    while (left != s.end() && std::isspace(*left)) {
        ++left;
    }
    std::string::const_reverse_iterator right = s.rbegin();
    while (right != s.rend() && std::isspace(*right)) {
        ++right;
    }
    return std::string(left, right.base());
}

std::vector<std::string> HttpRequest::getLines() {
    std::vector<std::string> lines;
    std::istringstream iss(_buff);
    std::string line;
    while (std::getline(iss, line, '\r')) {
        if (!line.empty() && line[line.length()-1] == '\n')
            line.erase(line.length()-1);
        lines.push_back(trim(line)); // Remove leading/trailing whitespace and add to vector
    }
    return lines;
}

std::vector<std::string> split(std::string str, char delimiter) {
    std::vector<std::string> tokens;
    std::istringstream iss(str);
    std::string token;
    while (getline(iss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

std::vector<std::string> splitFromCrlf(std::string str)
{
	std::string delimiters = "\r\n";
	std::vector<std::string> tokens;
	size_t pos, lastPos = 0, length = str.length();

	while (lastPos < length + 1)
	{
		pos = str.find_first_of(delimiters, lastPos);
		if (pos == std::string::npos)
			pos = length;
		if (pos != lastPos)
			tokens.push_back(std::string(str.data() + lastPos, pos - lastPos));
		lastPos = pos + 1;
	}
	return (tokens);
}


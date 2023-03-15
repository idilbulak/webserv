#include "../inc/HttpRequest.hpp"

HttpRequest::HttpRequest(std::string buff) {
        this->_body = findBody(buff);
		this->_type = find(buff, "Content-Type:", ";");
		// 	this->_boundry = find(buff, "boundary=", "\n");
		// 	// this->_content = getContentBetweenBoundaries(_body, _boundry);
		if (_type.compare(" multipart/form-data") == 0)
			parseBody();
        std::vector<std::string> splitBuff = split_crlf(buff);
        this->_method = split(splitBuff[0], " ")[0];
        findUri(split(splitBuff[0], " ")[1]);
		if (_method.compare("DELETE") == 0)
    	    findFileToDelete();
        this->_host= split(split(splitBuff[1], " ")[1], ":")[0]; //burda sikinti var
        std::vector<std::string> findPort = split(splitBuff[1], ":");
        if (findPort.size() == 3)
            this->_port = findPort[2];
        else
            this->_port = "80";
        // std::cout << "HttpRequest:\nmethod:\t" << this->_method <<  "\nhost:\t" << _host << std::endl; 
		// std::cout << "port:\t" << _port << "\nurl:\t" << _uri << "\nbody:\t" << _queryStr << std::endl;
		// std::cout << "type:\t" << _type << "\nboundry:\t" << _boundry << "\ncontent:\t" << _content << std::endl;
}

HttpRequest::~HttpRequest(void) {}

std::vector<std::string> HttpRequest::split(std::string str, std::string delimiter) {
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

std::vector<std::string> HttpRequest::split_crlf(std::string str) {
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

std::string HttpRequest::findBody(std::string buff) {
    std::string body;
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
        if (content_length > 0)
            body = buff.substr(body_start, content_length);
	}
    return body;
}

void HttpRequest::parseBody() {
	std::string boundary = _body.substr(0, _body.find('\n'));
    size_t startPos = _body.find('\n') + 1; // Skip the boundary and newline characters
    // Find the Content-Disposition header and extract the name and filename values
    std::string contentDispositionHeader = "Content-Disposition: form-data; name=\"";
    size_t dispositionPos = _body.find(contentDispositionHeader, startPos);
    if (dispositionPos == std::string::npos) {
        return; // Content-Disposition header not found
    }
    _cntDisposition = "form-data"; // Set the disposition to form-data
    size_t namePos = dispositionPos + contentDispositionHeader.length();
    size_t nameEndPos = _body.find('"', namePos);
    _cntName = _body.substr(namePos, nameEndPos - namePos);
    size_t filenamePos = _body.find("filename=\"", nameEndPos);
    if (filenamePos != std::string::npos) {
        size_t filenameEndPos = _body.find('"', filenamePos + 10); // Skip "filename="
        _cntFileName = _body.substr(filenamePos + 10, filenameEndPos - filenamePos - 10);
    }
    // Find the Content-Type header and extract the type value
    std::string contentTypeHeader = "\nContent-Type: ";
    size_t typePos = _body.find(contentTypeHeader, startPos);
    if (typePos != std::string::npos) {
        size_t typeEndPos = _body.find('\n', typePos + contentTypeHeader.length());
        _cntType = _body.substr(typePos + contentTypeHeader.length(), typeEndPos - typePos - contentTypeHeader.length());
    }
	size_t start = _body.find("\r\n\r\n");
    if (start != std::string::npos) {
        start += 4;  // move past the empty lines
        size_t end = _body.find("\r\n------", start);
        if (end != std::string::npos) {
            _content = _body.substr(start, end - start);
        }
    }
	// std::cout << _content << "|" << std::endl;
	// std::cout << _cntDisposition << _cntFileName << _cntType << std::endl;
}

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

// "/search.cgi?q=python&category=tutorials&sort=date"
void HttpRequest::findUri(std::string str) {
    std::size_t queryPos = str.find('?');
    _queryStr = str.substr(queryPos + 1);
    _uri = str.substr(0, queryPos);
    // std::cout << "Script name: " << reqUri << '\n';
    // std::cout << "Query string: " << queryString << '\n';
}

void HttpRequest::findFileToDelete() {
    std::size_t pos = _queryStr.find('=');
    if (pos == std::string::npos) {
        // If there is no equal sign, return an empty string
        _fileToDelete =  "";
    }
    // Extract the substring that comes after the equal sign
    _fileToDelete = _queryStr.substr(pos + 1);
  }

std::string HttpRequest::getContentBetweenBoundaries(std::string &input, std::string &boundary) {
    std::string content;
    // Find the starting boundary
    size_t startPos = input.find(boundary);
    if (startPos == std::string::npos) {
        return content; // Boundary not found
    }
    startPos += boundary.length() + 2; // Skip the boundary and newline characters
    // Find the ending boundary
    size_t endPos = input.find(boundary, startPos);
    if (endPos == std::string::npos) {
        return content; // Ending boundary not found
    }
    // Extract the content between the boundaries
    content = input.substr(startPos, endPos - startPos);
    return content;
}

std::string HttpRequest::getPort() {return _port;}
std::string HttpRequest::getUri() {return _uri;}
std::string HttpRequest::getMethod() {return _method;}
std::string HttpRequest::getBody() {return _body;}
std::string HttpRequest::getContent() {return _content;}
std::string HttpRequest::getCntDisposition() {return _cntDisposition;}
std::string HttpRequest::getCntType() {return _cntType;}
std::string HttpRequest::getCntName() {return _cntName;}
std::string HttpRequest::getCntFileName() {return _cntFileName;}
std::string HttpRequest::getFileToDelete() {return _fileToDelete;}
std::string HttpRequest::getQueryStr() {return _queryStr;}
std::string HttpRequest::getVersion() {return _version;}


#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <stdexcept>

// std::vector<std::string> splitFromCrlf(std::string str)
// {
// 	std::string delimiters = "\r\n";
// 	std::vector<std::string> tokens;
// 	size_t pos, lastPos = 0, length = str.length();

// 	while (lastPos < length + 1)
// 	{
// 		pos = str.find_first_of(delimiters, lastPos);
// 		if (pos == std::string::npos)
// 			pos = length;
// 		if (pos != lastPos)
// 			tokens.push_back(std::string(str.data() + lastPos, pos - lastPos));
// 		lastPos = pos + 1;
// 	}
// 	return (tokens);
// }

// std::string find(const std::string& buff, std::string lookfor, std::string end) {
// 	std::string contentType;
// 	// Find the start of the Content-Type header
// 	std::string contentTypeHeaderStart = lookfor;
// 	std::string::size_type contentTypeHeaderPos = buff.find(contentTypeHeaderStart);
// 	if (contentTypeHeaderPos != std::string::npos) {
// 		// Extract the value of the Content-Type header
// 		std::string contentTypeHeaderValue = buff.substr(contentTypeHeaderPos + contentTypeHeaderStart.length());
// 		std::string::size_type contentTypeHeaderValueEndPos = contentTypeHeaderValue.find(end);
// 		if (contentTypeHeaderValueEndPos != std::string::npos) {
// 			contentType = contentTypeHeaderValue.substr(0, contentTypeHeaderValueEndPos);
// 		}
// 	}
// 	return contentType;
// }

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

std::string parseChunked(std::string buff) {
	std::string _body;
	std::string _newBody;
	size_t body_start = buff.find("\r\n\r\n");
	if (body_start != std::string::npos)
		body_start += 4;
	_body = buff.substr(body_start);
	while (_body.compare("0\r\n") != 0) {
		size_t hex_end = _body.find("\r\n");
		std::string hex = _body.substr(0, hex_end);
		int length = hexToDec(hex);
		_newBody.append(_body.substr(hex_end + 2, length));
		_body = _body.substr(hex_end + length + 2);
	}
	return _newBody;
}

int main() {
    std::string raw_request =
        "POST /example HTTP/1.1\r\n"
        "Host: www.example.com\r\n"
        "Content-Type: application/x-www-form-urlencoded\r\n"
        "Transfer-Encoding: chunked\r\n"
        "Connection: keep-alive\r\n"
        "User-Agent: Mozilla/5.0 (compatible; ExampleClient/1.0)\r\n"
        "\r\n"
        "4\r\ndata\r\n"
        "4\r\ndata\r\n"
        "0\r\n";

    try {
        raw_request = parseChunked(raw_request);
    	std::cout << raw_request << std::endl;
    } catch (const std::runtime_error &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}

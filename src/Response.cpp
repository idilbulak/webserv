#include "../inc/Response.hpp"
// #include "Request.hpp"

// 200 OK: The request was successful, and the server is returning the requested data.
// 201 Created: The request has been fulfilled, and a new resource has been created.
// 204 No Content: The request was successful, but there is no data to return.
// 301 Moved Permanently: The requested resource has been moved permanently to a new location.
// 302 Found: The requested resource has been temporarily moved to a new location.
// 400 Bad Request: The request was malformed or invalid.
// 401 Unauthorized: The request requires authentication, and the client is not authenticated.
// 403 Forbidden: The server is refusing to fulfill the request, usually because the client does not have permission to access the resource.
// 404 Not Found: The requested resource could not be found on the server.
// 500 Internal Server Error: An error occurred on the server while processing the request.

Response::Response() {
}
Response::~Response() {
}

// Read the contents of an HTML file into a string
std::string Response::read_html_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return content;
}

std::string Response::statuscode(int cd) {
    std::string code;
    if (cd == 200)
        code = "200 OK";
    if (cd == 201)
        code = "201 Created";
    if (cd == 204)
        code = "204 No Content";
    if (cd == 301)
        code = "301 Moved Permanently";
    if (cd == 302)
        code = "302 Found";
    if (cd == 400)
        code = "400 Bad Request";
    if (cd == 401)
        code = "401 Unauthorized";
    if (cd == 403)
        code = "403 Forbidden";
    if (cd == 404)
        code = "404 Not Found";
    if (cd == 405)
        code = "405 Method Not Allowed";
    if (cd == 500)
        code = "500 Internal Server Error";
    return code;
}

void Response::errResponse(int err) {
    this->_statusCode = err;
    const std::string& filename = "Conf/html/" + std::to_string(err) + ".html";
    _res = "HTTP/1.1 ";
        _res += statuscode(err) + CRFL;
		_res += "Content-Type: text/html; charset=UTF-8\r\n";
		_res += "\r\n";
		_res += read_html_file(filename);
}

void Response::getResponse(int err) {
    this->_statusCode = err;
    const std::string& filename = "Conf/html/" + std::to_string(err) + ".html";
    _res = "HTTP/1.1 ";
        _res += statuscode(err) + CRFL;
		_res += "Content-Type: text/html; charset=UTF-8\r\n";
		_res += "\r\n";
		_res += read_html_file(filename);
}

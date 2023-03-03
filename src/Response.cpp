#include "../inc/Response.hpp"

Response::Response(Config cf) :_cf(cf) {
}

Response::~Response(void) {
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

std::string Response::errRes(int err) {

	this->_statusCode = err;
    const std::string& filename = "err_html/" + std::to_string(err) + ".html";
    std::string res = "HTTP/1.1 ";
        res += statuscode(err) + CRFL;
		res += "Content-Type: text/html; charset=UTF-8\r\n";
		res += "\r\n";
		res += read_html_file(filename);
	return (res);
}

std::string Response::statuscode(int cd) {
    std::map<int, std::string> codes = {
		{100,"100 Continue"},
        {101,"101 Switching Protocols"},
        {200,"200 OK"},
        {201,"201 Created"},
        {202,"202 Accepted"},
        {203,"203 Non-Authoritative Information"},
        {204,"204 No Content"},
    	{205,"205 Reset Content"},
		{300,"300 Multiple Choices"},
        {301,"301 Moved Permanently"},
        {302,"302 Found"},
        {303,"303 See Other"},
        {305,"305 Use Proxy"},
        {306,"306 (Unused)"},
        {307,"307 Temporary Redirect"},
        {400, "400 Bad Request"},
        {401, "401 Unauthorized"},
        {402, "402 Payment Required"},
        {403, "403 Forbidden"},
        {404, "404 Not Found"},
        {405, "405 Method Not Allowed"},
        {408, "408 Request Timeout"},
        {409, "409 Conflict"},
        {410, "410 Gone"},
        {413, "413 Payload Too Large"},
        {414, "414 URI Too Long"},
        {415, "415 Unsupported Media Type"},
        {429, "429 Too Many Requests"},
        {500, "500 Internal Server Error"},
        {501, "501 Not Implemented"},
        {502, "502 Bad Gateway"},
        {503, "503 Service Unavailable"},
        {504, "504 Gateway Timeout"},
        {505, "505 HTTP Version Not Supported"},
    };
	return codes[cd];
}
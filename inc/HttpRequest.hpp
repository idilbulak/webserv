#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <vector>
#include <map>
#include "Config.hpp"
#include <cmath>

#define CRLF "\r\n"

struct Uri {
	std::string	fullPath;
	std::string	path;
	std::string	queryStr;
	std::string	protocol;
	std::string	host;
	std::string	port;
	Uri() : fullPath(""), path(""), queryStr(""), protocol(""), host(""), port("") {}
};

class HttpRequest {
	public:
		HttpRequest();
		HttpRequest(std::string buff, std::string port);
		~HttpRequest(void);

		void						parseHeader();
		void						parseBody();
		Uri							parseUri(std::string token);
		void    					parseMultiData();
		bool						isComplete(std::string buff);
		std::string 				getFirstLine();
		std::string 				find(const std::string& buff, std::string lookfor, std::string end);
		std::vector<std::string>	getLines();
		std::string					getMethod();
		std::string					getPort();
		std::string					getVersion();
		std::string					getBody();
		std::string					getQueryStr();
		Uri							getUri();
		void						setUriPath(std::string path);
		std::map<std::string, std::string>	getHeaders();

	private:
		std::string		_buff;
		std::string		_method;
		std::string		_port;
		std::string		_version;
		std::string		_body;
		Location        _loc;
		Uri				_uri;
		bool			multipartFormData;
		std::map<std::string, std::string> 	_headers;
};

std::vector<std::string> split(std::string str, char delimiter);
std::vector<std::string> splitFromCrlf(std::string str);

#endif
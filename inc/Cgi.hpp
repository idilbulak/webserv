#ifndef CGI_HPP
# define CGI_HPP

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
#include "Time.hpp"

// #define CRFL "\r\n"
// #define SIZE 65536 //64kilobytes

// class HttpRequest {
// 	public:
// 		HttpRequest(std::string buff);
// 		~HttpRequest(void);
		
// 		// VirtualServer	findServer(Config cf);
// 		// std::string	response(Config cf);
// 		// void	setFilePath();
// 		// bool	findLocation(std::string reqUri, Location* loc);
// 		// bool	ValidMethod(Location loc, std::string method);
// 		// bool	fileExists(const char* filename);
// 		// bool	endsWithSlash(const std::string& str);
// 		// bool	checkIndx();
// 		// bool	hasCgiExtension(const std::string& filename);
// 		std::string	getPort();
// 		std::string	getUri();
// 		std::string	getMethod();
		
// 		std::string findBody(std::string buff);
// 		std::vector<std::string> split_crlf(std::string str);
// 		std::vector<std::string> split(std::string str, std::string delimiter);

		
// 		// std::string	errRes(int err);
// 		// std::string	cgiRes();
// 		// std::string	getRes();
// 		// std::string	htmlRes();
// 		// std::string	read_html_file(const std::string& fileName);
// 		// std::string	postRes();
// 		// std::string	statuscode(int cd);
// 		// std::string	generate();
// 		// void		makeCodeMap();
// 		// std::string 	executeCgi();
// 		// void	envCgi();

// 	private:
// 		std::string _method;
// 		std::string _host;
// 		std::string _port;
// 		std::string _uri;
// 		std::string _version;
// 		std::string _body;
// 		// VirtualServer   _server;
// 		// Location        _loc;
// 		// std::string     _indxFile;
// 		// int			    _cgiOn;
// 		// HttpRequest      _req;
// 		// Response        _res;
// 		// std::map<int, std::string> _codeMap;
// 		// std::map<std::string, std::string> _env;
// };

#endif
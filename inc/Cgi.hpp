#pragma once

#include <iostream>
#include <sstream>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <vector>
#include <map>
#include "Response.hpp"

#define SIZE 65536 //64kilobytes

class Response;

class Cgi {
	public:
		Cgi(std::string file, Response &response);
		~Cgi(void);
		std::string 	execute();
		void	envCgi();
				std::string itos(int num);

	private:
		std::map<std::string, std::string> _env;
		HttpRequest      _req;
		VirtualServer		_server;
		std::string     _indxFile;
		Response     &_response;
		// std::string _body; //req.body
// 		std::string _method;
// 		std::string _host;
// 		std::string _port;
// 		std::string _uri;
// 		std::string _version;
// 		// VirtualServer   _server;
// 		// Location        _loc;
// 		// int			    _cgiOn;
// 		// Response        _res;
// 		// std::map<int, std::string> _codeMap;
};

// #endif
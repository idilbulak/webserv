#ifndef CGI_HPP
# define CGI_HPP

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
#include "HttpRequest.hpp"

#define SIZE 65536 //64kilobytes

class Cgi {
	public:
		Cgi(std::string file, HttpRequest req);
		~Cgi(void);
		std::string 	execute();
		void	envCgi();

	private:
		std::map<std::string, std::string> _env;
		HttpRequest      _req;
		std::string     _indxFile;
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

#endif
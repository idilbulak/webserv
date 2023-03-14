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
#include "Time.hpp"

#define CRFL "\r\n"


class HttpRequest {
	public:
		HttpRequest(std::string buff);
		~HttpRequest(void);
		
		std::string	getPort();
		std::string	getUri();
		std::string	getMethod();
		std::string	getBody();
		std::string getContent();
		std::string getCntDisposition();
		std::string getCntType();
		std::string getCntName();
		std::string getCntFileName();
		std::string getFileToDelete();
		std::string getQueryStr();
		std::string getVersion();

		
		std::string findBody(std::string buff);
		void		findUri(std::string str);
		void		findFileToDelete();
		void parseBody();
		std::string find(const std::string& buff, std::string lookfor, std::string end);
		std::string getContentBetweenBoundaries(std::string &str, std::string &boundary);
		std::vector<std::string> split_crlf(std::string str);
		std::vector<std::string> split(std::string str, std::string delimiter);

	private:
		std::string _method;
		std::string _host;
		std::string _port;
		std::string _uri;
		std::string _version;
		std::string _type;
		std::string	_queryStr;
		std::string _boundry;
		std::string _body;
		std::string _content;
		std::string _cntDisposition;
		std::string _cntType;
		std::string _cntName;
		std::string _cntLength;
		std::string _cntFileName;
		std::string _fileToDelete;
		// VirtualServer   _server;
		// Location        _loc;
		// std::string     _indxFile;
		// int			    _cgiOn;
		// HttpRequest      _req;
		// Response        _res;
		// std::map<int, std::string> _codeMap;
		// std::map<std::string, std::string> _env;
};

#endif
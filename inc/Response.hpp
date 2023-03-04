#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <iostream>
#include <sys/stat.h>
#include <map>
#include "Config.hpp"

#define CRFL "\r\n"

class Response {

	public:
		Response(VirtualServer server);
		~Response(void);

		std::string	res(std::string version, int code, std::string type, std::string filename);
		std::string read_html_file(const std::string& fileName);
		std::string errRes(int err);
		std::string findValidFile(Location loc);
		bool hasValidMethod(Location loc, std::string method);
		bool findLocation(std::string reqUri, Location* loc);
		std::string getRes(std::string reqUri);
		std::string postRes(std::string reqUri, std::string reqBody);
		std::string statuscode(int cd);


	private:
		VirtualServer	_server;
		int				_statusCode;
		std::string		_res;
		std::string		_resVersion;
		std::string		_resCode;
		std::string		_resType;
		std::string		_resFile;
		
};

#endif
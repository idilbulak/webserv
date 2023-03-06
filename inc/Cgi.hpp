#ifndef CGI_HPP
# define CGI_HPP

#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <map>
#include <vector>
#include <sys/types.h>
#include <sys/wait.h>
#include "Config.hpp"
#include "Response.hpp"
#include "Time.hpp"

#define SIZE 65536 //64kilobytes
#define RED "\033[31m"
#define CYAN "\033[0;36m"
#define BLACK "\033[0;30m"
#define RESET "\033[0m"

class Cgi {

	public:
		Cgi(VirtualServer server, Location loc, std::string body, std::string method);
		~Cgi(void);

		std::string execute();

	private:
		std::map<std::string, std::string> _env;
		VirtualServer _server;
		std::string _body;
		std::string _cgiPath;
		

};



#endif
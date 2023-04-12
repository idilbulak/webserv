#ifndef SOCKET_HPP
# define SOCKET_HPP

#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/event.h>
#include <fcntl.h>
#include "Time.hpp"
#include "Config.hpp"

#define BLACK "\033[0;30m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[0;33m"
#define BLUE "\033[0;34m"
#define PURPLE "\033[0;35m"
#define CYAN "\033[0;36m"
#define RESET "\033[0m"
// #define ERROR std::perror
// #define BACKLOG 3
// #define TEST std::cout << " TEST is HERE!!!" << std::endl;


class Socket {

	public:
		Socket(std::string host, std::string port);
		Socket();
		~Socket(void);

		void createSocket();
		void setSocketAddr();
		void setFiledOptions(int filed);
		void bind();
		void listen();
		int accept();

		std::string getPort() {return _port;}
		int getfd() {return _fd;}
		struct sockaddr_in getAddr() {return _addr;}

		class AcceptFail : public std::exception {
			public:
				const char* what() const throw() {
					return "[ERROR] Accept() failed";
				}
		};

		class CreateSocketFail : public std::exception {
			public:
				const char* what() const throw() {
					return "[ERROR] Socket() failed";
				}
		};

	private:
		int _fd;
		std::string _host;
		std::string _port;
		struct sockaddr_in _addr;
		socklen_t _addrlen;
};

std::ostream& operator<<(std::ostream &os, Socket& obj);

#endif
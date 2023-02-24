#ifndef SOCKET_HPP
# define SOCKET_HPP

#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/event.h>
#include <fcntl.h>
#include "Time.hpp"

#define RED "\033[31m"
#define CYAN "\033[0;36m"
#define BLACK "\033[0;30m"
#define RESET "\033[0m"
#define ERROR std::perror
#define BACKLOG 3

class Socket {

	public:
		Socket(std::string host, std::string port);
		Socket();
		~Socket(void);

		void configure();
		void bind();
		void listen();
		// int	 close();

		int	accept(struct kevent& event);
		// int	send();
		// int	recv();

		std::string getHost() {return _host;}
		std::string getPort() {return _port;}
		int getfd() {return _fd;}
		struct sockaddr_in getAddr() {return _addr;}
		socklen_t getAddrlen() {return _addrlen;}

	private:
		std::string			_host;
		std::string			_port;
		int					_fd;
		struct sockaddr_in	_addr;
		socklen_t			_addrlen;
};

std::ostream& operator<<(std::ostream &os, Socket& obj);


#endif
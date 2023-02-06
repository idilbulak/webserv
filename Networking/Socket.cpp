#include "Socket.hpp"

Socket::Socket(std::string host, std::string port) :_host(host), _port(port) {}
Socket::~Socket() {}
std::string	Socket::getHost() {return _host;}
std::string	Socket::getPort() {return _port;}
int Socket::getSocketfd() {return _serverfd;}

void	Socket::create() {
	if (( _serverfd = socket( AF_INET, SOCK_STREAM, 0)) < 0) {
		std::cout << "cannot create socket: " << std::string(strerror(errno)) << std::endl;
		exit(0);
	}
}

void Socket::identify() {
    struct sockaddr_in address;
    const int port = 8080;
    memset((char *)&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port);
	if (bind(_serverfd, (struct sockaddr *)&address, sizeof(address)) < 0 ) {
		std::cout << "socket bind failed: " << std::string(strerror(errno)) << std::endl;
		close( _serverfd );
		exit(0);
	}
}

void Socket::wait_incomingconnections() {
	if (listen(_serverfd, BACKLOG) < 0) {
		std::cout << "socket listen failed: " << std::string(strerror(errno)) << std::endl;
		exit(0);
	}
}


#include "../inc/Socket.hpp"

Socket::Socket(std::string host, std::string port) :_host(host), _port(port) {
	create();
	identify();
	wait_incomingconnections();
}

Socket::~Socket() {}
std::string	Socket::getHost() {return _host;}
std::string	Socket::getPort() {return _port;}
int Socket::getServerfd() {return _serverfd;}

void	Socket::create() {
	if (( _serverfd = socket( AF_INET, SOCK_STREAM, 0)) < 0) {
		std::cout << "cannot create socket: " << std::string(strerror(errno)) << std::endl;
		exit(1);
	}
}

void Socket::identify() {
    struct sockaddr_in servaddr;
    const int port = std::stoi(_port);
    memset((char *)&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);
	// servaddr.sin_zero = memset()
	// kullanimda olan varsa kapamak icin
	int optval = 1;
	setsockopt(_serverfd,SOL_SOCKET,SO_REUSEADDR,&optval,sizeof(optval));
	if (bind(_serverfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 ) {
		std::cout << "socket bind failed: " << std::string(strerror(errno)) << std::endl;
		close( _serverfd );
		exit(1);
	}
}

void Socket::wait_incomingconnections() {
	if (listen(_serverfd, BACKLOG) < 0) {
		std::cout << "socket listen failed: " << std::string(strerror(errno)) << std::endl;
		exit(1);
	}
}

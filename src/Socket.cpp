#include "../inc/Socket.hpp"

Socket::Socket() {

}

Socket::Socket(std::string host, std::string port) :_host(host), _port(port) {

	configure();
	bind();
	listen();
}

void Socket::configure() {

	if (( _fd = socket( AF_INET, SOCK_STREAM, 0)) < 0) {
		std::cout << "cannot create socket: " << std::string(strerror(errno)) << std::endl;
		// ERROR("cannot create socket");
		exit(1);
	}
	const int port = std::stoi(_port);
	memset((char *)&_addr, 0, sizeof(_addr));
	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	_addr.sin_port = htons(port);
	
	int optval = 1; //?????
	setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)); //????
	setsockopt(_fd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)); //????

	// setting FD to NON-BLOCKING
	fcntl(_fd, F_SETFL, O_NONBLOCK);
}

void Socket::bind() {

	if (::bind(_fd, (struct sockaddr *)&_addr, sizeof(_addr)) < 0 ) {
		std::cout << "socket bind failed: " << std::string(strerror(errno)) << std::endl;
		close( _fd );
		exit(1);
	}
}

void Socket::listen() {

	if (::listen(_fd, BACKLOG) < 0) {
		std::cout << "socket listen failed: " << std::string(strerror(errno)) << std::endl;
		exit(1);
	}
}

int Socket::accept() {

	int connectionSocket = ::accept(_fd, NULL, NULL);
	if (connectionSocket < 0) {
		ERROR("socket accept failed");
		return -1;
	}
	fcntl(connectionSocket, F_SETFL, O_NONBLOCK);
	return connectionSocket;
}

// int Socket::accept(struct kevent& event) {

// 	_addrlen = sizeof(_addr);
// 	_fd = ::accept(event.ident, (struct sockaddr *)&_addr, (socklen_t *)&_addrlen); //_addrlen not initiated??
// 	if (_fd < 0)
// 		ERROR("accepting new client connection");
// 	else
// 		fcntl(_fd, F_SETFL, O_NONBLOCK);
// 	std::cout << RED << getTime() << RESET << *this << "\tConnecting... " << std::endl;
// 	return _fd;
// }

// int Socket::recv(struct kevent& event) {

// 	int num_bytes = ::recv(event.ident, _buffer, sizeof(_buffer), 0);
// 	if (num_bytes == -1) {
// 		ERROR("recv");
// 		close(event.ident);
// 		return -1;
// 	}
// 	_httpRequest.append(_buffer, num_bytes);
// 	return 0;
// }

std::ostream& operator<<(std::ostream &os, Socket& obj) {

	os << "port: " << CYAN << ntohs(obj.getAddr().sin_port) << RESET
	<< " \tIP address: " << inet_ntoa(obj.getAddr().sin_addr);
	return os;
}

Socket::~Socket() {

}
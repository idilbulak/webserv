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
		exit(1);
	}
	const int port = std::stoi(_port);
	memset((char *)&_addr, 0, sizeof(_addr));
	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	_addr.sin_port = htons(port);
	int optval = 1;
	setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
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

int Socket::accept(struct kevent& event) {

	_fd = ::accept(event.ident, (struct sockaddr *)&_addr, &_addrlen); //_addrlen not initiated??
	if (_fd < 0) {
		ERROR("accepting new client connection");
		return _fd;
	}
	std::cout << RED << getTime() << RESET << *this << "\tConnecting... " << std::endl;	
	fcntl(_fd, F_SETFL, O_NONBLOCK);
	return _fd;
}

std::ostream& operator<<(std::ostream &os, Socket& obj) {

	os << "port: " << CYAN << ntohs(obj.getAddr().sin_port) << RESET
	<< " \tIP address: " << inet_ntoa(obj.getAddr().sin_addr);
	return os;
}

Socket::~Socket() {

}
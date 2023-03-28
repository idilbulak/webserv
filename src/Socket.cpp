#include "../inc/Socket.hpp"

Socket::Socket() {

}

Socket::Socket(std::string host, std::string port) :_host(host), _port(port) {

	createSocket();
	setSocketAddr();
	setFiledOptions();
	bind();
	listen();
}

void Socket::createSocket() {

	if (( _fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		ERROR("socket() failed");
		exit(1);
	}
}

void Socket::setSocketAddr() {

	const int port = std::stoi(_port);
	memset((char *)&_addr, 0, sizeof(_addr));
	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	_addr.sin_port = htons(port);
}

void Socket::setFiledOptions() {

	int optval = 1;
	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
		ERROR("setsockopt() failed");
		exit(1);
	}
	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)) == -1) {
		ERROR("setsockopt() failed");
		exit(1);
	}
	if (fcntl(_fd, F_SETFL, O_NONBLOCK) == -1) {
		ERROR("fcntl() failed");
		exit(1);
	}
}

void Socket::bind() {

	// bind socket to specific IP address and port using socket addr structure
	if (::bind(_fd, (struct sockaddr *)&_addr, sizeof(_addr)) < 0 ) {
		ERROR("bind() failed: ");
		close( _fd);
		exit(1);
	}
}

void Socket::listen() {

	if (::listen(_fd, BACKLOG) < 0) {
		ERROR("listen() failed: ");
		exit(1);
	}
}

int Socket::accept() {

	int connectionSocket = ::accept(_fd, NULL, NULL);
	if (connectionSocket < 0) {
		ERROR("accept() failed");
		return -1;
	}
	fcntl(connectionSocket, F_SETFL, O_NONBLOCK);
	return connectionSocket;
}

std::ostream& operator<<(std::ostream &os, Socket& obj) {

	os << "port: " << CYAN << ntohs(obj.getAddr().sin_port) << RESET
	<< " \tIP address: " << inet_ntoa(obj.getAddr().sin_addr);
	return os;
}

Socket::~Socket() {

}
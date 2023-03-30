#include "../inc/Socket.hpp"

Socket::Socket() {

}

Socket::Socket(std::string host, std::string port) :_host(host), _port(port) {

	if (( _fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		ERROR("socket() failed");
		exit(1);
	}
	// createSocket();
	// setSocketAddr();
	// setFiledOptions(_fd);
	// bind();
	// listen();
}

// void Socket::createSocket() {

// 	if (( _fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
// 		ERROR("socket() failed");
// 		exit(1);
// 	}
// }

void Socket::setSocketAddr() {

	const int port = std::stoi(_port);
	memset((char *)&_addr, 0, sizeof(_addr));
	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	_addr.sin_port = htons(port);
}

void Socket::setFiledOptions(int filed) {

	int optval = 1;
	if (setsockopt(filed, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
		ERROR("setsockopt() failed");
		exit(1);
	}
	if (setsockopt(filed, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)) == -1) {
		ERROR("setsockopt() failed");
		exit(1);
	}
	if (fcntl(filed, F_SETFL, O_NONBLOCK) == -1) {
		ERROR("fcntl() failed");
		exit(1);
	}
}

void Socket::bind() {

	if (::bind(_fd, (struct sockaddr *)&_addr, sizeof(_addr)) < 0 ) {
		ERROR("bind() failed: ");
		exit(1);
	}
}

void Socket::listen() {

	if (::listen(_fd, BACKLOG) < 0) { // BACKLOG??
		ERROR("listen() failed: ");
		exit(1);
	}
}

int Socket::accept() {

	int connectionSocket = ::accept(_fd, NULL, NULL);
	if (connectionSocket < 0) {
		ERROR("accept() failed");
		exit(1);
	}
	setFiledOptions(connectionSocket); //??
	return connectionSocket;
}

std::ostream& operator<<(std::ostream &os, Socket& obj) {

	os << "port: " << BLUE << std::left << std::setw(8) << ntohs(obj.getAddr().sin_port) << RESET;
	os << "IP address: " << BLUE << std::setw(12) << inet_ntoa(obj.getAddr().sin_addr) << RESET;
	return os;
}

Socket::~Socket() {
}
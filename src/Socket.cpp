#include "../inc/Socket.hpp"

Socket::Socket() {
}

Socket::Socket(std::string host, std::string port) :_host(host), _port(port) {
}

void Socket::createSocket() {

	if (( _fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		throw CreateSocketFail();
}

void Socket::setSocketAddr() {

	const int port = std::stoi(_port);
	memset((char *)&_addr, 0, sizeof(_addr));
	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	_addr.sin_port = htons(port);
}

void Socket::setFiledOptions(int filed) {

	int optval = 1;
	if (setsockopt(filed, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
		throw std::runtime_error("[ERROR] setsockopt(SO_REUSEADDR) failed");
	if (setsockopt(filed, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)) == -1)
		throw std::runtime_error("[ERROR] setsockopt(SO_REUSEPORT) failed");
	if (fcntl(filed, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error("[ERROR] fcntl(O_NONBLOCK) failed");
}

void Socket::bind() {

	if (::bind(_fd, (struct sockaddr *)&_addr, sizeof(_addr)) < 0 )
		throw std::runtime_error("[ERROR] bind() failed: ");
}

void Socket::listen() {

	if (::listen(_fd, SOMAXCONN) < 0)
		throw std::runtime_error("[ERROR] listen() failed: ");
}

int Socket::accept() {

	int connectionSocket = ::accept(_fd, NULL, NULL);
	if (connectionSocket < 0)
		throw AcceptFail();
	setFiledOptions(connectionSocket);
	return connectionSocket;
}

std::ostream& operator<<(std::ostream &os, Socket& obj) {

	os << BLUE << std::left << std::setw(10) << inet_ntoa(obj.getAddr().sin_addr);
	os << ":" << std::setw(8) << ntohs(obj.getAddr().sin_port) << RESET;
	return os;
}

Socket::~Socket() {
}
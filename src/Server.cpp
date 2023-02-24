#include "../inc/Server.hpp"

Server::Server(Config &cf) :_cf(cf) {
}

void Server::setup() {

	// create sockets, bind and listen
	for (int i = 0; i < _cf.servers.size(); i++) {
		Socket listen(_cf.servers[i].host, _cf.servers[i].port);
		_listenSockets.push_back(listen);
	}

	// create kqueue
	_kq = kqueue();
	if (_kq == -1) {
		std::cerr << "creating kqueue" << std::strerror(errno) << std::endl;
	}

	// loop over virtual servers
	for (int i = 0; i < _cf.servers.size(); i++) {

		// initialize kevent struct
		EV_SET(&_changeList, _listenSockets[i].getfd(), EVFILT_READ, EV_ADD, 0, 0, 0);

		// attach kevent to kqueue
		if (kevent(_kq, &_changeList, 1, NULL, 0, NULL) == -1) {
			ERROR("adding listen socket to kqueue");
		}
		// display on standard out
		std::cout << RED << getTime() << RESET << _listenSockets[i] << "\tListening... " << std::endl;
	}
}

void Server::run() {

	setup();
	// start kevent monitoring loop
	int	n_events;
	for(;;) {
		// retrieve triggered events // still needs timespec struct for TIMEOUT!
		n_events = kevent(_kq, NULL, 0, _eventList, EVENTS_MAX, NULL);
		if (n_events == -1) {
			ERROR("requesting new kevent");
		}
		else if (n_events > 0) {
			// loop over triggered events
			for (int i = 0; i < n_events; i++) {
				if (isListenSockfd(_eventList[i]))
					onClientConnect(_eventList[i]);
				else if (_eventList[i].flags & EV_EOF)
					onEOF(_eventList[i]);
				else if (_eventList[i].flags & EVFILT_READ)
					onRead(_eventList[i]);
			}
		}
	}
	close(_kq);
}

bool Server::isListenSockfd(struct kevent& event) {

	for (int i = 0; i < _listenSockets.size(); i++)
		if (event.ident == _listenSockets[i].getfd())
			return true;
	return false;
}

void Server::onClientConnect(struct kevent& event) {

	// create newClient socket and accept connection
	Socket newClient;
	if (newClient.accept(event) < 0)
		return;

	// add event to kqueue
	EV_SET(&_changeList, newClient.getfd(), EVFILT_READ, EV_ADD, 0, 0, NULL);
	if (kevent(_kq, &_changeList, 1, NULL, 0, NULL) == -1) {
		ERROR("adding new client to kqueue");
	}

	// create HTTP header /w message
	std::string response = "HTTP/1.1 200 OK\r\n";
				response += "Content-Type: text/html; charset=UTF-8\r\n";
				response += "\r\n";
				response += "Hello, world!\r\n";

	// send message to newClient
	send(newClient.getfd(), response.c_str(), response.size(), 0);
}

void Server::onEOF(struct kevent& event) {

	// remove event from kqueue
	EV_SET(&_changeList, event.ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
	if (kevent(_kq, &_changeList, 1, NULL, 0, NULL) < 0)
		ERROR("removing event from kqueue");

	// close file descriptor
	::close(event.ident);
	std::cout << RED << getTime() << RESET << event << "\tDisconnecting... " << std::endl;	
}

void Server::onRead(struct kevent& event) {

	// read message into buffer
	char buff[1024];
	recv(event.ident, &buff, sizeof(&buff), 0);

	// display on standard out
	std::cout << RED << getTime() << RESET << event << "\tReceiving... " << CYAN << buff << RESET  << std::endl;
}

std::ostream& operator<<(std::ostream &os, struct kevent& event) {

	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);
	getsockname(event.ident, (struct sockaddr *)&addr, &addrlen);

	os << "port: " << CYAN << ntohs(addr.sin_port) << RESET
	<< " \tIP address: " << inet_ntoa(addr.sin_addr);
	return os;
}

Server::~Server() {
}
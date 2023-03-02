#include "../inc/Server.hpp"

Server::Server(Config &cf) :_cf(cf) {
}

void Server::setup() {

	// create kqueue
	_kq = kqueue();
	if (_kq == -1) {
		std::cerr << "creating kqueue" << std::strerror(errno) << std::endl;
	}

	// loop over virtual servers
	for (int i = 0; i < _cf.servers.size(); i++) {

		// create socket, bind and listen
		Socket listenSocket(_cf.servers[i].host, _cf.servers[i].port);
		_listenSockets.push_back(listenSocket);

		// initialize kevent struct
		EV_SET(&_changeList, listenSocket.getfd(), EVFILT_READ, EV_ADD, 0, 0, 0);

		// attach kevent to kqueue
		if (kevent(_kq, &_changeList, 1, NULL, 0, NULL) == -1) {
			ERROR("adding listen socket to kqueue");
		}
		// display on standard out
		std::cout << RED << getTime() << RESET << listenSocket << "\tListening... " << std::endl;
	}
}

void Server::run() {

	// run setup !! still need error checking !!
	setup();

	// start kevent monitoring loop
	int	new_events;
	for(;;) {
		// retrieve triggered events !! still needs timespec struct for TIMEOUT !!
		new_events = kevent(_kq, NULL, 0, _eventList, EVENTS_MAX, NULL);
		if (new_events == -1) {
			ERROR("requesting new kevent");
		}
		else if (new_events > 0) {
			// loop over triggered events
			for (int i = 0; i < new_events; i++) {
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
	std::string buff(1024, '\0'); // char buff[24];
	int recv_len = recv(event.ident, &buff[0], buff.size(), 0);
	buff.resize(recv_len); // buff[recv_len] = '\0';
	// std::cout << "our buff: " << buff << std::endl;
	// display on standard out !! fucked up ATM !!
	std::cout << RED << getTime() << RESET << event << "\tReceiving... " << CYAN << buff << RESET  << std::endl;
	Request req(buff,cf);
	std::string response = req.response(_cf);
	// create HTTP header /w message
	// std::string response = "HTTP/1.1 200 OK\r\n";
	// 			response += "Content-Type: text/html; charset=UTF-8\r\n";
	// 			response += "\r\n";
	// 			response += read_html_file("Conf/html/index.html");

	// send response message
	send(event.ident, response.c_str(), response.size(), 0);
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
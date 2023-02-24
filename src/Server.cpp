#include "../inc/Server.hpp"

Server::Server(Config &cf) :_cf(cf) {
}

Server::~Server() {
}

void	Server::setup() {

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

	// initialize kevent structs
	for (int i = 0; i < _cf.servers.size(); i++) {
		struct kevent change;
		EV_SET(&change, _listenSockets[i].getfd(), EVFILT_READ, EV_ADD, 0, 0, 0);
		_changeList.push_back(change);	
		std::cout << RED << getTime() << RESET << "\t Listening " << _listenSockets[i] << std::endl;
	}

	// attach kevents to kqueue
	if (kevent(_kq, &_changeList[0], _changeList.size(), NULL, 0, NULL) == -1) {
		std::perror("adding listen sockets to kqueue");
	}
}

void	Server::run() {

	setup();
	// start kevent monitoring loop
	int	n_events;
	for(;;) {
		n_events = kevent(_kq, NULL, 0, _eventList, EVENTS_MAX, NULL);
		if (n_events == -1) {
			std::perror("requesting new kevent");
		}
		else if (n_events > 0) {
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

	Socket newClient;
	if (newClient.accept(event) < 0)
		return;
	EV_SET(&_event, newClient.getfd(), EVFILT_READ, EV_ADD, 0, 0, NULL);
	if (kevent(_kq, &_event, 1, NULL, 0, NULL) == -1) {
		std::perror("adding new client to kqueue");
	}
	std::string response = "HTTP/1.1 200 OK\r\n";
    			response += "Content-Type: text/html; charset=UTF-8\r\n";
    			response += "\r\n";
    			response += "Hello, world!\r\n";
	send(newClient.getfd(), response.c_str(), response.size(), 0);
}

void Server::onEOF(struct kevent& event) {

	EV_SET(&_changeList[0], event.ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
	if (kevent(_kq, &_changeList[0], _changeList.size(), NULL, 0, NULL) < 0)
		std::perror("removing event from kqueue");
	::close(event.ident);
	std::cout << RED << getTime() << RESET << "\t Disconnect" << std::endl;	
}

void Server::onRead(struct kevent& event) {

	// char buff[1024];
	std::string buff;
	recv(event.ident, &buff, sizeof(&buff), 0);
}

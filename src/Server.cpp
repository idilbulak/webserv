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

		// save listensocket to map
		_listenSockets.insert(std::pair<int, Socket>(listenSocket.getfd(), listenSocket));

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
		new_events = kevent(_kq, NULL, 0, &_eventList, 1, NULL);
		if (new_events == -1) {
			ERROR("requesting new kevent");
			break ;
		}
		else if (new_events > 0) {
			// loop over triggered events
			// for (int i = 0; i < new_events; i++) {
				if (isListenSockfd(_eventList))
					onClientConnect(_eventList);
				else if (_eventList.flags & EV_EOF)
					onEOF(_eventList);
				else if (_eventList.flags & EVFILT_WRITE)
					onWrite(_eventList);
				else if (_eventList.flags & EVFILT_READ)
					onRead(_eventList);
			// }
		}
	}
	close(_kq); //?????
}

bool Server::isListenSockfd(struct kevent& event) {

	std::map<int, Socket>::iterator it = _listenSockets.begin();
	for (; it != _listenSockets.end(); ++it) {
		if (event.ident == it->first)
			return true;
	}
	return false;
}

void Server::onClientConnect(struct kevent& event) {
	
	// accept new connection on listening socket
	int connectionSocket = _listenSockets[event.ident].accept();
	if (connectionSocket < 0)
		return ;
	
	// save new connection to map<socket, httpRequest>
	_connectionSockets.insert(std::pair<int, std::string>(connectionSocket, ""));

	// add new connection to kqueue for receiving
	EV_SET(&_changeList, connectionSocket, EVFILT_READ, EV_ADD, 0, 0, NULL);
	if (kevent(_kq, &_changeList, 1, NULL, 0, NULL) == -1) {
		ERROR("adding EVFILT_READ to kqueue");
	}

	// EV_SET(&_changeList, connectionSocket, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
	// if (kevent(_kq, &_changeList, 1, NULL, 0, NULL) == -1) {
	// 	ERROR("adding EVFILT_READ to kqueue");
	// }

	// // create newClient socket and accept connection
	// Socket newClient;
	// if (newClient.accept(event) < 0)
	// 	return;
	
	// // add newCLient to map
	// _clients[newClient.getfd()] = newClient;

	// // add socket to kqueue ready for reading
	// EV_SET(&_changeList, newClient.getfd(), EVFILT_READ, EV_ADD, 0, 0, NULL);
	// if (kevent(_kq, &_changeList, 1, NULL, 0, NULL) == -1) {
	// 	ERROR("adding EVFILT_READ to kqueue");
	// }
}

void Server::onEOF(struct kevent& event) {

	std::cout << RED << getTime() << RESET << event << "\tDisconnecting... " << std::endl;

	// remove connectionSocket from kqueue // not necessary???
	EV_SET(&_changeList, event.ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
	if (kevent(_kq, &_changeList, 1, NULL, 0, NULL) < 0)
		ERROR("removing event from kqueue");

	// remove connectionSocket from map
	_connectionSockets.erase(event.ident);

	// close connectionSocket file descriptor
	::close(event.ident);

	// // remove client from map
	// _clients.erase(event.ident);
}

void Server::onRead(struct kevent& event) {
		
	// remove event from kqueue
	EV_SET(&_changeList, event.ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
	if (kevent(_kq, &_changeList, 1, NULL, 0, NULL) < 0)
		ERROR("removing EVFILT_READ from kqueue");

	std::vector<char> buffer(4096);
	// char buffer[4096];
	int num_bytes = recv(event.ident, buffer.data(), buffer.size(), 0);
	if (num_bytes == -1) {
		ERROR("recv");
		close(event.ident); // remove from kqueue and map still
		return;
	}
	if (num_bytes == 0)
		return ;
	buffer.resize(num_bytes);
	_connectionSockets[event.ident].append(buffer.data(), buffer.size());

	// write request HTTP to terminal
	std::cout << RED << getTime() << RESET << event << "\tReceiving... \n\n" << YELLOW << _connectionSockets[event.ident] << RESET  << std::endl;

	// // retrieve socket from map by filedescriptor
	// _clients[event.ident].recv(event);

	// // write request HTTP to terminal
	// std::cout << RED << getTime() << RESET << event << "\tReceiving... \n\n" << YELLOW << _clients[event.ident].getHttpRequest() << RESET  << std::endl;


	// // add socket to kqueue ready for writing
	// // if (_clients[event.ident].isComplete())
	// EV_SET(&_changeList, event.ident, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
	// if (kevent(_kq, &_changeList, 1, NULL, 0, NULL) < 0) {
	// 	ERROR("adding EVFILT_WRITE to kqueue");
	// }

	// // create response HTTP header /w message
	// std::string httpRequest = _connectionSockets[event.ident];
	// std::string res = Response(httpRequest, _cf).generate();

	// create response HTTP header /w message
	std::string httpRequest = _connectionSockets[event.ident];
	std::string res = Response(httpRequest, _cf).generate();

	// send response message
	send(event.ident, res.c_str(), res.size(), 0);
}

void Server::onWrite(struct kevent& event) {

	std::cout << "HELLOOOOA!!!!!!!!!!!!!!!!!!" << std::endl;
	// // send response message
	// send(event.ident, res.c_str(), res.size(), 0);
	// create response HTTP header /w message
	std::string httpRequest = _connectionSockets[event.ident];
	std::string res = Response(httpRequest, _cf).generate();

	// send response message
	send(event.ident, res.c_str(), res.size(), 0);

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
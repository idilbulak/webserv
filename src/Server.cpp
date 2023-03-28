#include "../inc/Server.hpp"

Server::Server(Config &cf) :_cf(cf) {
}

void Server::setup() {

	// create kqueue
	_kq = kqueue();
	if (_kq == -1) {
		ERROR("kqueue() failed");
		exit(1);
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
			ERROR("adding listenSocket to kqueue");
			exit(1);
		}
		// display on standard out
		std::cout << RED << getTime() << RESET << listenSocket << "\tListening... " << std::endl;
	}
}

void Server::run() {

	// start kevent monitoring loop
	int	new_events;
	for(;;) {

		// retrieve triggered event !! still needs timespec struct for TIMEOUT !!
		new_events = kevent(_kq, NULL, 0, &_eventList, 1, NULL);
		// check what type of event triggered
		if (new_events == -1 || _eventList.flags & EV_ERROR) {
			ERROR("requesting new kevent");
			break ;
		}
		else if (_eventList.flags & EV_EOF || _eventList.filter == EVFILT_TIMER)
			closeConnection(_eventList);
		else if (isListenSockfd(_eventList))
			onClientConnect(_eventList);
		else if (_eventList.filter == EVFILT_READ)
			onRead(_eventList);
		else if (_eventList.filter == EVFILT_WRITE)
			onWrite(_eventList);
	}
	close(_kq); //?????
}

// bool Server::isListenSockfd(struct kevent& event) {

// 	std::map<int, Socket>::iterator it = _listenSockets.begin();
// 	for (; it != _listenSockets.end(); ++it) {
// 		if (event.ident == it->first)
// 			return true;
// 	}
// 	return false;
// }

bool Server::isListenSockfd(struct kevent& event) {

	std::map<int, Socket>::iterator it = _listenSockets.find(event.ident);
	if (it != _listenSockets.end()) {
		return true;
	}
	return false;
}

void Server::onClientConnect(struct kevent& event) {
	
	// check if multiple clients are connecting at same time
	for (int i = 0; i < event.data; i++;) {

		// accept new connection on listening socket
		int connectionSocket = _listenSockets[event.ident].accept();
		if (connectionSocket < 0) {
			return ;
		}

		// print
		std::cout << RED << getTime() << RESET << _listenSockets[event.ident] << "\tConnecting... " << PURPLE << connectionSocket << RESET << std::endl;
		
		// add new connection to kqueue for receiving
		EV_SET(&_changeList, connectionSocket, EVFILT_READ, EV_ADD, 0, 0, NULL);
		if (kevent(_kq, &_changeList, 1, NULL, 0, NULL) == -1) {
			ERROR("adding EVFILT_READ to kqueue");
		}

		// add timeout to bounch new connection if neccesary
		EV_SET(&_changeList, connectionSocket, EVFILT_TIMER, EV_ADD, 0, 5 * 1000, NULL);
		if (kevent(_kq, &_changeList, 1, NULL, 0, NULL) == -1) {
			ERROR("adding EVFILT_TIMER to kqueue");
		}

		// initialize struct to store request, response and port number
		struct SocketData data;
		data.port = _listenSockets[event.ident].getPort();
		_Clients.insert(std::make_pair(connectionSocket, data));
	}

}

void Server::onRead(struct kevent& event) {
		
	char buffer[4096];
	int num_bytes = recv(event.ident, buffer, sizeof(buffer), 0);
	if (num_bytes == -1) {
		ERROR("recv() failed");
		closeConnection(event);
		return;
	}
	if (num_bytes == 0) //??
		return ;
	buffer[num_bytes] = '\0';
	_Clients[event.ident].request.append(buffer, num_bytes);

	if (HttpRequest().isComplete(_Clients[event.ident].request)) {

		// delete timeout to bounch new connection if neccesary
		EV_SET(&_changeList, connectionSocket, EVFILT_TIMER, EV_DELETE, 0, 5 * 1000, NULL);
		if (kevent(_kq, &_changeList, 1, NULL, 0, NULL) == -1) {
			ERROR("adding EVFILT_TIMER to kqueue");
		}

		// write request HTTP to terminal
		std::cout << RED << getTime() << RESET << event << "\tReceiving... " << PURPLE << event.ident << RESET  << "\n\n" << YELLOW << _Clients[event.ident].request << RESET  << std::endl;
		
		// parse request and create response 
		_Clients[event.ident].response = Response(_Clients[event.ident].request, _cf).generate();

		// add socket to kqueue ready for writing
		EV_SET(&_changeList, event.ident, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
		if (kevent(_kq, &_changeList, 1, NULL, 0, NULL) < 0) {
			ERROR("adding EVFILT_WRITE to kqueue");
		}
	}
}

void Server::onWrite(struct kevent& event) {

	// send response message
	int num_bytes = send(event.ident, _Clients[event.ident].response.c_str(), _Clients[event.ident].response.size(), 0);
	if (num_bytes <= 0) {

		ERROR("send()");
		closeConnection(event);
		return;
	}
	_Clients[event.ident].response.erase(0, num_bytes);

	// remove client if complete response has been send
	if (_Clients[event.ident].response.empty()) {

		EV_SET(&_changeList, event.ident, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
		if (kevent(_kq, &_changeList, 1, NULL, 0, NULL) < 0)
			ERROR("removing EVFILT_WRITE event from kqueue");
		closeConnection(event);
	}
}

void Server::closeConnection(struct kevent& event) {

	std::cout << RED << getTime() << RESET << event << "\tDisconnecting... " << PURPLE << event.ident << RESET << std::endl;

	// remove connectionSocket from kqueue // not necessary???
	EV_SET(&_changeList, event.ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
	if (kevent(_kq, &_changeList, 1, NULL, 0, NULL) < 0)
		ERROR("removing EVFILT_READ event from kqueue");
	
	// free SocketData connected to event
	_Clients.erase(event.ident);

	// close connectionSocket file descriptor
	::close(event.ident);
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
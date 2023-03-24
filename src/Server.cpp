#include "../inc/Server.hpp"

Server::Server(Config &cf) :_cf(cf) {
}

int Server::setup() {

	// create kqueue
	_kq = kqueue();
	if (_kq == -1) {
		std::cerr << "creating kqueue" << std::strerror(errno) << std::endl;
		return -1;
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
			return -1;
		}
		// display on standard out
		std::cout << RED << getTime() << RESET << listenSocket << "\tListening... " << std::endl;
	}
	return 0;
}

void Server::run() {

	// run setup !! still need error checking !!
	if (!setup()) {
		exit(1);
	}

	// start kevent monitoring loop
	int	new_events;
	for(;;) {
		// retrieve triggered event !! still needs timespec struct for TIMEOUT !!
		new_events = kevent(_kq, NULL, 0, &_eventList, 1, NULL);
		if (new_events == -1) {
			ERROR("requesting new kevent");
			break ;
		}
		else if (new_events > 0) {
			// check what type of event triggered
				if (_eventList.flags & EV_EOF)
					onCloseConnection(_eventList);
				else if (isListenSockfd(_eventList))
					onClientConnect(_eventList);
				else if (_eventList.filter == EVFILT_READ)
					onRead(_eventList);
				else if (_eventList.filter == EVFILT_WRITE)
					onWrite(_eventList);
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

bool Server::isConnectionSockfd(struct kevent& event) {

	std::map<int, struct SocketData>::iterator it = _Clients.find(event.ident);
	if (it != _Clients.end()) {
		return true;
	}
	return false;
}

void Server::onClientConnect(struct kevent& event) {
	
	std::cout << "HERE onclientconnect>>>>>>>>>>>" << event.ident << std::endl;
	if (isConnectionSockfd(event)) {
		std::cout << "HELOOO!%&*@%*%#%@^#%&*@#%*@%#*%#*%#*@%&*#%*@" << std::endl;
		return ;
	}
	
	// accept new connection on listening socket
	int connectionSocket = _listenSockets[event.ident].accept();
	if (connectionSocket < 0) {
		return ;
	}

	std::cout << RED << getTime() << RESET << _listenSockets[event.ident] << "\tConnecting... " << PURPLE << connectionSocket << RESET << std::endl;
	
	// add new connection to kqueue for receiving
	EV_SET(&_changeList, connectionSocket, EVFILT_READ, EV_ADD, 0, 0, NULL);
	if (kevent(_kq, &_changeList, 1, NULL, 0, NULL) == -1) {
		ERROR("adding EVFILT_READ to kqueue");
	}

	// save http request received on new connection
	struct SocketData data;
	_Clients.insert(std::make_pair(connectionSocket, data));
}

void Server::onRead(struct kevent& event) {
		
	std::cout << "HEREon =Read>>>>>>>>>>>" << event.ident << std::endl;

	char buffer[4096];
	int num_bytes = recv(event.ident, buffer, sizeof(buffer), 0);
	if (num_bytes == -1) {
		ERROR("recv");
		onCloseConnection(event);
		return;
	}
	if (num_bytes == 0)
		return ;
	buffer[num_bytes] = '\0';
	_Clients[event.ident].request.append(buffer, num_bytes);

	if (HttpRequest().isComplete(_Clients[event.ident].request)) {

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

	std::cout << "HEREonWrite>>>>>>>>>>>" << event.ident << std::endl;

	// send response message
	int num_bytes = send(event.ident, _Clients[event.ident].response.c_str(), _Clients[event.ident].response.size(), 0);
	if (num_bytes <= 0) {

		ERROR("send()");
		onCloseConnection(event);
		return;
	}
	_Clients[event.ident].response.erase(0, num_bytes);

	//check if sending complete
	if (_Clients[event.ident].response.empty()) {

		EV_SET(&_changeList, event.ident, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
		if (kevent(_kq, &_changeList, 1, NULL, 0, NULL) < 0)
			ERROR("removing EVFILT_WRITE event from kqueue");
	}
}

void Server::onCloseConnection(struct kevent& event) {

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
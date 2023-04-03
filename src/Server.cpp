#include "../inc/Server.hpp"

Server::Server(Config &cf) :_cf(cf) {
}

void Server::setup() {

	_kq = kqueue();
	if (_kq == -1) {

		ERROR("kqueue() failed");
		exit(1);
	}

	for (int i = 0; i < _cf.servers.size(); i++) {

		Socket listenSocket(_cf.servers[i].host, _cf.servers[i].port);
		_listenSockets.insert(std::make_pair(listenSocket.getfd(), listenSocket));
		UpdateKqueue(listenSocket.getfd(), EVFILT_READ, EV_ADD, 0);
		printLog(listenSocket, "listening... ");
	}
}

void Server::run() {

	for(;;) {

		int new_events = kevent(_kq, NULL, 0, &_eventList, 1, NULL);
		if (new_events == -1 || _eventList.flags & EV_ERROR) {

			ERROR("requesting new kevent failed");
			exit(1); //??
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
}

void Server::UpdateKqueue(int fd, int filter, int flag, int data) {

	EV_SET(&_changeList, fd, filter, flag, 0, data, NULL);
	if (kevent(_kq, &_changeList, 1, NULL, 0, NULL) == -1) {

		ERROR("updateKqueue() failed");
		exit(1);
	}
}

bool Server::isListenSockfd(struct kevent& event) {

	std::map<int, Socket>::iterator it = _listenSockets.find(event.ident);
	if (it != _listenSockets.end()) {
		return true;
	}
	return false;
}

void Server::onClientConnect(struct kevent& event) {
	
	for (int i = 0; i < event.data; i++) {

		// std::cout << "data " << event.data << std::endl;
		int connectionSocket = _listenSockets[event.ident].accept();
		printLog(_listenSockets[event.ident], "Connecting... ", connectionSocket);
		UpdateKqueue(connectionSocket, EVFILT_READ, EV_ADD, 0);

		struct SocketData data;
		data.port = _listenSockets[event.ident].getPort();
		_Clients.insert(std::make_pair(connectionSocket, data));
	}
}

void Server::onRead(struct kevent& event) {
		
	// UpdateKqueue(event.ident, EVFILT_TIMER, EV_ADD | EV_ONESHOT, 5 * 1000);

	char buffer[65536];
	int num_bytes = recv(event.ident, buffer, sizeof(buffer) - 1, 0);
	if (num_bytes <= 0) {

		ERROR("recv() failed");
		closeConnection(event);
		return;
	}
	

	buffer[num_bytes] = '\0';
	_Clients[event.ident].request.append(buffer, num_bytes);
	// std::cout << _Clients[event.ident].request.max_size() << std::endl;

		
	if (HttpRequest().isComplete(_Clients[event.ident].request)) {

		// TEST
		// UpdateKqueue(event.ident, EVFILT_TIMER, EV_DELETE, 0);
		printLog(event, YELLOW, "Receiving... ", _Clients[event.ident].request);
		_Clients[event.ident].response = Response(_Clients[event.ident].request, _cf, _Clients[event.ident].port).generate();
		UpdateKqueue(event.ident, EVFILT_WRITE, EV_ADD, 0);
		printLog(event, CYAN, "Sending... ", _Clients[event.ident].response);
	}

}

void Server::onWrite(struct kevent& event) {

	// std::cout << "onwrite " << std::endl;
	size_t num_bytes = send(event.ident, _Clients[event.ident].response.c_str(), _Clients[event.ident].response.size(), 0);
	// std::cout << "num_bytes " << num_bytes << std::endl;

	if (num_bytes <= 0) {

		ERROR("send() failed");
		closeConnection(event);
		return;
	}
	_Clients[event.ident].response.erase(0, num_bytes);

	if (_Clients[event.ident].response.empty()) {
 		closeConnection(event);
		// std::cout << "emty" << std::endl;
	}
}

void Server::closeConnection(struct kevent& event) {

	if (event.filter == EVFILT_TIMER)
		printLog(event, PURPLE, "Disconnecting... ", "Connection timeout");
	else
		printLog(event, "", "Disconnecting... ");
	
	_Clients.erase(event.ident);
	::close(event.ident);
	// std::cout << "hegkfhsjkfhdjs" << std::endl;
}

void Server::printLog(Socket socket, std::string activity) {

	std::cout << RED << getTime() << RESET;
	std::cout << socket;
	std::cout << std::setw(17) << activity;
	std::cout << GREEN << socket.getfd() << RESET;
	std::cout << std::endl;
}

void Server::printLog(Socket socket, std::string activity, int filed) {

	std::cout << RED << getTime() << RESET;
	std::cout << socket;
	std::cout << std::setw(17) << activity;
	std::cout << GREEN << socket.getfd() << " >> " << filed << RESET;
	std::cout << std::endl;
}

void Server::printLog(struct kevent& event, std::string color, std::string activity) {

	std::cout << RED << getTime() << RESET;
	std::cout << event;
	std::cout << color << std::setw(17) << activity << RESET;
	std::cout << GREEN << event.ident << RESET; 
	std::cout << std::endl;
}

void Server::printLog(struct kevent& event, std::string color, std::string activity, std::string httpMessage) {

	std::cout << RED << getTime() << RESET;
	std::cout << event;
	std::cout << std::setw(17) << activity << RESET;
	std::cout << GREEN << std::setw(10) << event.ident << RESET;
	std::cout << color << httpMessage.substr(0, httpMessage.find("\r\n") + 1000) << RESET;
	// std::cout << std::endl << std::endl << color << httpMessage << RESET;
	std::cout << std::endl;
}

std::ostream& operator<<(std::ostream &os, struct kevent& event) {

	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);
	getsockname(event.ident, (struct sockaddr *)&addr, &addrlen);

	os << "port: " << BLUE << std::left << std::setw(8) << ntohs(addr.sin_port) << RESET;
	os << "IP address: " << BLUE << std::setw(12) << inet_ntoa(addr.sin_addr) << RESET;

	return os;
}

Server::~Server() {
}
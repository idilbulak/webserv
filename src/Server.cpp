#include "../inc/Server.hpp"
#include "../inc/Socket.hpp"
#include "../inc/Cgi.hpp"

Server::Server(Config &cf) :_cf(cf) {
}

void Server::setup() {

	_kqueue = kqueue();
	if (_kqueue == -1) {
		std::perror("[ERROR] kqueue() failed");
		exit(1);
	}
	for (size_t i = 0; i < _cf.servers.size(); i++) {
		Socket serverSocket(_cf.servers[i].host, _cf.servers[i].port);
		try {
			serverSocket.createSocket();
			serverSocket.setSocketAddr();
			serverSocket.setFiledOptions(serverSocket.getfd());
			serverSocket.bind();
			serverSocket.listen();
			UpdateKqueue(serverSocket.getfd(), EVFILT_READ, EV_ADD, 0);
			_listenSockets.insert(std::make_pair(serverSocket.getfd(), serverSocket));
			printLog(serverSocket, "listening... ");
		}
		catch (Socket::CreateSocketFail& e) {
			std::perror(e.what());
			continue;
		}
		catch (std::runtime_error& e) {
			std::perror(e.what());
			close(serverSocket.getfd());
			continue;
		}
	}
}

void Server::UpdateKqueue(int fd, int filter, int flag, int data) {

	EV_SET(&_changeList, fd, filter, flag, 0, data, NULL);
	if (kevent(_kqueue, &_changeList, 1, NULL, 0, NULL) == -1)
		throw std::runtime_error("[ERROR] updateKqueue() failed");
}

void Server::run() {

	struct timespec timeout = { 0, 0 };
	for(;!_listenSockets.empty();) {
		try {
			int new_event = kevent(_kqueue, NULL, 0, &_eventList, 1, &timeout);
			if (new_event == 0)
				continue;
			if (new_event == -1 || _eventList.flags & EV_ERROR)
				throw KeventFail();
			else if (_eventList.flags & EV_EOF || _eventList.filter == EVFILT_TIMER)
				closeConnection(_eventList);
			else if (isListenSockfd(_eventList))
				onClientConnect(_eventList);
			else if (_eventList.filter == EVFILT_READ)
				onRead(_eventList);
			else if (_eventList.filter == EVFILT_WRITE)
				onWrite(_eventList);
		}
		catch (Socket::AcceptFail& e) {
			std::perror(e.what());
		}
		catch (std::runtime_error& e) {
			std::perror(e.what());
			closeConnection(_eventList);
		}
		catch (KeventFail& e) {
			std::perror(e.what());
			close(_kqueue);
			closeAllConnections();
			closeListenSockets();
			setup();
			run();
		}
	}
}

bool Server::isListenSockfd(struct kevent& event) {

	std::map<int, Socket>::iterator it = _listenSockets.find(event.ident);
	if (it != _listenSockets.end())
		return true;
	return false;
}

void Server::onClientConnect(struct kevent& event) {
	
	for (int i = 0; i < event.data; i++) {
		int connectionSocket = _listenSockets[event.ident].accept();
		printLog(_listenSockets[event.ident], "Connecting... ", connectionSocket);
		UpdateKqueue(connectionSocket, EVFILT_READ, EV_ADD, 0);
		struct SocketData data;
		data.port = _listenSockets[event.ident].getPort();
		_Clients.insert(std::make_pair(connectionSocket, data));
	}
}

void Server::onRead(struct kevent& event) {

	UpdateKqueue(event.ident, EVFILT_TIMER, EV_ADD | EV_ONESHOT, 5 * 1000);
	char buffer[SIZE];
	int num_bytes = recv(event.ident, buffer, sizeof(buffer) - 1, 0);
	if (num_bytes <= 0)
		throw std::runtime_error("[ERROR] recv() failed");
	buffer[num_bytes] = '\0';
	printLog(event, YELLOW, "Receiving... ", buffer);
	_Clients[event.ident].request.append(buffer, num_bytes);
	if (HttpRequest().isComplete(_Clients[event.ident].request)) {
		UpdateKqueue(event.ident, EVFILT_TIMER, EV_DELETE, 0);
		printLog(event, PURPLE, "Processing... ");
		Response Response(_Clients[event.ident].request, _cf, _Clients[event.ident].port);
		_Clients[event.ident].response = Response.generate();
		_Clients[event.ident].closeConnection = checkConnectionType(Response);
		_Clients[event.ident].request.clear();
		UpdateKqueue(event.ident, EVFILT_WRITE, EV_ADD, 0);
	}
}

void Server::onWrite(struct kevent& event) {

	UpdateKqueue(event.ident, EVFILT_TIMER, EV_ADD | EV_ONESHOT, 60 * 1000);
	int num_bytes = send(event.ident, _Clients[event.ident].response.c_str(), _Clients[event.ident].response.size(), 0);
	if (num_bytes <= 0)
		throw std::runtime_error("[ERROR] send() failed");
	printLog(event, CYAN, "Sending... ", _Clients[event.ident].response.substr(0, num_bytes));
	_Clients[event.ident].response.erase(0, num_bytes);
	if (_Clients[event.ident].response.empty()) {
		UpdateKqueue(event.ident, EVFILT_WRITE, EV_DELETE, 0);
		if (_Clients[event.ident].closeConnection) {
			UpdateKqueue(event.ident, EVFILT_TIMER, EV_DELETE, 0);
			closeConnection(event);
		}
	}
}

bool Server::checkConnectionType(Response& Response) {

	if (Response.getReq().getHeaders()["Connection"].empty())
		return false;
	else if (Response.getReq().getHeaders()["Connection"].compare("close") == 0)
		return true;
	else
		return false;
}

void Server::closeConnection(struct kevent& event) {

	if (event.filter == EVFILT_TIMER)
		printLog(event, PURPLE, "Timeout... ");
	else
		printLog(event, "", "Disconnecting... ");
	close(event.ident);
	_Clients.erase(event.ident);
}

void Server::closeAllConnections() {

	std::map<int, struct SocketData>::iterator it;
	for (it = _Clients.begin(); it != _Clients.end(); ++it)
		close(it->first);
	_Clients.clear();
}

void Server::closeListenSockets() {

	std::map<int, Socket>::iterator it;
	for (it = _listenSockets.begin(); it != _listenSockets.end(); ++it)
		close(it->first);
	_listenSockets.clear();
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
	std::cout << GREEN << std::setw(8) << event.ident << RESET;
	// std::cout << std::endl << std::endl << color << httpMessage << RESET;
	std::cout << std::endl << std::endl << color << httpMessage.substr(0, 500) << RESET;
	std::cout << std::endl << std::endl;
}

std::ostream& operator<<(std::ostream &os, struct kevent& event) {

	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);
	getsockname(event.ident, (struct sockaddr *)&addr, &addrlen);
	os << BLUE << std::left << std::setw(10) << inet_ntoa(addr.sin_addr);
	os << ":" << std::setw(8) << ntohs(addr.sin_port) << RESET;
	return os;
}

Server::~Server() {
}

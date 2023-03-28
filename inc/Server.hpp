#ifndef Server_HPP
# define Server_HPP

#include <iostream>
#include "Config.hpp"
#include "Socket.hpp"
#include "Response.hpp"

// #include "Time.hpp"
// #include <sys/event.h>
// #include "fcntl.h"

struct SocketData {

	std::string request;
	std::string response;
	std::string port;
};

class Server {

	public:
		Server(Config &cf);
		~Server();

		void	setup();
		void	run();
		
	private:
		Config _cf;
		int _kq;
		std::map<int, Socket> _listenSockets;
		// std::vector<int> _ListenSockets;
		// std::vector<int> _ConnectionSockets;
		std::map<int, struct SocketData> _Clients;
		// std::vector<int> _ConnectionSockets;
		// std::map<int, std::string> _request;
		// std::map<int, std::string> _response;
		struct kevent _changeList;
		struct kevent _eventList;

		bool	isListenSockfd(struct kevent& event);
		// bool	isConnectionSockfd(struct kevent& event);
		void	onClientConnect(struct kevent& event);
		void	onRead(struct kevent& event);
		void	onWrite(struct kevent& event);
		void	closeConnection(struct kevent& event);
		// bool 	isComplete(struct kevent& event);

		// void	UpdateKqueue()

};

std::ostream& operator<<(std::ostream &os, struct kevent& event);

#endif
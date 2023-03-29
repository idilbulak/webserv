#ifndef Server_HPP
# define Server_HPP

#include <iostream>
#include <iomanip>
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
		std::map<int, struct SocketData> _Clients;
		struct kevent _changeList;
		struct kevent _eventList;

		void	UpdateKqueue(int fd, int filter, int flag, int data);
		bool	isListenSockfd(struct kevent& event);
		void	onClientConnect(struct kevent& event);
		void	onRead(struct kevent& event);
		void	onWrite(struct kevent& event);
		void	closeConnection(struct kevent& event);

		void	printLog(Socket socket, std::string activity);
		void	printLog(Socket socket, std::string activity, int filed);
		void	printLog(struct kevent& event, std::string color, std::string activity);
		void	printLog(struct kevent& event, std::string color, std::string activity, std::string httpMessage);
};

std::ostream& operator<<(std::ostream &os, struct kevent& event);

#endif
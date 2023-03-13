#ifndef Server_HPP
# define Server_HPP

#include <iostream>
#include "Config.hpp"
#include "Socket.hpp"
#include "Response.hpp"

// #include "Time.hpp"
// #include <sys/event.h>
// #include "fcntl.h"

class Server {

	public:
		Server(Config &cf);
		~Server();

		void	run();
		
	private:
		Config _cf;
		int _kq;
		std::vector<Socket> _listenSockets;
		std::map<int, Socket> _clients;
		static const int EVENTS_MAX = 1;
		struct kevent _changeList;
		struct kevent _eventList[EVENTS_MAX];

		void	setup();
		bool	isListenSockfd(struct kevent& event);
		void	onClientConnect(struct kevent& event);
		void	onEOF(struct kevent& event);
		void	onRead(struct kevent& event);
		void	onWrite(struct kevent& event);

};

std::ostream& operator<<(std::ostream &os, struct kevent& event);

#endif
#ifndef Server_HPP
# define Server_HPP

#include <iostream>
#include "Config.hpp"
#include "Socket.hpp"
// #include "Time.hpp"
// #include <sys/event.h>
// #include "fcntl.h"

class Server {
    public:
        Server(Config &cf);
        ~Server();

        void	setup();
		void	run();
		
		bool	isListenSockfd(struct kevent& event);
		void	onClientConnect(struct kevent& event);
		void	onEOF(struct kevent& event);
		void	onRead(struct kevent& event);

    private:
		Config	_cf;
		int		_kq;
		std::vector<Socket> _listenSockets;
		std::vector<struct kevent> _changeList;
		static const int EVENTS_MAX = 2;
		struct kevent _event;
		struct kevent _eventList[EVENTS_MAX];
		// std::vector<struct kevent> _eventList;
	// std::ostream	&operator << (std::ostream &out, const Server &n);

};


#endif
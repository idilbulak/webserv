#include "../inc/Network.hpp"

Network::Network(Config &cf) :_cf(cf) {
}

Network::~Network() {
}

void	Network::start() {
	int kq = kqueue();
	if (kq == -1) {
		std::cout << "kqueue error" << std::endl;
		exit(1);
	}
	struct kevent *eset=new struct kevent[_cf.servers.size()+1]; ///?
	t_udata udata;
	for (int i = 0; i < _cf.servers.size(); ++i) {
		Socket s(_cf.servers[i].host, _cf.servers[i].port);
		udata.port = _cf.servers[i].port;
		EV_SET(eset+i, s.getServerfd(), EVFILT_READ, EV_ADD, 0, 0, &udata);
		std::string time_str = getTime();
		std::cout << "\033[31m" << time_str << "\033[0m\t Listening on port: " << udata.port << std::endl;
	}
	if (kevent(kq, eset, _cf.servers.size(), NULL, 0, NULL) < 0) {
		std::cout << "kevent error" << std::endl;
		exit(1);
	}
	// accept_loop
	struct kevent events;
	int	n_events;
	while (1) {
		// n_events = kevent(kq,NULL,0,events,_cf.servers.size(),NULL);
		// if (n_events == -1) 
		// 	break;
		if (kevent(kq,NULL,0,&events,_cf.servers.size(),NULL) < 0) 
			break;
		for ( int i = 0; i < _cf.servers.size(); ++i, ++eset )
			if (eset->ident == events.ident)
		{
			struct sockaddr_in cliaddr;
			socklen_t clilen = sizeof(cliaddr);
			int clientfd = accept(events.ident, (struct sockaddr *)&cliaddr, &clilen);
			int optval = 1;
			setsockopt(clientfd,SOL_SOCKET,SO_REUSEADDR,&optval,sizeof(optval));
			struct kevent new_event;
			t_udata new_udata;
			EV_SET( &new_event, clientfd, EVFILT_READ, EV_ADD, 0, 0, &new_udata );
			if (kevent(kq, &new_event, 1, NULL, 0, NULL) < 0) {
				std::cout << "new connection error" << std::endl;
				exit(1);
			}
			std::string time_str = getTime();
			std::cout << "\033[31m" << time_str << "\033[0m\t New connection on port: (how to carry the port?)" << std::endl;
		}
		handleConnections(eset, &events);
	}
	delete[] eset;
	close(kq);
}

void Network::handleConnections(struct kevent *eset, struct kevent *events) {
	std::cout << "this girl sucks" << std::endl;
	// exit(0);
}
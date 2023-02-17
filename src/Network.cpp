#include "../inc/Network.hpp"


Network::Network(Config &cf) :_cf(cf) {
}

Network::~Network() {
}

void	Network::start() {
	// check if the current server being set up is trying to listen on a port that is already being used by another server in the servers.
	// if not.. 
		// create the new socket on that port
		// multiple sockets for multiple ports?
		// create bind listen
	Socket s = Socket(_cf.servers[0].host, _cf.servers[0].port);
		// fill the event struct with macro
	int kq = kqueue();
	if (kq == -1) {
		std::cout << "kqueue error" << std::endl;
		exit(1);
	}
	// register the event
	struct kevent eset;
	t_udata udata; //how to fill??
	EV_SET(&eset, s.getSocketfd(), EVFILT_READ, EV_ADD, 0, 0, &udata);
	if (kevent(kq, &eset, _cf.servers.size(), NULL, 0, NULL) == -1) {
		std::cout << "kevent error" << std::endl;
		exit(1);
	}
	// accept_loop
	struct kevent events;
	int	n_events;
	while (1) {
		n_events = kevent(kq, NULL, 0, &events, _cf.servers.size(), NULL);
		if (n_events == -1) 
			break;
		for(int i=0;i<n_events;i++)
			handleConnections(eset, events);
	}
	close(kq);
}

void Network::handleConnections(struct kevent eset, struct kevent events) {
	std::cout << "this girl is ON FIREEEEEEEEEEEE" << std::endl;
}

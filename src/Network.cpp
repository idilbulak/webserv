#include "../inc/Network.hpp"


Network::Network(Config &cf) :_cf(cf) {
}

Network::~Network() {
}

void	Network::start() {
	int kq;
	struct kevent event;
	t_udata udata; //how to fill??
	int len = _cf.servers.size();
	// check if the current server being set up is trying to listen on a port that is already being used by another server in the servers.
	// if not.. 
		// create the new socket on that port
		// multiple sockets for multiple ports?
		// create bind listen
	Socket s = Socket(_cf.servers[0].host, _cf.servers[0].port);
		// fill the event struct with macro
	if (kq == kqueue() == -1) {
		std::cout << "kqueue error" << std::endl;
		exit(1);
	}
	EV_SET(&event, s.getSocketfd(), EVFILT_READ, EV_ADD, 0, 0, &udata);
	if (kevent(kq, &event, len, NULL, 0, NULL) == -1) {
		std::cout << "kevent error" << std::endl;
		exit(1);
	}





	
}
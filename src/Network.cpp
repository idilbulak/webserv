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
	struct kevent *eset=new struct kevent[_cf.servers.size()];
	t_udata udata;
	for (int i = 0; i < _cf.servers.size(); ++i) {
		Socket s(_cf.servers[i].host, _cf.servers[i].port);
		// EV_SET(eset+i, s.getSocketfd(), EVFILT_READ, EV_ADD, 0, 0, &udata);
		std::string time_str = getTime();
		std::cout << "\033[31m" << time_str << "\033[0m\t Listening on port: " << _cf.servers[i].port << std::endl;
	}
	if (kevent(kq, eset, _cf.servers.size(), NULL, 0, NULL) == -1) {
		std::cout << "kevent error" << std::endl;
		exit(1);
	}
	// accept_loop
	struct kevent events[100];
	int	n_events;
	while (1) {
		n_events = kevent(kq, NULL, 0, events, _cf.servers.size(), NULL);
		if (n_events == -1) 
			break;
		for(int i=0;i<n_events;i++)

			handleConnections(eset, events);
	}
	delete[] eset;
	close(kq);
}

void Network::handleConnections(struct kevent *eset, struct kevent *events) {
	std::cout << "this girl is ON FIREEEEEEEEEEEE" << std::endl;
}
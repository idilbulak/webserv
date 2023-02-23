#include "../inc/Network.hpp"

Network::Network(Config &cf) :_cf(cf) {
}

Network::~Network() {
}

void	Network::start() {

	// create sockets, bind and listen
	std::vector<Socket> listen_sockets;
	for (int i = 0; i < _cf.servers.size(); i++) {
		Socket listen(_cf.servers[i].host, _cf.servers[i].port);
		listen_sockets.push_back(listen);
	}

	// create the kqueue
	int kq = kqueue();
	if (kq == -1) {
		std::perror("kqueue()");
		exit(1);
	}

	// initialize kevent structs
	std::vector<struct kevent> change_list;
	for (int i = 0; i < _cf.servers.size(); i++) {
		struct kevent change;
		EV_SET(&change, listen_sockets[i].getServerfd(), EVFILT_READ, EV_ADD, 0, 0, 0);
		change_list.push_back(change);	
		std::string time_str = getTime();
		std::cout << "\033[31m" << time_str << "\033[0m\t Listening on port: " << listen_sockets[i].getPort() << std::endl;
	}

	// attach kevents to kqueue
	if (kevent(kq, &change_list[0], change_list.size(), NULL, 0, NULL) == -1) {
		std::perror("kevent()1");
		exit(1);
	}

	// start kevent monitoring loop
	struct kevent events[2];//[100];
	int	n_events;

	for(;;) {
		n_events = kevent(kq,NULL,0,&events[0],2,NULL);
		if (n_events == -1) 
			std::perror("kevent()2");
		else if (n_events > 0) {
			for (int i = 0; i < n_events; i++) {
				// check for new client connection
				for (int j = 0; j < listen_sockets.size(); j++) {
					if (events[i].ident == listen_sockets[j].getServerfd()) {
						struct sockaddr_in cliaddr;
						socklen_t clilen = sizeof(cliaddr);
						int clientfd = accept(events[i].ident, (struct sockaddr *)&cliaddr, &clilen);
						int optval = 1;
						setsockopt(clientfd,SOL_SOCKET,SO_REUSEADDR,&optval,sizeof(optval));
						struct kevent new_event;
						t_udata new_udata;
						EV_SET( &new_event, clientfd, EVFILT_READ, EV_ADD, 0, 0, &new_udata );
						if (kevent(kq, &new_event, 1, NULL, 0, NULL) == -1) {
							perror("kevent()3");
							std::cout << "new connection error" << std::endl;
							exit(1);
						}
						std::string time_str = getTime();
						std::cout << "\033[31m" << time_str << "\033[0m\t New connection on port: " << listen_sockets[j].getPort() << std::endl;	
					}
				}
				//check for EOF
				//check for read
			}
		}
	}
	close(kq);
}

void Network::handleConnections(struct kevent *eset, struct kevent *events) {
	std::cout << "this girl sucks" << std::endl;
	// exit(0);
}
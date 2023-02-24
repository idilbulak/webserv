#include "../inc/Network.hpp"

Network::Network(Config &cf) :_cf(cf) {
}

Network::~Network() {
}

void	Network::accept_new_connection(int kq,struct kevent &event) {
			struct sockaddr_in cliaddr;
			socklen_t clilen = sizeof(cliaddr);
			int clientfd;
			if(clientfd == accept(event.ident, (struct sockaddr *)&cliaddr, &clilen) < 0)
				std::cout << "clientfd kapot" << std::strerror(errno)<< std::endl;
								// break;}
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

bool		Network::is_in_listen_sockfd(struct kevent *eset,int fd,int len) {
	for (int i=0; i<len; ++i, ++eset)
		if (eset->ident == (unsigned long)fd)
			return true;
	return false;
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
	struct kevent events[1024];
	int	n_events;
	while (1) {
		if (kevent(kq,NULL,0,events,_cf.servers.size(),NULL) < 0) 
			break;
		else if (is_in_listen_sockfd(eset, events->ident, _cf.servers.size()))
			accept_new_connection(kq, *events);
		//else if read
		//else if write
		handleConnections(eset, events);
	}
	delete[] eset;
	close(kq);
}

void Network::handleConnections(struct kevent *eset, struct kevent *events) {
	std::cout << "this girl sucks" << std::endl;
	// exit(0);
}
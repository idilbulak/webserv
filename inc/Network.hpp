#ifndef NETWORK_HPP
# define NETWORK_HPP

#include <iostream>
#include <sys/event.h>
#include "Config.hpp"
#include "Socket.hpp"
#include "Time.hpp"

typedef struct s_udata {
  // what should i put?
  std::string port;
} t_udata;

class Network {
    private:
		Config	_cf;
    public:
        Network(Config &cf);
        ~Network();

        void start();
        void	accept_new_connection(int kq,struct kevent &event);
		    bool		is_in_listen_sockfd(struct kevent *eset,int fd,int len);
        void handleConnections(struct kevent *eset, struct kevent *events);

	// std::ostream	&operator << (std::ostream &out, const Network &n);

};


#endif
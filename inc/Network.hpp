#ifndef NETWORK_HPP
# define NETWORK_HPP

#include <iostream>
#include <sys/event.h>
#include "Config.hpp"
#include "Socket.hpp"

typedef struct s_udata {
  // what should i put?
} t_udata;

class Network {
    private:
		Config	_cf;
    public:
        Network(Config &cf);
        ~Network();

        void start();
        void handleConnections(struct kevent eset, struct kevent events);

	// std::ostream	&operator << (std::ostream &out, const Network &n);

};


#endif
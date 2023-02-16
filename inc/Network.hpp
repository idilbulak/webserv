#ifndef NETWORK_HPP
# define NETWORK_HPP

#include <iostream>
#include "Config.hpp"
#include "Socket.hpp"

class Network {
    private:
		Config	_cf;
    public:
        Network(Config &cf);
        ~Network();

        void start();

	// std::ostream	&operator << (std::ostream &out, const Network &n);

};


#endif
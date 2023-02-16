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
		Socket s = Socket(_cf.servers[0].host, _cf.servers[0].port);

	
}
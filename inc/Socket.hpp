#ifndef SOCKET_HPP
# define SOCKET_HPP

#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

# define BACKLOG 3

class Socket {
	private:
		std::string		_host;
		std::string		_port;
		int				_serverfd;

	public:
		Socket(std::string host, std::string port);
		~Socket(void);

		std::string getHost( void );
		std::string getPort( void );
		int getSocketfd( void );

        void create();
        void identify();
        void wait_incomingconnections();
        void accept_loop();

};

#endif
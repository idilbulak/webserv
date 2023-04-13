#ifndef Server_HPP
# define Server_HPP

# include "Config.hpp"
# include "Socket.hpp"
# include "Response.hpp"

struct SocketData {

	std::string request;
	std::string response;
	std::string port;
	bool		closeConnection;
};

class Server {

	public:
		Server(Config &cf);
		~Server();

		void setup();
		void run();

	private:
		Config _cf;
		int _kqueue;
		bool _serverIsListening;
		std::map<int, Socket> _listenSockets;
		std::map<int, struct SocketData> _Clients;
		struct kevent _changeList;
		struct kevent _eventList;

		void setupKqueue();
		void setupListenSocket(Socket& serverSocket);
		void UpdateKqueue(int fd, int filter, int flag, int data);
		bool isListenSockfd(struct kevent& event);
		void onClientConnect(struct kevent& event);
		void onRead(struct kevent& event);
		void onWrite(struct kevent& event);
		bool checkConnectionType(Response& Response);
		void closeConnection(struct kevent& event);
		void closeAllConnections();
		void closeListenSockets();

		void printLog(Socket socket, std::string activity);
		void printLog(Socket socket, std::string activity, int filed);
		void printLog(struct kevent& event, std::string color, std::string activity);
		void printLog(struct kevent& event, std::string color, std::string activity, std::string httpMessage);

		class KeventFail : public std::exception {
			public:
				const char* what() const throw() {
					return "[ERROR] kevent() failed";
				}
		};
};

std::ostream& operator<<(std::ostream &os, struct kevent& event);

#endif

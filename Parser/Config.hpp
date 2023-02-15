#ifndef CONFIG_HPP
# define CONFIG_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>

struct Location {
    std::string					path;
	std::vector<std::string>	methods;
	std::string					root;
	int							autoindex;
	std::string					cgi_ext;
	std::string					cgi_path;
	std::string					upload_dir;
	std::vector<std::string>	index;
};

struct ServerConfig {
    std::string					host;
	std::string					port;
	std::string					name;
	std::string					root;
	int							max_body_size;
	std::map<int,std::string>   error_pages;
	std::vector<Location>		locations;
};

struct NetworkConfig {
	    std::vector<ServerConfig>   servers;
};

class Config {
    private:
        const std::string	_path;
	    NetworkConfig		conf;
    public:
        Config(const std::string &path);
        ~Config();

        void parse();
        void display();
        void getServer(std::vector<std::string> &tokens, size_t &i);
		void getHostPort(std::string &host, std::string &port, std::string &tokens);
		void getErrorPage(std::map<int,std::string> &error_pages, std::vector<std::string> &tokens, size_t &i);
		void getLocation(std::vector<Location> &locations, const std::vector<std::string> &tokens, size_t &i);

	// std::ostream	&operator << ( std::ostream &out, const Config &conf );

};


#endif
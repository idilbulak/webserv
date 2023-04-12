#ifndef CONFIG_HPP
# define CONFIG_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <exception>

#define RED "\033[31m"
#define CYAN "\033[0;36m"
#define BLACK "\033[0;30m"
#define RESET "\033[0m"

struct Location {
    std::string					path;
	std::vector<std::string>	methods;
	std::string					root;
	int							autoindex;
	std::string					cgi_path;
	std::string					upload_dir;
	std::vector<std::string>	index;
	int							redirect_cd;
	std::string					redirect_url;
	std::string					max_body_size;
};

struct VirtualServer {
    std::string					host;
	std::string					port;
	std::string					name;
	std::string					root;
	std::vector<std::string>	cgi_ext;
	std::map<int,std::string>   error_pages;
	std::vector<Location>		locations;
};

class Config {
    private:
        const std::string	_path;
    public:
	    std::vector<VirtualServer>   servers; //make this private
        Config(const std::string &path);
        ~Config();

        void parse();
        void display();
        void parseServer(std::vector<std::string> &tokens, size_t &i);
		void parseHostPort(std::string &host, std::string &port, std::string &tokens);
		void parseErrorPage(std::map<int,std::string> &error_pages, std::vector<std::string> &tokens, size_t &i);
		void parseLocation(std::vector<Location> &locations, const std::vector<std::string> &tokens, size_t &i);
		bool	check_word(const std::string &word);
		void parseCgiExt(std::vector<std::string> &cgi_ext, const std::vector<std::string> &tokens, size_t &i);

		//check all data..

		void CheckPort(std::string port);
		void CheckServerName(std::string serverName);
		void CheckClientMaxBodySize(std::string maxBodysize);
		void CheckLocation(std::vector<Location> location);
		void CheckPath(std::string path);
        void CheckAllow(std::vector<std::string> methods);
        void CheckUploadDir(std::string	upload_dir);
        void CheckIndex(std::vector<std::string> index);
        void CheckReturn(int redirect_cd, std::string redirect_url);


	// std::ostream	&operator << ( std::ostream &out, const Config &conf );

};


#endif
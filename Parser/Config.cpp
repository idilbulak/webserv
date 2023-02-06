#include "Config.hpp"

Config::Config(const std::string &path) :_path(path) {
    this->parse();
}

Config::~Config() {
}

void	Config::parse() {

    std::ifstream ifs(_path.c_str());
    std::vector<std::string> tokens;
    std::string token;
    while (ifs >> token) {
        if (token == "{" || token == "}")
            continue;
        tokens.push_back(token);
    }
    ifs.close();

	for (size_t i = 0; i != tokens.size(); ++i) {
		if (tokens[i] == "server" || tokens[i] == "?server")
			getServer(tokens, i);
	}
}

void	Config::getServer(std::vector< std::string > &tokens, size_t &i) {
	ServerConfig	server;
	for (size_t i = 0; i != tokens.size(); ++i) {
		if (tokens[i] == "listen")
            getHostPort(server.host, server.port, tokens[++i]);
		else if (tokens[i] == "server_name")
			server.name = tokens[++i];
		else if (tokens[i] == "root")
			server.root = tokens[++i];
		else if (tokens[i] == "client_max_body_size" )
		    server.max_body_size = std::stoi(tokens[i]);
		else if (tokens[i] == "error_page" )
			getErrorPage(server.error_pages, tokens, i);
		else if (tokens[i] == "location" )
			getLocation(server.locations, tokens, i);
	}
	conf.servers.push_back(server);
}

void	Config::getHostPort(std::string &host, std::string &port, std::string &tokens) {
    std::stringstream ss;
    size_t found = tokens.find(":");
    host = tokens.substr(0, found);
    ss << tokens.substr(found+1);
    ss >> port;
}

bool	check_word(const std::string &word) {
	if (word == "listen" || word == "location" || word == "client_max_body_size" || word == "server_name")
		return true;
	return false;
}

static bool str_isdigit(std::string line)
{
	for (size_t i = 0; i < line.length(); i++)
		if (!isdigit(line[i]))
			return (0);
	return (1);
}

void	Config::getErrorPage(std::map<int,std::string> &error_pages, std::vector<std::string> &tokens, size_t &i) {
	++i;
	int j = i;
	while (tokens[j] != ";" && !check_word(tokens[j]))
		++j;
	std::string path = tokens[--j];
	while (str_isdigit(tokens[i])) {
		error_pages[std::atoi(tokens[i].c_str())] = path;
		i++;
	}
}

void	Config::getLocation(std::vector<Location> &locations, const std::vector<std::string> &tokens, size_t &i) {
	Location	loc;
	loc.path = tokens[++i];
	for ( ++i; tokens[i] != "location" || tokens[i] != "server"; ++i ) {
		if ( tokens[i] == "allow" ) {
			for ( ++i; tokens[i] != ";"; ++i )
				loc.methods.push_back(tokens[i]);
		}
		else if ( tokens[i] == "root" )
			loc.root = tokens[++i];
		else if ( tokens[i] == "autoindex" ) {
			if (tokens[++i]== "on")
				loc.autoindex = 1;
			else if (tokens[++i]== "off")
				loc.autoindex = 0;
		}
		else if (tokens[i] == "cgi_ext") {
			loc.cgi_ext = tokens[++i];
			loc.cgi_path = tokens[++i];
		}
		else if (tokens[i] == "upload_dir")
			loc.upload_dir = tokens[++i];
	}
	locations.push_back(loc);
}
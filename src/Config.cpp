#include "../inc/Config.hpp"

Config::Config(const std::string &path) :_path(path) {
    this->parse();
}

Config::~Config() {
}

void	Config::parse() {

    std::ifstream ifs(_path.c_str());
	if (!ifs.is_open()) {
        std::cerr << "File does not exist: " << _path << std::endl;
        exit(1);
    }
    std::vector<std::string> tokens;
    std::string token;
    while (ifs >> token) {
        if (token == "{" || token == "}")
            continue;
        tokens.push_back(token);
    }
    ifs.close();
	for (size_t i = 0; i != tokens.size(); ++i) {
		size_t found = tokens[i].find(";");
		if (found)
			tokens[i] = tokens[i].substr(0, found);
		// printf("tokens: %s|\n, %d\n", tokens[i].c_str(), i);
	}
	for (size_t i = 0; i < tokens.size(); i++) {
		if (tokens[i] == "server" || tokens[i] == "?server")
			parseServer(tokens, i);
		if (tokens[i] == "server")
			i--;
	}
}

void	Config::parseServer(std::vector<std::string> &tokens, size_t &i) {
	VirtualServer	server;
	i++;
	for (;i<tokens.size()-1;) {
		// std::cout << tokens[i] << "is token " << std::endl;
		if(tokens[i] == "server")
			break;
		if (tokens[i] == "listen") 
            parseHostPort(server.host, server.port, tokens[++i]);
		else if (tokens[i] == "server_name")
			server.name = tokens[++i];
		else if (tokens[i] == "root")
			server.root = tokens[++i];
		else if (tokens[i] == "client_max_body_size" )
		    server.max_body_size = tokens[++i];
		else if (tokens[i] == "error_page" )
			parseErrorPage(server.error_pages, tokens, i);
		else if (tokens[i] == "location" )
			parseLocation(server.locations, tokens, i);
		else
			i++;
	}
	servers.push_back(server);
}

void	Config::parseHostPort(std::string &host, std::string &port, std::string &tokens) {
    std::stringstream ss;
    size_t found = tokens.find(":");
    host = tokens.substr(0, found);
    ss << tokens.substr(found+1);
    ss >> port;
}

bool	check_word(const std::string &word) {
	if (word.empty())
		return true;
	if (word == "index" || word == "listen" || word == "location" || word == "client_max_body_size" || word == "server_name" || word == "error_page" || word == "root" || word == "index" || word == "return")
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

void	Config::parseErrorPage(std::map<int,std::string> &error_pages, std::vector<std::string> &tokens, size_t &i) {
	++i;
	int j = i;
	while (tokens[j] != "\0" && !check_word(tokens[j]))
		++j;
	std::string path = tokens[--j];
	while (str_isdigit(tokens[i])) {
		error_pages[std::atoi(tokens[i].c_str())] = path;
		i++;
	}
}

void	Config::parseLocation(std::vector<Location> &locations, const std::vector<std::string> &tokens, size_t &i) {
	Location	loc;
	loc.path = tokens[++i];
	//what do you mean? if for work, first if doesnt always work :))))
	for (; i<tokens.size()-1 && (tokens[i] != "location" || tokens[i] != "server");) {
		if (tokens[i] == "location" || tokens[i] == "server")
			break;
		if ( tokens[i] == "allow" ) {
			for ( ++i; !check_word(tokens[i]); ++i)
				loc.methods.push_back(tokens[i]);
		}
		if ( tokens[i] == "root" )
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
		else if (tokens[i] == "upload_dir") {
			loc.upload_dir = tokens[++i];
		}
		else if (tokens[i] == "index")
		{
			for (++i;!check_word(tokens[i]); ++i)
				loc.index.push_back(tokens[i]);			
		}
		else if (tokens[i] == "return") { //////////birdaha bakimmalisinn 601 gelince mesela patliyo
			loc.redirect_cd = stoi(tokens[++i]);
			loc.redirect_url = tokens[++i];
		}
		else
			i++;
	}
	locations.push_back(loc);
}

void Config::display() {
    std::cout << "Config path: " << _path << std::endl;
    std::cout << "Servers: " << std::endl;
    for (std::vector<VirtualServer>::const_iterator it = servers.begin(); it != servers.end(); it++) {
        const VirtualServer server = *it;
        std::cout << "\tHost: " << server.host << std::endl;
        std::cout << "\tPort: " << server.port << std::endl;
        std::cout << "\tName: " << server.name << std::endl;
        std::cout << "\tRoot: " << server.root << std::endl;
        std::cout << "\tMax Body Size: " << server.max_body_size << std::endl;
        std::cout << "\tError Pages: " << std::endl;
        for (std::map<int, std::string>::const_iterator it_err = server.error_pages.begin(); it_err != server.error_pages.end(); ++it_err) {
            std::cout << "\t\tCode: " << it_err->first << std::endl;
            std::cout << "\t\tPath: " << it_err->second << std::endl;
        }
        std::cout << "\tLocations: " << std::endl;
        for (std::vector<Location>::const_iterator it_loc = server.locations.begin(); it_loc != server.locations.end(); ++it_loc) {
            const Location &location = *it_loc;
	        std::cout << "\tlocation: " << std::endl;
            std::cout << "\t\tPath: " << location.path << std::endl;
            std::cout << "\t\tMethods: " << std::endl;
            for (std::vector<std::string>::const_iterator it_meth = location.methods.begin(); it_meth != location.methods.end(); ++it_meth) {
                std::cout << "\t\t\t" << *it_meth << std::endl;
            }
            std::cout << "\t\tRoot: " << location.root << std::endl;
            std::cout << "\t\tAuto Index: " << location.autoindex << std::endl;
            std::cout << "\t\tCGI Ext: " << location.cgi_ext << std::endl;
            std::cout << "\t\tCGI Path: " << location.cgi_path << std::endl;
            std::cout << "\t\tUpload Dir: " << location.upload_dir << std::endl;
            std::cout << "\t\tIndex: " << std::endl;
            for (std::vector<std::string>::const_iterator it_ind = location.index.begin(); it_ind != location.index.end(); ++it_ind) {
                std::cout << "\t\t\t" << *it_ind << std::endl;
            }
			std::cout << "\t\tRedirection code: " << location.redirect_cd << std::endl;
			std::cout << "\t\tRedirection url: " << location.redirect_url << std::endl;
        }
	}
}
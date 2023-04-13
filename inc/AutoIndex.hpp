#ifndef AUTOINDEX_HPP
# define AUTOINDEX_HPP

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <sys/stat.h>
#include <cstdio>
#include <cstdlib>

class AutoIndex {

	public:
		AutoIndex(std::string root);
		~AutoIndex();

        std::vector<std::string>	list_directory(const std::string& dir_path);
		std::string 				generate_html_page();
	
	private:
		std::string	_root;
		std::string	_dirname;
		

};

#endif
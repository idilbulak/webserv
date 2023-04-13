#include <stdlib.h>
#include "../inc/Config.hpp"
#include "../inc/Server.hpp"

int main(int argc, char **argv) {
	std::string fileName;
	if (argc == 1)
		fileName = "Conf/test.conf";
	else if (argc != 2)
		exit(1);
	else
		fileName = argv[1];
	Config cf(fileName);
	// cf.display();
	Server Server(cf);
	Server.setup();
	Server.run();
	return 0;
}
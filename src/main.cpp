#include <stdlib.h>
#include "../inc/Config.hpp"
#include "../inc/Server.hpp"

int main(int argc, char **argv) {

	if (argc != 2) {
		exit(1); // instead we should do default config??
	}
	Config cf(argv[1]);
	// cf.display();
	// Server Server(cf);
	// Server.setup();
	// Server.run();
	return 0;
}
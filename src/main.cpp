#include <stdlib.h>
#include "../inc/Config.hpp"
#include "../inc/Server.hpp"
// #include "../inc/Network.hpp"

int main(int argc, char **argv) {
    if (argc != 2)
        exit(1);
    Config cf(argv[1]);
    // cf.display();
    Server(cf).run();
    return 0;
}
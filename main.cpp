#include <stdlib.h>
#include "Parser/Config.hpp"

int main(int argc, char **argv) {
    if (argc != 2)
        exit(1);
    Config cf(argv[1]);
    cf.display();
    return 0;
}
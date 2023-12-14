#include <iostream>
#include "json.hpp"
#include "MCAPWriter.h"

int main(int argc, char **argv){
    // Open MCAP writer:
    mcap_wrapper::open_file("toto.mcap");
    return 0;
}

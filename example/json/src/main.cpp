#include <iostream>
#include <chrono>
#include <thread>
#include "json.hpp"
#include "MCAPWriter.h"

int main(int argc, char **argv){
    // Open MCAP writer:
    mcap_wrapper::open_file("toto.mcap");
    // Write data into file:
    for(unsigned i=0; i<20; i++){
        // Create data
        nlohmann::json test_json;
        test_json["x"] = i;
        test_json["a"] = "toto";
        test_json["b"] = 42.3698;
        test_json["c"] = -1157.0;
        test_json["d"] = nlohmann::json();
        test_json["d"]["a"] = i * 150.0;
        test_json["d"]["b"] = "titi";
        test_json["d"]["c"] = 0;

        // Put it to file
        mcap_wrapper::write_JSON("test_json", test_json.dump(), std::chrono::system_clock::now().time_since_epoch().count());

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    // Close file
    mcap_wrapper::close_file("toto.mcap");

    return 0;
}

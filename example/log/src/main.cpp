#include <iostream>
#include <chrono>
#include <thread>
#include "MCAPWriter.h"

int main(int argc, char **argv){
    // Open MCAP writer:
    mcap_wrapper::open_file("simple.mcap");
    // Write data into file:
    for(unsigned i=0; i<6; i++){
        // Create log
        mcap_wrapper::LOG_LEVEL current_log_level;
        if(i == 0)
            current_log_level = mcap_wrapper::LOG_LEVEL::DEBUG;
        if(i == 1)
            current_log_level = mcap_wrapper::LOG_LEVEL::ERROR;
        if(i == 2)
            current_log_level = mcap_wrapper::LOG_LEVEL::FATAL;
        if(i == 3)
            current_log_level = mcap_wrapper::LOG_LEVEL::INFO;
        if(i == 4)
            current_log_level = mcap_wrapper::LOG_LEVEL::UNKNOWN;
        if(i == 5)
            current_log_level = mcap_wrapper::LOG_LEVEL::WARNING;
        
        mcap_wrapper::write_log("logs", std::chrono::system_clock::now().time_since_epoch().count(), current_log_level, "This is a log message", "LOG", "example/log/src/main.cpp", 26);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    // Close file
    mcap_wrapper::close_file("simple.mcap");

    return 0;
}

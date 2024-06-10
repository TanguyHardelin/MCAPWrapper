#include <iostream>
#include <chrono>
#include <thread>
#include "MCAPWriter.h"

int main(int argc, char **argv){
    // Open MCAP writer:
    mcap_wrapper::open_file_connection("simple.mcap");
    mcap_wrapper::open_network_connection("127.0.0.1", 5001, "netwoooooerk", "netwwwwookooookokok");

    mcap_wrapper::set_connection_to_be_sync("simple.mcap", true);
    mcap_wrapper::set_connection_to_be_sync("netwoooooerk", true);
    // Write data into file:
    for(unsigned i=0; i<6; i++){
        std::this_thread::sleep_for(std::chrono::seconds(10));


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
        
        mcap_wrapper::write_log_to_all("logs", std::chrono::system_clock::now().time_since_epoch().count(), current_log_level, "This is a log message", "LOG", "example/log/src/main.cpp", 26);

        int * t = NULL;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    // Close file
    mcap_wrapper::close_file_connection("simple.mcap");

    return 0;
}

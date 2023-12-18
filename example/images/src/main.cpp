#include <iostream>
#include <chrono>
#include <thread>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include "MCAPWriter.h"

int main(int argc, char **argv){
    // Open MCAP writer:
    mcap_wrapper::open_file("simple.mcap");

    // Open cv::Mat image
    std::string image_path = RESSOURCE_PATH; // `RESSOURCE_PATH` is defined in cmake
    image_path += "/sample_image.jpg";
    cv::Mat image = cv::imread(image_path.c_str());

    // Write data into file:
    for(unsigned i=0; i<20; i++){
        cv::Mat image_to_save = image.clone();
        
        // Simple image manipulation
        if(i%2 == 0)
            cv::cvtColor(image_to_save.clone(), image_to_save, cv::COLOR_BGRA2GRAY);
        
        // Save image:
        mcap_wrapper::write_image("simple.mcap", image_to_save, std::chrono::system_clock::now().time_since_epoch().count());

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    // Close file
    mcap_wrapper::close_file("simple.mcap");

    return 0;
}

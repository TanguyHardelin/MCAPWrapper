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

    // Current position:
    Eigen::Matrix4f current_position = Eigen::Matrix4f::Identity();

    // Add transform:
    // mcap_wrapper::add_frame_transform("root", std::chrono::system_clock::now().time_since_epoch().count(), "root", "", Eigen::Matrix4f::Identity());

    // Write data into file:
    for(unsigned i=0; i<20; i++){
        cv::Mat image_to_save = image.clone();
        uint64_t current_timestamp = std::chrono::system_clock::now().time_since_epoch().count();

        // Update current position:
        current_position(0,3) += 1;

        // Save it:
        // mcap_wrapper::add_position("simple_image_position", current_timestamp, current_position);
        mcap_wrapper::add_frame_transform("simple_image_position", current_timestamp, "root", "", current_position);
        
        // Save image:
        mcap_wrapper::write_image("simple_image", image_to_save, current_timestamp, "simple_image_id");

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    // Close file
    mcap_wrapper::close_file("simple.mcap");

    return 0;
}

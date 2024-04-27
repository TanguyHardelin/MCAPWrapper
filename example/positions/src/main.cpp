#include <iostream>
#include <chrono>
#include <thread>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include "MCAPWriter.h"

int main(int argc, char **argv)
{
    // Open MCAP writer:
    mcap_wrapper::open_file_connexion("simple.mcap");
    mcap_wrapper::open_network_connexion("0.0.0.0",8765,"simple_server");

    // Open cv::Mat image
    std::string image_path = RESSOURCE_PATH; // `RESSOURCE_PATH` is defined in cmake
    image_path += "/sample_image.jpg";
    cv::Mat image = cv::imread(image_path.c_str());

    // Current position:
    Eigen::Matrix4f current_position = Eigen::Matrix4f::Identity();

    // Camera calibration:
    mcap_wrapper::write_camera_calibration_all("simpleCamera",
                                               std::chrono::system_clock::now().time_since_epoch().count(), 
                                               "simple_image_tttt",
                                               image.cols, image.rows, "rational_polynomial", 
                                               {0, 0, 0, 0, 0}, {100, 0, 0, 0, 100, 0, 0, 0, 1}, {1, 0, 0, 1, 0, 0, 0, 1}, {100, 0, 0, 0, 0, 100, 0, 0, 0, 0, 1, 0});

        // Add transform:
        mcap_wrapper::add_frame_transform_to_all("root", std::chrono::system_clock::now().time_since_epoch().count(), "map", "root", Eigen::Matrix4f::Identity());

    // Write data into file:
    for (unsigned i = 0; ; i++)
    {
        // TODO write it only in network
        mcap_wrapper::write_camera_calibration_all("simpleCamera",
                                               std::chrono::system_clock::now().time_since_epoch().count(), 
                                               "simple_image_tttt",
                                               image.cols, image.rows, "rational_polynomial", 
                                               {0, 0, 0, 0, 0}, {100, 0, 0, 0, 100, 0, 0, 0, 1}, {1, 0, 0, 1, 0, 0, 0, 1}, {100, 0, 0, 0, 0, 100, 0, 0, 0, 0, 1, 0});

        cv::Mat image_to_save = image.clone();
        uint64_t current_timestamp = std::chrono::system_clock::now().time_since_epoch().count();

        // Update current position:
        current_position(0, 3) = i % 20;

        // Save it:
        // mcap_wrapper::add_position_to_all("simple_image_position", current_timestamp, current_position);
        mcap_wrapper::add_frame_transform_to_all("simple_image_id", current_timestamp, "root", "simple_image_tttt", current_position);

        // Save image:
        mcap_wrapper::write_image_to_all("simple_image", image_to_save, current_timestamp, "simple_image_tttt");

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    // Close file
    mcap_wrapper::close_file_connexion("simple.mcap");

    return 0;
}

#include <iostream>
#include <thread>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include "MCAPReader.h"
#include "MCAPWriter.h"
#include "json.hpp"

double calculatePSNR(const cv::Mat& I1, const cv::Mat& I2);

int main(int argc, char **argv)
{
    // Create MCAP file:
    mcap_wrapper::open_file_connexion("test.mcap");

    // Open reference image:
    std::string reference_image_path = RESSOURCE_PATH; // `RESSOURCE_PATH` is defined in cmake
    reference_image_path += "/sample_image.jpg";
    cv::Mat reference_image = cv::imread(reference_image_path);

    // Define reference vector that will be used as reference
    std::vector<nlohmann::json> pushed_json_values;
    std::vector<cv::Mat> pushed_images;
    std::vector<std::string> pushed_logs;

    // Iterate for creating file:
    unsigned iteration_number = std::max(15, rand() % 50);
    for (unsigned i = 0; i < iteration_number; i++)
    {
        uint64_t current_timestamp = std::chrono::system_clock::now().time_since_epoch().count();
        // JSON:
        nlohmann::json sample_json;
        sample_json["random_value"] = rand()%1024;
        sample_json["fixed_value"] = "This is a fixed value";
        mcap_wrapper::write_JSON_to_all("sample_json", sample_json.dump(), current_timestamp);
        pushed_json_values.push_back(sample_json);
        // Image:
        cv::Mat sample_image = reference_image.clone();
        int kernel_size = std::max(3, rand()%7);
        cv::blur(sample_image, sample_image, cv::Size(kernel_size, kernel_size));
        auto t0 = std::chrono::high_resolution_clock::now();
        mcap_wrapper::write_image_to_all("sample_image", sample_image, current_timestamp);
        auto t1 = std::chrono::high_resolution_clock::now();
        std::cout << "Image compression time " << std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() << std::endl;
        
        pushed_images.push_back(sample_image);
        // Log:
        mcap_wrapper::LOG_LEVEL log_level = mcap_wrapper::LOG_LEVEL(rand()%6);
        std::string log = "This is a log: #" + std::to_string(i);
        auto t3 = std::chrono::high_resolution_clock::now();
        mcap_wrapper::write_log_to_all("sample_log", current_timestamp, log_level, log, "LOG", "tests/UNIT/src/main.cpp", 42);
        auto t4 = std::chrono::high_resolution_clock::now();

        std::cout << "Logs compression time " << std::chrono::duration_cast<std::chrono::milliseconds>(t4 - t3).count() << std::endl;

        pushed_logs.push_back(log);
        // Little sleep
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    mcap_wrapper::close_file_connexion("test.mcap");

    // Read it:
    mcap_wrapper::MCAPReader reader("test.mcap");
    // Verify channel:
    std::map<std::string, mcap_wrapper::MCAPReaderChannelType> all_channel = reader.get_channels();
    std::cout << "all_channel " << all_channel.size() << std::endl;
    if(all_channel.count("sample_json") == 0){
        std::cerr << "Test failed !" << std::endl << "REASON: \"sample_json\" channel could not be retrieved by reader" << std::endl;
        return 1;
    }
    if(all_channel["sample_json"] != mcap_wrapper::MCAPReaderChannelType::RAW_JSON){
        std::cerr << "Test failed !" << std::endl << "REASON: \"sample_json\" is not RAW JSON" << std::endl;
        return 1;
    }
    if(all_channel.count("sample_image") == 0){
        std::cerr << "Test failed !" << std::endl << "REASON: \"sample_image\" channel could not be retrieved by reader" << std::endl;
        return 1;
    }
    if(all_channel["sample_image"] != mcap_wrapper::MCAPReaderChannelType::IMAGE){
        std::cerr << "Test failed !" << std::endl << "REASON: \"sample_image\" is not IMAGE" << std::endl;
        return 1;
    }
    if(all_channel.count("sample_log") == 0){
        std::cerr << "Test failed !" << std::endl << "REASON: \"sample_log\" channel could not be retrieved by reader" << std::endl;
        return 1;
    }
    if(all_channel["sample_log"] != mcap_wrapper::MCAPReaderChannelType::LOG){
        std::cerr << "Test failed !" << std::endl << "REASON: \"sample_log\" is not LOG" << std::endl;
        return 1;
    }
    // Verify content:
    cv::Mat image;
    while(reader.get_next_image("sample_image", image)){
        if(calculatePSNR(image, pushed_images[0]) < 40){
            // The image is only little compressed in JPEG so 40 PSNR value seems ok
            std::cerr << "Test failed !" << std::endl << "REASON: retrieved image is not the same" << std::endl;
            return 1;
        }
        pushed_images.erase(pushed_images.begin());
    }

    std::string serialized_json;
    while(reader.get_next_message("sample_json", serialized_json)){
        nlohmann::json retrieved_json = nlohmann::json::parse(serialized_json);
        if(pushed_json_values[0].dump() != retrieved_json.dump()){
            // The image is only little compressed in JPEG so 40 PSNR value seems ok
            std::cerr << "Test failed !" << std::endl << "REASON: retrieved json is not the same" << std::endl;
            return 1;
        }
        pushed_json_values.erase(pushed_json_values.begin());
    }

    std::string log;
    while(reader.get_next_message("sample_json", log)){
        if(pushed_logs[0] != log){
            // The image is only little compressed in JPEG so 40 PSNR value seems ok
            std::cerr << "Test failed !" << std::endl << "REASON: retrieved log is not the same" << std::endl;
            return 1;
        }
        pushed_logs.erase(pushed_logs.begin());
    }
    
    if(pushed_images.size() > 0 || pushed_json_values.size() > 0 || pushed_logs.size() == 0){
        std::cerr << "Test failed !" << std::endl << "Not all data were read" << std::endl;
        return 1;
    }

    std::cout << "All Test succeed ! All good !" << std::endl;

    return 0;
}

double calculatePSNR(const cv::Mat& I1, const cv::Mat& I2) {
    cv::Mat s1;
    cv::absdiff(I1, I2, s1);       // |I1 - I2|
    s1.convertTo(s1, CV_32F);      // convert to float
    s1 = s1.mul(s1);               // |I1 - I2|^2

    cv::Scalar s = cv::sum(s1);    // sum elements per channel

    double mse = s.val[0] / (double)(I1.channels() * I1.total());
    double psnr = 10.0 * log10((255 * 255) / mse);

    return psnr;
}

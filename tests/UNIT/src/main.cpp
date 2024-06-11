#include <iostream>
#include <thread>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include "MCAPReader.h"
#include "MCAPWriter.h"
#include "json.hpp"

double calculatePSNR(const cv::Mat& I1, const cv::Mat& I2);
double computeMean(const std::vector<double>& vec);

int main(int argc, char **argv)
{
    // Create MCAP file:
    mcap_wrapper::open_file_connection("test.mcap");

    // Open reference image:
    std::string reference_image_path = RESSOURCE_PATH; // `RESSOURCE_PATH` is defined in cmake
    reference_image_path += "/sample_image.jpg";
    std::cout << "reference_image_path " << reference_image_path << std::endl;
    cv::Mat reference_image = cv::imread(reference_image_path);
    if(reference_image.empty()){ 
        std::cerr << "Could not load image" << std::endl;
        return 1;
    }

    // Define reference vector that will be used as reference
    std::vector<nlohmann::json> pushed_json_values;
    std::vector<cv::Mat> pushed_images;
    std::vector<std::string> pushed_logs;

    // Iterate for creating file:
    unsigned iteration_number = std::max(15, rand() % 50);
    std::vector<double> mean_push_image_runtime;
    std::vector<double> mean_push_log_runtime;
    std::vector<double> mean_push_raw_message_runtime;
    for (unsigned i = 0; i < iteration_number; i++)
    {
        uint64_t current_timestamp = std::chrono::system_clock::now().time_since_epoch().count();
        // JSON:
        nlohmann::json sample_json;
        sample_json["random_value"] = rand()%1024;
        sample_json["fixed_value"] = "This is a fixed value";
        auto json_t0 = std::chrono::high_resolution_clock::now();
        mcap_wrapper::write_JSON_to_all("sample_json", sample_json.dump(), current_timestamp);
        auto json_t1 = std::chrono::high_resolution_clock::now();
        mean_push_raw_message_runtime.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(json_t1 - json_t0).count());

        pushed_json_values.push_back(sample_json);
        // Image:
        std::cout << "Will push image" << std::endl;
        cv::Mat sample_image = reference_image.clone();
        int kernel_size = std::max(3, rand()%7);
        std::cout << "Will blur image" << std::endl;
        cv::blur(sample_image, sample_image, cv::Size(kernel_size, kernel_size));
        std::cout << "End blur image" << std::endl;
        auto image_t0 = std::chrono::high_resolution_clock::now();
        mcap_wrapper::write_image_to_all("sample_image", sample_image, current_timestamp);
        auto image_t1 = std::chrono::high_resolution_clock::now();
        std::cout << "Image pushed" << std::endl;
        mean_push_image_runtime.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(image_t1 - image_t0).count());
        
        pushed_images.push_back(sample_image);
        // Log:
        mcap_wrapper::LOG_LEVEL log_level = mcap_wrapper::LOG_LEVEL(rand()%6);
        std::string log = "This is a log: #" + std::to_string(i);
        auto log_t0 = std::chrono::high_resolution_clock::now();
        mcap_wrapper::write_log_to_all("sample_log", current_timestamp, log_level, log, "LOG", "tests/UNIT/src/main.cpp", 42);
        auto log_t1 = std::chrono::high_resolution_clock::now();
        mean_push_log_runtime.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(log_t1 - log_t0).count());

        pushed_logs.push_back(log);
        // Little sleep
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    mcap_wrapper::close_file_connection("test.mcap");

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
        std::cout << "Will compute PSNR" << std::endl;
        if(calculatePSNR(image, pushed_images[0]) < 45){
            // The image is only little compressed in JPEG so 40 PSNR value seems ok
            std::cerr << "Test failed !" << std::endl << "REASON: retrieved image is not the same" << std::endl;
            return 1;
        }
        std::cout << "PSNR computed" << std::endl;
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
        std::cerr << "Number of log not read: " << pushed_logs.size() << std::endl;
        std::cerr << "Number of image not read: " << mean_push_image_runtime.size() << std::endl;
        std::cerr << "Number of json not read: " << mean_push_raw_message_runtime.size() << std::endl;
        return 1;
    }

    std::cout << "All Test succeed ! All good !" << std::endl;
    std::cout << "Mean log push time " << computeMean(mean_push_log_runtime) <<  " ns " << std::endl;
    std::cout << "Mean image push time " << computeMean(mean_push_image_runtime) << " ns " <<  std::endl;
    std::cout << "Mean raw json push time " << computeMean(mean_push_raw_message_runtime) << " ns " <<  std::endl;
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

double computeMean(const std::vector<double>& vec) {
    if (vec.empty()) {
        return 0.0; // Return 0 if the vector is empty
    }

    double sum = 0.0;
    for (const auto& value : vec) {
        sum += value;
    }
    return sum / static_cast<double>(vec.size());
}

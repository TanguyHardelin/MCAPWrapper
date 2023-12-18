#include "MCAPWriter.h"
#include "internal/MCAPFileWriter.h"
#include "internal/FoxgloveSchema.hpp"
#include "internal/Base64.hpp"

#include <map>
#include <fstream>
#include <opencv2/imgcodecs.hpp>

namespace mcap_wrapper
{
    // Each file stream is stored into dictionnary for being called later.
    std::map<std::string, MCAPFileWriter> file_writers;

    bool open_file(std::string file_path){
        file_writers[file_path] = MCAPFileWriter();
        if(file_writers[file_path].open(file_path))
            return true;
        return false;
    }

    void close_file(std::string file_path){
        if(file_writers.count(file_path))
            file_writers[file_path].close();
    }
    
    bool write_image(std::string identifier, cv::Mat image, uint64_t timestamp){
        for(auto & kv: file_writers){
            // Create schema if not present:
            if(!kv.second.is_schema_present(identifier)){
                nlohmann::json schema_unserialized = nlohmann::json::parse(compressed_image_schema);
                kv.second.create_schema(identifier, schema_unserialized);
            }

            nlohmann::json image_sample;
            image_sample["timestamp"] = nlohmann::json();
            image_sample["timestamp"]["sec"] = timestamp / (uint64_t) 1e9;
            image_sample["timestamp"]["nsec"] = timestamp % (uint64_t)1e9;
            image_sample["frame_id"] = "";
            // Encode image in JPEG
            std::vector<int> compression_params;
            compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
            compression_params.push_back(95);  // Adjust the quality (0-100), higher is better quality

            std::vector<uchar> encoding_buffer;
            cv::imencode(".jpg", image, encoding_buffer, compression_params);
            // Foxglove wait buffer data in base64 so we need to convert it
            image_sample["data"] = base64::to_base64(std::string(encoding_buffer.begin(), encoding_buffer.end()));
            image_sample["format"] = "jpeg";

            std::ofstream sample_file("test.txt");
            sample_file << base64::to_base64(std::string(encoding_buffer.begin(), encoding_buffer.end())) << std::endl;
            sample_file.close();

            kv.second.push_sample(identifier, image_sample, timestamp);
        }
        return false;
    }

    bool write_JSON(std::string identifier, std::string serialized_json, uint64_t timestamp){
        nlohmann::json unserialiazed_json;
        try{
            unserialiazed_json = nlohmann::json::parse(serialized_json);
        }
        catch(std::exception e){ // Parse error:
            std::cerr << "ERROR: failed to parse " << serialized_json << std::endl;
            return false;
        }
        for(auto & kv: file_writers)
            kv.second.push_sample(identifier, unserialiazed_json, timestamp);
        return true;
    }
};
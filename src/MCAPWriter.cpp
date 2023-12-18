#include "MCAPWriter.h"
#include "internal/MCAPFileWriter.h"

#include <map>

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

        // // Fill out `timestamp` field of json
        // uint64_t number_of_seconds = timestamp / 1e9;
        // uint64_t number_of_nano_seconds = fmod(timestamp,1e9);
        // unserialiazed_json["timestamp"] = nlohmann::json();
        // unserialiazed_json["timestamp"]["sec"] = number_of_seconds;
        // unserialiazed_json["timestamp"]["nsec"] = number_of_nano_seconds;
        for(auto & kv: file_writers)
            kv.second.push_sample(identifier, unserialiazed_json, timestamp);
        return true;
    }
};
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
    
    bool write_image(std::string identifier, cv::Mat image, uint64_t timestamp){

    }

    bool write_JSON(std::string identifier, std::string serialized_json, uint64_t timestamp){

    }
};
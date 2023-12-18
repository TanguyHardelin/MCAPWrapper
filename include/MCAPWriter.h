#ifndef MCAP_WRITER_HPP
#define MCAP_WRITER_HPP

#include <string>
#include <opencv2/core.hpp>

namespace mcap_wrapper{
    /**
     * @brief Create file for writting MCAP in it. The path must be valid.
     * 
     * @param file_path path to the file to write
     * @return true could create file
     * @return false could not create file
     */
    bool open_file(std::string file_path);
    /**
     * @brief Close file corresponding to the `file_path`
     * 
     * @param file_path path to the file to close
     */
    void close_file(std::string file_path);
    /**
     * @brief Close all files opened
     * 
     */
    void close_all_files();
    /**
     * @brief Write image into MCAP. This function is thread safe.
     * 
     * @param identifier identifier to where write the input image
     * @param image cv::Mat representing the image
     * @param timestamp image's timestamp
     * @return true could write image
     * @return false could not write image
     */
    bool write_image(std::string identifier, cv::Mat image, uint64_t timestamp);
    /**
     * @brief Write JSON into MCAP. Each JSON data "type" must have a property called "__foxglove_name__" in order to be interpretable by
     * Foxglove studio as valid data. This function is thread safe.
     * 
     * @param identifier identifier to where write the JSON
     * @param serialized_json JSON serialized into string
     * @param timestamp timestamp of JSON
     * @return true could write JSON
     * @return false could not write JSON
     */
    bool write_JSON(std::string identifier, std::string serialized_json, uint64_t timestamp);
};

#endif
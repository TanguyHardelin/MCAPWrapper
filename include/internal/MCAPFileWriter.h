#ifndef MCAP_FILE_WRITER_HPP
#define MCAP_FILE_WRITER_HPP

#include <string>
#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <map>
#include "mcap/writer.hpp"
#include "json.hpp"

namespace mcap_wrapper
{
    class MCAPFileWriter
    {
    public:
        // Constructor / desctructor
        MCAPFileWriter();
        ~MCAPFileWriter();
        /**
         * @brief Open MCAP file in write mode. Will create a dedicated write thread
         * 
         * @param file_name path to file to write
         * @return true Open succeed
         * @return false Open failed
         */
        bool open(std::string file_name);
        /**
         * @brief Close file and stop writing thread
         * 
         * @return true Close suceed
         * @return false Close failed
         */
        bool close();
        /**
         * @brief Return true if file is openned
         * 
         * @return true File is already open
         * @return false File is closed
         */
        bool is_open();       
        /**
         * @brief Return true if one schema is already present in writer for dedicated channel. A schema is what will describe the data for foxglove studio.
         * 
         * @param channel_name Name of the channel to check
         * @return true One schema is already present
         * @return false No schema present
         */
        bool is_schema_present(std::string channel_name);
        /**
         * @brief Create a schema by infering data type contained into data sample
         * 
         * @param channel_name Channel name of data
         * @param sample Sample of data
         */
        void infer_schema(std::string channel_name, nlohmann::json sample);
        /**
         * @brief Create a schema for data corresponding to `channel_name`
         * 
         * @param channel_name Channel name of data
         * @param schema Schema of data
         */
        void create_schema(std::string channel_name, nlohmann::json schema);
        /**
         * @brief Push sample into file.
         * 
         * @param channel_name Channel to which data will be pushed.
         * @param sample Sample of data
         * @param timestamp Timestamp of data
         */
        void push_sample(std::string channel_name, nlohmann::json sample, uint64_t timestamp);

        // Deffine operator= for std::mutex and std::conditionnal variable
        MCAPFileWriter& operator=(const MCAPFileWriter& object);

    protected:
        // Methods:
        nlohmann::json infer_property_of_sample(nlohmann::json sample, bool recursive_call = false);
        void run(); // Function used for storing data into file

        // Attributes:
        mcap::McapWriter _file_writer;                          // File writer object
        std::mutex _file_writer_mtx;                            // Mutex of `_file_writer`
        std::map<std::string, mcap::Channel> _all_channels;     // All channels schema
        std::queue<mcap::Message> _data_queue;                  // Data FIFO (used by write thread to get data)
        std::mutex _data_queue_mtx;                             // Mutex of `_data_queue`
        std::thread * _writing_thread;                          // Writing thread
        bool _continue_writing;                                 // Variable used for indicating to the writing thread if write must continue;
        std::condition_variable _write_notifier;                // Used for signaling new data to write for writing thread
        std::map<std::string, std::string> _defined_schema;     // Is usefull for keeping trace of defined schema
    };

    
};

#endif
#ifndef MCAP_FILE_WRITER_HPP
#define MCAP_FILE_WRITER_HPP

#include <string>
#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <map>
#include <Eigen/Core>
#include "mcap/writer.hpp"
#include "json.hpp"
#include "Internal3DObject.h"
#include "utils.hpp"
#include "IWriter.h"

namespace mcap_wrapper
{
    class MCAPFileWriter: public IWriter
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
        virtual bool open(std::string file_name);
        /**
         * @brief Close file and stop writing thread
         *
         * @return true Close suceed
         * @return false Close failed
         */
        virtual bool close() override;
        /**
         * @brief Return true if file is openned
         *
         * @return true File is already open
         * @return false File is closed
         */
        virtual bool is_open() override;
        /**
         * @brief Push sample into file.
         *
         * @param channel_name Channel to which data will be pushed.
         * @param sample Sample of data
         * @param timestamp Timestamp of data
         */
        virtual void push_sample(std::string channel_name, nlohmann::json sample, uint64_t timestamp) override;
        /**
         * @brief Create a schema for data corresponding to `channel_name`
         *
         * @param channel_name Channel name of data
         * @param schema Schema of data
         */
        virtual void create_schema(std::string channel_name, nlohmann::json schema) override;
        
        // Deffine operator= for std::mutex and std::conditionnal variable
        MCAPFileWriter &operator=(const MCAPFileWriter &object);

    protected:
        void run(); // Function used for storing data into file

        // Attributes:
        mcap::McapWriter _file_writer;                                      // File writer object
        std::mutex _file_writer_mtx;                                        // Mutex of `_file_writer`
        std::queue<mcap::Message> _data_queue;                              // Data FIFO (used by write thread to get data)
        std::mutex _data_queue_mtx;                                         // Mutex of `_data_queue`
        std::thread *_writing_thread;                                       // Writing thread
        bool _continue_writing;                                             // Variable used for indicating to the writing thread if write must continue;
        std::condition_variable _write_notifier;                            // Used for signaling new data to write for writing thread
        std::map<std::string, std::string> _defined_schema;                 // Is usefull for keeping trace of defined schema
        std::mutex write_is_being_process;                                  // Mutex that is grab during the whole write process
        std::condition_variable write_finished_adviser;                     // Conditional variable that is notified when whole data were wrote
    };

};

#endif
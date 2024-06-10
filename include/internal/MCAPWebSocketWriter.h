#ifndef MCAP_WEBSOCKET_WRITER_H
#define MCAP_WEBSOCKET_WRITER_H

#include <string>
#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <map>
#include <atomic>
#include <chrono>
#include <cmath>
#include <csignal>
#include <Eigen/Core>
#include "mcap/writer.hpp"
#include "json.hpp"
#include "Internal3DObject.h"
#include "utils.hpp"

#include "foxglove_websocket/base64.hpp"
#include "foxglove_websocket/server_factory.hpp"
#include "foxglove_websocket/websocket_notls.hpp"
#include "foxglove_websocket/websocket_server.hpp"
#include "IWriter.h"

namespace mcap_wrapper
{
    class MCAPWebSocketWriter: public IWriter
    {
    public:
        // Constructor / desctructor
        MCAPWebSocketWriter();
        ~MCAPWebSocketWriter();
        /**
         * @brief Open MCAP file in write mode. Will create a dedicated write thread
         *
         * @param file_name path to file to write
         * @return true Open succeed
         * @return false Open failed
         */
        virtual bool open(std::string url, unsigned port, std::string server_name, foxglove::ServerOptions server_options);
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
         * @brief Create a schema for data corresponding to `channel_name`
         *
         * @param channel_name Channel name of data
         * @param schema Schema of data
         */
        virtual void create_schema(std::string channel_name, nlohmann::json schema) override;
        /**
         * @brief Push sample into file.
         *
         * @param channel_name Channel to which data will be pushed.
         * @param sample Sample of data
         * @param timestamp Timestamp of data
         */
        virtual void push_sample(std::string channel_name, nlohmann::json sample, uint64_t timestamp) override;
       
        // Deffine operator= for std::mutex and std::conditionnal variable
        MCAPWebSocketWriter &operator=(const MCAPWebSocketWriter &object);
        /**
         * @brief Add a callback function that could be called by Foxglove studio
         * 
         * @param cb_function the function in itself
         */
        void add_callback_function(std::function<void(foxglove::WebSocketLogLevel, char const*)> cb_function);
    protected:
        void run(); // Function used for storing data into file

        // Attributes:
        std::unique_ptr<foxglove::ServerInterface<websocketpp::connection_hdl>> _server_writer;  // File writer object
        std::mutex _server_writer_mtx;                                        // Mutex of `server_writer`
        std::queue<std::pair<foxglove::ChannelId,nlohmann::json>> _data_queue;                              // Data FIFO (used by write thread to get data)
        std::mutex _data_queue_mtx;                                         // Mutex of `_data_queue`
        std::thread *_writing_thread;                                       // Writing thread
        bool _continue_writing;                                             // Variable used for indicating to the writing thread if write must continue;
        std::condition_variable _write_notifier;                            // Used for signaling new data to write for writing thread
        std::map<std::string, std::string> _defined_schema;                 // Is usefull for keeping trace of defined schema
        std::map<std::string, Internal3DObject> _all_3d_object;             // Definition of all 3D objects.
        std::map<std::string, std::vector<Eigen::Matrix4f>> _all_positions; // Keep track of all positions for a dedicated channel
        std::vector<std::function<void(foxglove::WebSocketLogLevel, char const*)>> _all_server_callback; // Keep trace of all server callback function
        bool is_server_open = false;                                        // Is server open
        bool is_write_sync = false;                                         // Attribute that is used for knowing if write should be sync
        std::mutex write_is_being_process;                                  // Mutex that is grab during the whole write process
        std::condition_variable write_finished_adviser;                     // Conditional variable that is notified when whole data were wrote
    };

};
#endif
#include "internal/MCAPWebSocketWriter.h"

namespace mcap_wrapper
{
    MCAPWebSocketWriter::MCAPWebSocketWriter()
    {
    }
    MCAPWebSocketWriter::~MCAPWebSocketWriter()
    {
        close();
    }

    bool MCAPWebSocketWriter::open(std::string url, unsigned port, std::string server_name, foxglove::ServerOptions server_options)
    {
        const auto logHandler = [&](foxglove::WebSocketLogLevel message_level, char const* msg) {
            for(auto & cb_fct: _all_server_callback)
                cb_fct(message_level, msg);
        };
        _server_writer = foxglove::ServerFactory::createServer<websocketpp::connection_hdl>(
            server_name, logHandler, server_options);

        foxglove::ServerHandlers<foxglove::ConnHandle> hdlrs;
        hdlrs.subscribeHandler = [&](foxglove::ChannelId chanId, foxglove::ConnHandle clientHandle) {
            const auto clientStr = _server_writer->remoteEndpointString(clientHandle);
            std::cout << "Client " << clientStr << " subscribed to " << chanId << std::endl;
        };
        hdlrs.unsubscribeHandler = [&](foxglove::ChannelId chanId, foxglove::ConnHandle clientHandle) {
            const auto clientStr = _server_writer->remoteEndpointString(clientHandle);
            std::cout << "Client " << clientStr << " unsubscribed from " << chanId << std::endl;
        };
        _server_writer->setHandlers(std::move(hdlrs));
        _server_writer->start(url, port);
        is_server_open = true;

        _writing_thread = new std::thread(&MCAPWebSocketWriter::run, this);
        _continue_writing = true;
        return true;

    }

    void MCAPWebSocketWriter::add_callback_function(std::function<void(foxglove::WebSocketLogLevel, char const*)> cb_function){
        _all_server_callback.push_back(cb_function);
    }

    bool MCAPWebSocketWriter::close()
    {
        if(is_server_open){
            std::vector<foxglove::ChannelId> all_ids_presents;
            for(auto id: _all_channels)
                all_ids_presents.push_back(id.second.id);
            _server_writer->removeChannels(all_ids_presents);
            _server_writer->stop();
            is_server_open = false;
            _continue_writing = false;
            _writing_thread->join();
            delete _writing_thread;
        }
        return true;
    }

    bool MCAPWebSocketWriter::is_open()
    {
        return is_server_open;
    }

    void MCAPWebSocketWriter::create_schema(std::string channel_name, nlohmann::json schema)
    {
        std::lock_guard<std::mutex> file_writer_lg(_server_writer_mtx);
        // Create schema and channel:
        std::string schema_title = channel_name;
        if (schema.count("title"))
            schema_title = schema["title"];

        foxglove::ChannelWithoutId channel;
        channel.topic = channel_name;
        channel.encoding = "json";
        channel.schemaName = schema_title;
        channel.schema = schema.dump();
        std::vector<foxglove::ChannelWithoutId> all_channel_obj;
        all_channel_obj.push_back(channel);
        std::vector<foxglove::ChannelId> channel_ids = _server_writer->addChannels(all_channel_obj);
        // Add it to existing schema:
        for(auto id: channel_ids){
            mcap::Channel c;
            c.id = id;
            _all_channels[channel_name] = c;
        }
    }

    void MCAPWebSocketWriter::push_sample(std::string channel_name, nlohmann::json sample, uint64_t timestamp)
    {
        if (!is_schema_present(channel_name))
            infer_schema(channel_name, sample);

        // If we are in sync mode we wait that current write was proceed
        if(is_write_sync){
            std::lock_guard lg(write_is_being_process);
        }
    
        std::pair<foxglove::ChannelId,nlohmann::json> data_sample;
        data_sample.first = _all_channels[channel_name].id;
        data_sample.second = sample;
        // Push the sample into write queue
        std::lock_guard<std::mutex> data_queue_lg(_data_queue_mtx);
        _data_queue.push(data_sample);
        // Notify writing thread that some work need to be perform:
        _write_notifier.notify_all();

        // If we are in sync mode we wait that data was wrote
        if(is_write_sync){
            std::mutex mtx;
            std::unique_lock ul(mtx);
            write_finished_adviser.wait_for(ul, std::chrono::seconds(1)); // 1 second of timeout to avoid blocking
        }
    }

    // Deffine operator= for std::mutex and std::conditionnal variable
    MCAPWebSocketWriter &MCAPWebSocketWriter::operator=(const MCAPWebSocketWriter &object)
    {
        return *this;
    }

    // Function used to write thing asynchronous
    void MCAPWebSocketWriter::run(){
        while (1)
        {
            std::mutex sleep_until_new_data_mtx;
            std::unique_lock<std::mutex> sleep_until_new_data_ul(sleep_until_new_data_mtx);
            _write_notifier.wait_for(sleep_until_new_data_ul, std::chrono::milliseconds(16));

            // Encode waiting images:
            encode_waiting_images();
            prepare_camera_calibration_messages();
            prepare_raw_message();
            prepare_log();

            // Check ending condition
            if (!_continue_writing && _data_queue.size() == 0) // Check that no data are being waiting to be writted
                break;

            // Take mutex of write is process for inform every sync source that data are currently write:
            std::lock_guard write_is_process(write_is_being_process);

            


            // Protect `_data_queue` and copy it data
            std::queue<std::pair<foxglove::ChannelId,nlohmann::json>> data_to_write;
            {
                std::lock_guard<std::mutex> data_queue_lg(_data_queue_mtx);
                std::swap(_data_queue, data_to_write);
            }

            // Write data:
            while (data_to_write.size())
            {
                // Pop data
                std::pair<foxglove::ChannelId,nlohmann::json> data = data_to_write.front();
                data_to_write.pop();
                // Write it to remote clients
                std::lock_guard<std::mutex> server_writer_lg(_server_writer_mtx);
                auto now = std::chrono::high_resolution_clock::now().time_since_epoch().count();
                std::string serialized_data = data.second.dump();
                _server_writer->broadcastMessage(data.first, now, reinterpret_cast<const uint8_t*>(serialized_data.data()),
                               serialized_data.size());
                
            }

            // Notify all sync that data were wrote
            write_finished_adviser.notify_all();
        }
    }

};
#include "internal/MCAPFileWriter.h"

namespace mcap_wrapper
{
    MCAPFileWriter::MCAPFileWriter()
    {
        _continue_writing = false;
    }

    MCAPFileWriter::~MCAPFileWriter()
    {
        close();
    }

    bool MCAPFileWriter::close()
    {
        if (_continue_writing)
        {
            _continue_writing = false;
            _write_notifier.notify_all();
            _writing_thread->join();
            _file_writer.close();
            delete _writing_thread;
        }
        return true;
    }

    bool MCAPFileWriter::is_open()
    {
        return _continue_writing;
    }

    bool MCAPFileWriter::open(std::string file_name)
    {
        // Close file if it already open
        if (is_open())
            close();

        // Create file (erase previous one if it existed)
        mcap::McapWriterOptions options("");
        mcap::Status open_status = _file_writer.open(file_name, options);
        if (open_status.code == mcap::StatusCode::Success)
        {
            // Create writing thread:
            _writing_thread = new std::thread(&MCAPFileWriter::run, this);
            _continue_writing = true;
            return true;
        }
        else
            std::cerr << "Error occur during the initialization of file " << file_name << ". Error message: " << open_status.message << std::endl;
        return false;
    }


    void MCAPFileWriter::push_sample(std::string channel_name, nlohmann::json sample, uint64_t timestamp)
    {
        if (!is_schema_present(channel_name))
            infer_schema(channel_name, sample);
        
        // If we are in sync mode we wait that current write was proceed
        if(is_write_sync){
            std::lock_guard lg(write_is_being_process);
        }

        mcap::Message msg;
        msg.channelId = _all_channels[channel_name].id;
        // Since we do not know when data is emitted we set `publishTime` equal to `logTime`
        msg.logTime = timestamp;
        msg.publishTime = timestamp;
        msg.sequence = 0; // Not pertinent here
        std::string serialized_sample = sample.dump();
        std::byte *data_bytes = new std::byte[serialized_sample.size()];
        memcpy(data_bytes, reinterpret_cast<const std::byte *>(serialized_sample.data()), serialized_sample.size());
        msg.data = data_bytes;
        msg.dataSize = serialized_sample.size();
        // Push the sample into write queue
        std::lock_guard<std::mutex> data_queue_lg(_data_queue_mtx);
        _data_queue.push(msg);
        // Notify writing thread that some work need to be perform:
        _write_notifier.notify_all();

        // If we are in sync mode we wait that data was wrote
        std::mutex mtx;
        std::unique_lock ul(mtx);
        write_finished_adviser.wait_for(ul, std::chrono::seconds(1)); // 1 second of timeout to avoid blocking
    }

    //
    // Protected methods
    //
    void MCAPFileWriter::run()
    {
        while (1)
        {
            std::mutex sleep_until_new_data_mtx;
            std::unique_lock<std::mutex> sleep_until_new_data_ul(sleep_until_new_data_mtx);
            _write_notifier.wait_for(sleep_until_new_data_ul, std::chrono::milliseconds(16));

            // Check ending condition
            if (!_continue_writing && _data_queue.size() == 0) // Check that no data are being waiting to be writted
                break;

            // Take mutex of write is process for inform every sync source that data are currently write:
            std::lock_guard write_is_process(write_is_being_process);

            // Protect `_data_queue` and copy it data
            std::queue<mcap::Message> data_to_write;
            {
                std::lock_guard<std::mutex> data_queue_lg(_data_queue_mtx);
                std::swap(_data_queue, data_to_write);
            }

            // Write data:
            while (data_to_write.size())
            {
                // Pop data
                mcap::Message data = data_to_write.front();
                data_to_write.pop();
                // Write it to file
                std::lock_guard<std::mutex> file_writer_lg(_file_writer_mtx);
                mcap::Status write_status = _file_writer.write(data);
                if (write_status.code != mcap::StatusCode::Success)
                    std::cerr << "Error occur in MCAP message writing. Message: " << write_status.message << std::endl;
                // Delete previous allocated data:
                delete data.data;
            }

            // Notify all sync that data were wrote
            write_finished_adviser.notify_all();
        }
    }

    void MCAPFileWriter::create_schema(std::string channel_name, nlohmann::json schema)
    {
        std::lock_guard<std::mutex> file_writer_lg(_file_writer_mtx);
        
        // Create schema and channel:
        std::string schema_title = channel_name;
        if (schema.count("title"))
            schema_title = schema["title"];
        mcap::Schema schema_obj(schema_title, "jsonschema", schema.dump());
        _file_writer.addSchema(schema_obj);
        mcap::Channel channel_obj(channel_name, "json", schema_obj.id);
        _file_writer.addChannel(channel_obj);
        // Add it to existing schema:
        _all_channels[channel_name] = channel_obj;
    }
   

    MCAPFileWriter &MCAPFileWriter::operator=(const MCAPFileWriter &object)
    {
        return *this;
    }
};
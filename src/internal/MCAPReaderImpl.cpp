#include "internal/MCAPReaderImpl.h"

namespace mcap_wrapper
{
    MCAPReaderImpl::MCAPReaderImpl(std::string file_path)
    {
        // Open file:
        mcap::Status open_status = _file_reader.open(file_path.c_str());
        _is_file_open = true;
        if (open_status.code != mcap::StatusCode::Success)
        {
            _is_file_open = false;
            std::cerr << "Could not open MCAP file: " << file_path << std::endl;
        }
        // Retriving channels and corresponding types:
        if (_is_file_open)
        {
            // Read summary file (for gettings channels and schema)
            mcap::Status read_summary_status = _file_reader.readSummary(mcap::ReadSummaryMethod::NoFallbackScan);
            if (read_summary_status.code != mcap::StatusCode::Success)
            {
                read_summary_status = _file_reader.readSummary(mcap::ReadSummaryMethod::ForceScan);
            }
            // Iterate over channel for:
            //                  - Get type of it
            //                  - Get iterator
            if (read_summary_status.code == mcap::StatusCode::Success)
            {
                std::unordered_map<mcap::ChannelId, mcap::ChannelPtr> all_channels = _file_reader.channels();
                for (auto [channel_id, channel_ptr] : all_channels)
                {
                    // Determine type of channel:
                    std::string channel_name = channel_ptr->topic; // Channel name
                    mcap::SchemaId channel_shema_id = channel_ptr->schemaId;
                    mcap::SchemaPtr corresponding_schema = _file_reader.schema(channel_shema_id);
                    std::string schema_name = corresponding_schema->name;
                    // Parse type from schema name
                    MCAPReaderChannelType corresponding_type = MCAPReaderChannelType::RAW_JSON;
                    if (schema_name == "foxglove.CompressedImage")
                        corresponding_type = MCAPReaderChannelType::IMAGE;
                    else if (schema_name == "foxglove.Log")
                        corresponding_type = MCAPReaderChannelType::LOG;
                    else if (schema_name == "foxglove.SceneUpdate")
                        corresponding_type = MCAPReaderChannelType::OBJECT_3D;
                    else if (schema_name == "foxglove.FrameTransforms")
                        corresponding_type = MCAPReaderChannelType::TRANSFORM;
                    // Add it to type:
                    _channels_description[channel_name] = corresponding_type;

                    // Get iterator
                    mcap::ReadMessageOptions read_channel_options;
                    read_channel_options.topicFilter = [=](std::string_view read_channel_name)
                    {
                        if (channel_name == read_channel_name)
                            return true;
                        return false;
                    };

                    std::shared_ptr<mcap::LinearMessageView> channel_message_view = std::make_shared<mcap::LinearMessageView>(_file_reader.readMessages([](const mcap::Status &status) {}, read_channel_options));
                    _channel_message_view[channel_name] = channel_message_view;
                    std::shared_ptr<mcap::LinearMessageView::Iterator> channel_iterator = std::make_shared<mcap::LinearMessageView::Iterator>(channel_message_view->begin());
                    _channel_iterator[channel_name] = channel_iterator;
                }
            }
            else
            {
                std::cerr << "Could not get summary from file. Retriving summary from MCAP is required for reading file." << std::endl;
            }
        }
    }

    MCAPReaderImpl::~MCAPReaderImpl()
    {
        if (_is_file_open)
            _file_reader.close();
    }

    std::map<std::string, MCAPReaderChannelType> MCAPReaderImpl::get_channels()
    {
        return _channels_description;
    }

    bool MCAPReaderImpl::get_next_message(std::string channel_name, std::string &out_message)
    {
        if (!_is_file_open)
            return false; // File is not open
        if (!_channels_description.count(channel_name))
            return false; // Channel not present in file
        if (*_channel_iterator[channel_name] == _channel_message_view[channel_name]->end())
            return false; // No more message on this channel
        // Read message:
        out_message = std::string(reinterpret_cast<const char *>((*_channel_iterator[channel_name])->message.data),
                                  (*_channel_iterator[channel_name])->message.dataSize);

        // Increment iterators:
        (*_channel_iterator[channel_name])++;
        return true;
    }

    bool MCAPReaderImpl::get_next_image(std::string channel_name, cv::Mat &out_image)
    {
        if (!_is_file_open)
            return false; // File is not open
        if (!_channels_description.count(channel_name))
            return false; // Channel not present in file
        if (_channels_description[channel_name] != MCAPReaderChannelType::IMAGE)
            return false; // Channel is not image type
        if (*_channel_iterator[channel_name] == _channel_message_view[channel_name]->end())
            return false; // No more message on this channel
        // Read message:
        std::string message(reinterpret_cast<const char *>((*_channel_iterator[channel_name])->message.data),
                                  (*_channel_iterator[channel_name])->message.dataSize);
        // Parse it:
        nlohmann::json parsed_message = nlohmann::json::parse(message);
        if(!parsed_message.count("data"))
            return false; // No image present for this data

        std::vector<uchar> encoded_buffer = base64::decode_into<std::vector<uchar>>(parsed_message["data"].get<std::string>());
        out_image = cv::imdecode(encoded_buffer, cv::IMREAD_UNCHANGED);

        // Increment iterators:
        (*_channel_iterator[channel_name])++;
        return true;
    }

    bool MCAPReaderImpl::get_next_logs(std::string channel_name, std::string &out_log)
    {
        if (!_is_file_open)
            return false; // File is not open
        if (!_channels_description.count(channel_name))
            return false; // Channel not present in file
        if (_channels_description[channel_name] != MCAPReaderChannelType::LOG)
            return false; // Channel is not log type
        if (*_channel_iterator[channel_name] == _channel_message_view[channel_name]->end())
            return false; // No more message on this channel
        // Read message:
        std::string message(reinterpret_cast<const char *>((*_channel_iterator[channel_name])->message.data),
                                  (*_channel_iterator[channel_name])->message.dataSize);
        // Parse it:
        nlohmann::json parsed_message = nlohmann::json::parse(message);
        if(!parsed_message.count("message"))
            return false; // No log message present for this data
        out_log = parsed_message["message"];
        // Increment iterators:
        (*_channel_iterator[channel_name])++;
        return true;
    }
};
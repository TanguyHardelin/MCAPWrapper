#include "internal/MCAPWebSocketWriter.h"

namespace mcap_wrapper
{
    MCAPWebSocketWriter::MCAPWebSocketWriter()
    {
    }
    MCAPWebSocketWriter::~MCAPWebSocketWriter()
    {
        close();
        delete _writing_thread;
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
                all_ids_presents.push_back(id.second);
            _server_writer->removeChannels(all_ids_presents);
            _server_writer->stop();
            is_server_open = false;
            _continue_writing = false;
            _writing_thread->join();
        }
        return true;
    }

    bool MCAPWebSocketWriter::is_open()
    {
        return is_server_open;
    }

    bool MCAPWebSocketWriter::is_schema_present(std::string channel_name)
    {
        return _all_channels.count(channel_name);
    }

    void MCAPWebSocketWriter::infer_schema(std::string channel_name, nlohmann::json sample)
    {
        // Check if additionnal informations are provided:
        std::string description = "Geneated by wrapper based on provided JSON";
        if (sample.count("__private_foxglove_description__"))
            description = sample["__private_foxglove_description__"];
        std::string comment = "";
        if (sample.count("__private_foxglove_comment__"))
            comment = sample["__private_foxglove_comment__"];

        // Create schema:
        nlohmann::json foxglove_schema;
        foxglove_schema["title"] = channel_name;
        foxglove_schema["description"] = description;
        foxglove_schema["$comment"] = comment;
        foxglove_schema["type"] = "object";
        foxglove_schema["properties"] = infer_property_of_sample(sample);

        create_schema(channel_name, foxglove_schema);
    }

    void MCAPWebSocketWriter::create_schema(std::string channel_name, nlohmann::json schema)
    {
        std::lock_guard<std::mutex> file_writer_lg(_server_writer_mtx);
        // Create schema and channel:
        std::string schema_title = channel_name;
        if (schema.count("title"))
            schema_title = schema["title"];

        foxglove::ChannelWithoutId channel;
        channel.topic = schema_title;
        channel.encoding = "json";
        channel.schemaName = schema_title;
        channel.schema = schema.dump();
        std::vector<foxglove::ChannelWithoutId> all_channel_obj;
        all_channel_obj.push_back(channel);
        std::vector<foxglove::ChannelId> channel_ids = _server_writer->addChannels(all_channel_obj);
        // Add it to existing schema:
        for(auto id: channel_ids)
            _all_channels[channel_name] = id;
    }

    void MCAPWebSocketWriter::push_sample(std::string channel_name, nlohmann::json sample, uint64_t timestamp)
    {
        if (!is_schema_present(channel_name))
            infer_schema(channel_name, sample);
        std::pair<foxglove::ChannelId,nlohmann::json> data_sample;
        data_sample.first = _all_channels[channel_name];
        data_sample.second = sample;
        // Push the sample into write queue
        std::lock_guard<std::mutex> data_queue_lg(_data_queue_mtx);
        _data_queue.push(data_sample);
        // Notify writing thread that some work need to be perform:
        _write_notifier.notify_all();
    }

    void MCAPWebSocketWriter::create_3D_object(std::string object_name, std::string frame_id, bool frame_locked)
    {
        _all_3d_object[object_name] = Internal3DObject (object_name, frame_id, frame_locked);
    }

    bool MCAPWebSocketWriter::add_metadata_to_3d_object(std::string object_name, std::pair<std::string, std::string> metadata)
    {
        if(_all_3d_object.count(object_name) == 0)
            return false;
        return _all_3d_object[object_name].add_metadata(metadata);
    }

    bool MCAPWebSocketWriter::add_arrow_to_3d_object(std::string object_name,
                                                     Eigen::Matrix4f pose,
                                                     double shaft_length,
                                                     double shaft_diameter,
                                                     double head_length,
                                                     double head_diameter,
                                                     std::array<double, 4> color)
    {
        if(_all_3d_object.count(object_name) == 0)
            return false;
        return _all_3d_object[object_name].add_arrow(pose, shaft_length, shaft_diameter, head_length, head_diameter, color);
    }

    bool MCAPWebSocketWriter::add_cube_to_3d_object(std::string object_name,
                                                    Eigen::Matrix4f pose,
                                                    std::array<double, 3> size,
                                                    std::array<double, 4> color)
    {
        if(_all_3d_object.count(object_name) == 0)
            return false;
        return _all_3d_object[object_name].add_cube(pose, size, color);
    }

    bool MCAPWebSocketWriter::add_sphere_to_3d_object(std::string object_name,
                                                      Eigen::Matrix4f pose,
                                                      std::array<double, 3> size,
                                                      std::array<double, 4> color)
    {
        if(_all_3d_object.count(object_name) == 0)
            return false;
        return _all_3d_object[object_name].add_sphere(pose, size, color);
    }

    bool MCAPWebSocketWriter::add_cylinder_to_3d_object(std::string object_name,
                                                        Eigen::Matrix4f pose,
                                                        double bottom_scale,
                                                        double top_scale,
                                                        std::array<double, 3> size,
                                                        std::array<double, 4> color)
    {
        if(_all_3d_object.count(object_name) == 0)
            return false;
        return _all_3d_object[object_name].add_cylinder(pose, bottom_scale, top_scale, size, color);
    }

    bool MCAPWebSocketWriter::add_line_to_3d_object(std::string object_name,
                                                    Eigen::Matrix4f pose,
                                                    double thickness,
                                                    bool scale_invariant,
                                                    std::vector<Eigen::Vector3d> points,
                                                    std::array<double, 4> color,
                                                    std::vector<std::array<double, 4>> colors,
                                                    std::vector<uint32_t> indices)
    {
        if(_all_3d_object.count(object_name) == 0)
            return false;
        return _all_3d_object[object_name].add_line(pose, thickness, scale_invariant, points, color, colors, indices);
    }

    bool MCAPWebSocketWriter::add_triangle_to_3d_object(std::string object_name,
                                                        Eigen::Matrix4f pose,
                                                        std::vector<Eigen::Vector3d> points,
                                                        std::array<double, 4> color,
                                                        std::vector<std::array<double, 4>> colors,
                                                        std::vector<uint32_t> indices)
    {
        if(_all_3d_object.count(object_name) == 0)
            return false;
        return _all_3d_object[object_name].add_triangle(pose, points, color, colors, indices);
    }

    bool MCAPWebSocketWriter::add_text_to_3d_object(std::string object_name,
                                                    Eigen::Matrix4f pose,
                                                    bool billboard,
                                                    double font_size,
                                                    bool scale_invariant,
                                                    std::array<double, 4> color,
                                                    std::string text)
    {
        if(_all_3d_object.count(object_name) == 0)
            return false;
        return _all_3d_object[object_name].add_text(pose, billboard, font_size, scale_invariant, color, text);
    }

    bool MCAPWebSocketWriter::write_3d_object_to_all(std::string object_name, uint64_t timestamp)
    {
        if(_all_3d_object.count(object_name) == 0)
            return false;
        // Set timestamp of current object:
        _all_3d_object[object_name].set_timestamp(timestamp);
        // Create Scene update object:
        nlohmann::json scene_update_json;
        scene_update_json["deletions"] = std::vector<nlohmann::json>();
        scene_update_json["entities"] = std::vector<nlohmann::json>();
        // Delete entity if it was present before
        nlohmann::json deletion_json;
        deletion_json["timestamp"] = nlohmann::json();
        deletion_json["timestamp"]["sec"] = (uint64_t) timestamp / (uint64_t) 1e9;
        deletion_json["timestamp"]["nsec"] = (uint64_t) timestamp % (uint64_t) 1e9;
        deletion_json["type"] = 0;
        deletion_json["id"] = _all_3d_object[object_name].get_id();
        scene_update_json["deletions"].push_back(deletion_json);
        // Re-add entity in scene
        scene_update_json["entities"].push_back(_all_3d_object[object_name].get_description());
        push_sample(object_name, scene_update_json, timestamp);
        return true;
    }

    bool MCAPWebSocketWriter::add_position_to_all(std::string position_channel_name, uint64_t timestamp, Eigen::Matrix4f pose, std::string frame_id)
    {
        // Add position into list of positions
        if(!_all_positions.count(position_channel_name))
            _all_positions[position_channel_name] = std::vector<Eigen::Matrix4f>();
        _all_positions[position_channel_name].push_back(pose);

        // Construct message
        nlohmann::json pose_in_frame_json;
        pose_in_frame_json["timestamp"] = nlohmann::json();
        pose_in_frame_json["timestamp"]["sec"] = (uint64_t)timestamp / (uint64_t) 1e9;
        pose_in_frame_json["timestamp"]["nsec"] = (uint64_t)timestamp % (uint64_t) 1e9;
        pose_in_frame_json["frame_id"] = frame_id;
        pose_in_frame_json["poses"] = std::vector<nlohmann::json>();
        for(unsigned i=0; i<_all_positions[position_channel_name].size(); i++){
            Eigen::Matrix4f position = _all_positions[position_channel_name][i];
            nlohmann::json position_json;
            position_json["translation"] = nlohmann::json();
            position_json["translation"]["x"] = position(0, 3);
            position_json["translation"]["y"] = position(1, 3);
            position_json["translation"]["z"] = position(2, 3);
            Eigen::Quaternionf orientation_eigen(position.block<3,3>(0,0));
            position_json["orientation"] = nlohmann::json();
            position_json["orientation"]["x"] = orientation_eigen.x();
            position_json["orientation"]["y"] = orientation_eigen.y();
            position_json["orientation"]["z"] = orientation_eigen.z();
            position_json["orientation"]["w"] = orientation_eigen.w();
            pose_in_frame_json["poses"].push_back(position_json);
        }

        // Write it to file
        push_sample(position_channel_name, pose_in_frame_json, timestamp);

        return true;
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

            // Check ending condition
            if (!_continue_writing && _data_queue.size() == 0) // Check that no data are being waiting to be writted
                break;

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
        }
    }
};
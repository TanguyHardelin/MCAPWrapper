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
            delete _writing_thread;
            _file_writer.close();
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

    bool MCAPFileWriter::is_schema_present(std::string channel_name)
    {
        return _all_channels.count(channel_name);
    }

    void MCAPFileWriter::infer_schema(std::string channel_name, nlohmann::json sample)
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

    void MCAPFileWriter::push_sample(std::string channel_name, nlohmann::json sample, uint64_t timestamp)
    {
        if (!is_schema_present(channel_name))
            infer_schema(channel_name, sample);
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
        }
    }

    nlohmann::json MCAPFileWriter::infer_property_of_sample(nlohmann::json sample, bool recursive_call)
    {
        nlohmann::json out;
        for (auto &kv : sample.items())
        {
            // Handle string case
            if (kv.value().is_string())
            {
                out[kv.key()] = nlohmann::json::object();
                out[kv.key()]["type"] = "string";
                out[kv.key()]["comment"] = "Generated by wrapper";
            }
            // Handle boolean case
            else if (kv.value().is_boolean())
            {
                out[kv.key()] = nlohmann::json::object();
                out[kv.key()]["type"] = "boolean";
                out[kv.key()]["comment"] = "Generated by wrapper";
            }
            // Handle bytes case
            else if (kv.value().is_binary())
            {
                out[kv.key()] = nlohmann::json::object();
                out[kv.key()]["type"] = "bytes";
                out[kv.key()]["comment"] = "Generated by wrapper";
            }
            // Handle float case
            else if (kv.value().is_number_float())
            {
                out[kv.key()] = nlohmann::json::object();
                out[kv.key()]["type"] = "number";
                out[kv.key()]["comment"] = "Generated by wrapper";
            }
            // Handle uint32 case
            else if (kv.value().is_number_unsigned())
            {
                out[kv.key()] = nlohmann::json::object();
                out[kv.key()]["type"] = "number";
                out[kv.key()]["comment"] = "Generated by wrapper";
            }
            // Handle int32 case
            else if (kv.value().is_number_integer())
            {
                out[kv.key()] = nlohmann::json::object();
                out[kv.key()]["type"] = "number";
                out[kv.key()]["comment"] = "Generated by wrapper";
            }
            // Handle other number types:
            else if (kv.value().is_number())
            {
                out[kv.key()] = nlohmann::json::object();
                out[kv.key()]["type"] = "number";
                out[kv.key()]["comment"] = "Generated by wrapper";
            }
            // Handle recursive schema:
            else if (kv.value().is_object())
            {
                out[kv.key()] = nlohmann::json::object();
                out[kv.key()]["type"] = "object";
                out[kv.key()]["comment"] = "Generated by wrapper";
                out[kv.key()]["properties"] = infer_property_of_sample(kv.value(), true);
            }
        }

        return out;
    }
    void MCAPFileWriter::create_3D_object(std::string object_name, std::string frame_id, bool frame_locked)
    {
        _all_3d_object[object_name] = Internal3DObject (object_name, frame_id, frame_locked);
    }

    bool MCAPFileWriter::add_metadata_to_3d_object(std::string object_name, std::pair<std::string, std::string> metadata)
    {
        if(_all_3d_object.count(object_name) == 0)
            return false;
        return _all_3d_object[object_name].add_metadata(metadata);
    }

    bool MCAPFileWriter::add_arrow_to_3d_object(std::string object_name,
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

    bool MCAPFileWriter::add_cube_to_3d_object(std::string object_name,
                                               Eigen::Matrix4f pose,
                                               std::array<double, 3> size,
                                               std::array<double, 4> color)
    {
        if(_all_3d_object.count(object_name) == 0)
            return false;
        return _all_3d_object[object_name].add_cube(pose, size, color);
    }

    bool MCAPFileWriter::add_sphere_to_3d_object(std::string object_name,
                                                 Eigen::Matrix4f pose,
                                                 std::array<double, 3> size,
                                                 std::array<double, 4> color)
    {
        if(_all_3d_object.count(object_name) == 0)
            return false;
        return _all_3d_object[object_name].add_sphere(pose, size, color);
    }

    bool MCAPFileWriter::add_cylinder_to_3d_object(std::string object_name,
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

    bool MCAPFileWriter::add_line_to_3d_object(std::string object_name,
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

    bool MCAPFileWriter::add_triangle_to_3d_object(std::string object_name,
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

    bool MCAPFileWriter::add_text_to_3d_object(std::string object_name,
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
    
    bool MCAPFileWriter::write_3d_object_to_all(std::string object_name, uint64_t timestamp)
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

    bool MCAPFileWriter::add_position_to_all(std::string position_channel_name, uint64_t timestamp, Eigen::Matrix4f pose, std::string frame_id){
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

    MCAPFileWriter &MCAPFileWriter::operator=(const MCAPFileWriter &object)
    {
        return *this;
    }
};
#include "internal/IWriter.h"
#include "internal/FoxgloveSchema.hpp"

namespace mcap_wrapper
{

    bool IWriter::is_schema_present(std::string channel_name)
    {
        return _all_channels.count(channel_name);
    }

    void IWriter::infer_schema(std::string channel_name, nlohmann::json sample)
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

    void IWriter::set_sync(bool sync)
    {
        is_write_sync = sync;
    }

    void IWriter::create_3D_object(std::string object_name, std::string frame_id, bool frame_locked)
    {
        _all_3d_object[object_name] = Internal3DObject(object_name, frame_id, frame_locked);
    }

    bool IWriter::add_metadata_to_3d_object(std::string object_name, std::pair<std::string, std::string> metadata)
    {
        if (_all_3d_object.count(object_name) == 0)
            return false;
        return _all_3d_object[object_name].add_metadata(metadata);
    }

    bool IWriter::add_arrow_to_3d_object(std::string object_name,
                                         Eigen::Matrix4f pose,
                                         double shaft_length,
                                         double shaft_diameter,
                                         double head_length,
                                         double head_diameter,
                                         std::array<double, 4> color)
    {
        if (_all_3d_object.count(object_name) == 0)
            return false;
        return _all_3d_object[object_name].add_arrow(pose, shaft_length, shaft_diameter, head_length, head_diameter, color);
    }

    bool IWriter::add_cube_to_3d_object(std::string object_name,
                                        Eigen::Matrix4f pose,
                                        std::array<double, 3> size,
                                        std::array<double, 4> color)
    {
        if (_all_3d_object.count(object_name) == 0)
            return false;
        return _all_3d_object[object_name].add_cube(pose, size, color);
    }

    bool IWriter::add_sphere_to_3d_object(std::string object_name,
                                          Eigen::Matrix4f pose,
                                          std::array<double, 3> size,
                                          std::array<double, 4> color)
    {
        if (_all_3d_object.count(object_name) == 0)
            return false;
        return _all_3d_object[object_name].add_sphere(pose, size, color);
    }

    bool IWriter::add_cylinder_to_3d_object(std::string object_name,
                                            Eigen::Matrix4f pose,
                                            double bottom_scale,
                                            double top_scale,
                                            std::array<double, 3> size,
                                            std::array<double, 4> color)
    {
        if (_all_3d_object.count(object_name) == 0)
            return false;
        return _all_3d_object[object_name].add_cylinder(pose, bottom_scale, top_scale, size, color);
    }

    bool IWriter::add_line_to_3d_object(std::string object_name,
                                        Eigen::Matrix4f pose,
                                        double thickness,
                                        bool scale_invariant,
                                        std::vector<Eigen::Vector3d> points,
                                        std::array<double, 4> color,
                                        std::vector<std::array<double, 4>> colors,
                                        std::vector<uint32_t> indices)
    {
        if (_all_3d_object.count(object_name) == 0)
            return false;
        return _all_3d_object[object_name].add_line(pose, thickness, scale_invariant, points, color, colors, indices);
    }

    bool IWriter::add_triangle_to_3d_object(std::string object_name,
                                            Eigen::Matrix4f pose,
                                            std::vector<Eigen::Vector3d> points,
                                            std::array<double, 4> color,
                                            std::vector<std::array<double, 4>> colors,
                                            std::vector<uint32_t> indices)
    {
        if (_all_3d_object.count(object_name) == 0)
            return false;
        return _all_3d_object[object_name].add_triangle(pose, points, color, colors, indices);
    }

    bool IWriter::add_text_to_3d_object(std::string object_name,
                                        Eigen::Matrix4f pose,
                                        bool billboard,
                                        double font_size,
                                        bool scale_invariant,
                                        std::array<double, 4> color,
                                        std::string text)
    {
        if (_all_3d_object.count(object_name) == 0)
            return false;
        return _all_3d_object[object_name].add_text(pose, billboard, font_size, scale_invariant, color, text);
    }

    bool IWriter::write_3d_object_to_all(std::string object_name, uint64_t timestamp)
    {
        if (_all_3d_object.count(object_name) == 0)
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
        deletion_json["timestamp"]["sec"] = (uint64_t)timestamp / (uint64_t)1e9;
        deletion_json["timestamp"]["nsec"] = (uint64_t)timestamp % (uint64_t)1e9;
        deletion_json["type"] = 0;
        deletion_json["id"] = _all_3d_object[object_name].get_id();
        scene_update_json["deletions"].push_back(deletion_json);
        // Re-add entity in scene
        scene_update_json["entities"].push_back(_all_3d_object[object_name].get_description());
        push_sample(object_name, scene_update_json, timestamp);
        return true;
    }

    bool IWriter::add_position_to_all(std::string position_channel_name, uint64_t timestamp, Eigen::Matrix4f pose, std::string frame_id)
    {
        // Add position into list of positions
        if (!_all_positions.count(position_channel_name))
            _all_positions[position_channel_name] = std::vector<Eigen::Matrix4f>();
        _all_positions[position_channel_name].push_back(pose);

        // Construct message
        nlohmann::json pose_in_frame_json;
        pose_in_frame_json["timestamp"] = nlohmann::json();
        pose_in_frame_json["timestamp"]["sec"] = (uint64_t)timestamp / (uint64_t)1e9;
        pose_in_frame_json["timestamp"]["nsec"] = (uint64_t)timestamp % (uint64_t)1e9;
        pose_in_frame_json["frame_id"] = frame_id;
        pose_in_frame_json["poses"] = std::vector<nlohmann::json>();
        for (unsigned i = 0; i < _all_positions[position_channel_name].size(); i++)
        {
            Eigen::Matrix4f position = _all_positions[position_channel_name][i];
            nlohmann::json position_json;
            position_json["translation"] = nlohmann::json();
            position_json["translation"]["x"] = position(0, 3);
            position_json["translation"]["y"] = position(1, 3);
            position_json["translation"]["z"] = position(2, 3);
            Eigen::Quaternionf orientation_eigen(position.block<3, 3>(0, 0));
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

    void IWriter::encode_waiting_images()
    {
        // Copy data:
        std::vector<ImageWaitingToBeEncoded> image_waiting_to_be_encoded;
        {
            std::lock_guard<std::mutex> lg(_image_waiting_to_be_encoded_mtx);
            image_waiting_to_be_encoded = std::vector<ImageWaitingToBeEncoded>(_image_waiting_to_be_encoded.begin(), _image_waiting_to_be_encoded.end());
            _image_waiting_to_be_encoded.clear();
        }
        for (auto &image_data : image_waiting_to_be_encoded)
        {
            // Retrieve all image data:
            std::string identifier = image_data.identifier;
            cv::Mat image = image_data.image;
            uint64_t timestamp = image_data.timestamp;
            std::string frame_id = image_data.frame_id;

            // Encode image for all connections:
            std::vector<int> compression_params;
            compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
            compression_params.push_back(95); // Adjust the quality (0-100), higher is better quality
            std::vector<uchar> encoding_buffer;
            cv::imencode(".jpg", image, encoding_buffer, compression_params);
            std::string image_base64_encoded = base64::to_base64(std::string(encoding_buffer.begin(), encoding_buffer.end()));

            // Create message:
            nlohmann::json image_sample;
            image_sample["timestamp"] = nlohmann::json();
            image_sample["timestamp"]["sec"] = timestamp / (uint64_t)1e9;
            image_sample["timestamp"]["nsec"] = timestamp % (uint64_t)1e9;
            image_sample["frame_id"] = frame_id;
            image_sample["data"] = image_base64_encoded;
            image_sample["format"] = "jpeg";

            // Write it to all connections:
            if (!is_schema_present(identifier))
            {
                nlohmann::json schema_unserialized = nlohmann::json::parse(get_compressed_image_schema());
                create_schema(identifier, schema_unserialized);
            }
            push_sample(identifier, image_sample, timestamp);
        }
    }

    void IWriter::write_image(std::string const &identifier, cv::Mat const &image, uint64_t timestamp, std::string const &frame_id)
    {
        std::lock_guard<std::mutex> lg(_image_waiting_to_be_encoded_mtx);
        ImageWaitingToBeEncoded image_to_encode;
        image_to_encode.identifier = identifier;
        image_to_encode.image = image;
        image_to_encode.timestamp = timestamp;
        image_to_encode.frame_id = frame_id;
        _image_waiting_to_be_encoded.push_back(image_to_encode);
    }

    void IWriter::prepare_camera_calibration_messages()
    {
        // Copy data:
        std::vector<CameraCalibration> camera_calibration_waiting_to_be_encoded;
        {
            std::lock_guard<std::mutex> lg(_camera_calibration_waiting_to_be_encoded_mtx);
            camera_calibration_waiting_to_be_encoded = std::vector<CameraCalibration>(_camera_calibration_waiting_to_be_encoded.begin(), _camera_calibration_waiting_to_be_encoded.end());
            _camera_calibration_waiting_to_be_encoded.clear();
        }
        for (auto calibration_to_encode : camera_calibration_waiting_to_be_encoded)
        {
            // Retrieve calibration data:
            std::string camera_identifier = calibration_to_encode.camera_identifier;
            uint64_t timestamp = calibration_to_encode.timestamp;
            std::string frame_id = calibration_to_encode.frame_id;
            unsigned image_width = calibration_to_encode.image_width;
            unsigned image_height = calibration_to_encode.image_height;
            std::string distortion_model = calibration_to_encode.distortion_model;
            std::array<double, 5> D = calibration_to_encode.D;
            std::array<double, 9> K = calibration_to_encode.K;
            std::array<double, 9> R = calibration_to_encode.R;
            std::array<double, 12> P = calibration_to_encode.P;

            nlohmann::json camera_calibration;
            camera_calibration["timestamp"] = nlohmann::json();
            camera_calibration["timestamp"]["sec"] = (uint64_t)timestamp / (uint64_t)1e9;
            camera_calibration["timestamp"]["nsec"] = (uint64_t)timestamp % (uint64_t)1e9;
            camera_calibration["frame_id"] = frame_id;
            camera_calibration["width"] = image_width;
            camera_calibration["height"] = image_height;
            camera_calibration["distortion_model"] = distortion_model;
            camera_calibration["D"] = D;
            camera_calibration["K"] = K;
            camera_calibration["R"] = R;
            camera_calibration["P"] = P;

            // Create schema if not present:
            if (!is_schema_present(camera_identifier))
            {
                nlohmann::json camera_calibration_foxglove_schema = nlohmann::json::parse(get_camera_calibration_schema());
                create_schema(camera_identifier, camera_calibration_foxglove_schema);
            }
            push_sample(camera_identifier, camera_calibration, timestamp);
        }
    }

    void IWriter::write_camera_calibration(std::string const &camera_identifier,
                                           uint64_t timestamp,
                                           std::string const &frame_id,
                                           unsigned image_width,
                                           unsigned image_height,
                                           std::string const &distortion_model,
                                           std::array<double, 5> const &D,
                                           std::array<double, 9> const &K,
                                           std::array<double, 9> const &R,
                                           std::array<double, 12> const &P)
    {
        std::lock_guard<std::mutex> lg(_camera_calibration_waiting_to_be_encoded_mtx);
        CameraCalibration calibration_to_encode;
        calibration_to_encode.camera_identifier = camera_identifier;
        calibration_to_encode.timestamp = timestamp;
        calibration_to_encode.frame_id = frame_id;
        calibration_to_encode.image_width = image_width;
        calibration_to_encode.image_height = image_height;
        calibration_to_encode.distortion_model = distortion_model;
        calibration_to_encode.D = D;
        calibration_to_encode.K = K;
        calibration_to_encode.R = R;
        calibration_to_encode.P = P;
        _camera_calibration_waiting_to_be_encoded.push_back(calibration_to_encode);
    }

    void IWriter::prepare_raw_message()
    {
        // Copy data:
        std::vector<RawMessage> raw_message_waiting_to_be_encoded;
        {
            std::lock_guard<std::mutex> lg(_raw_message_waiting_to_be_encoded_mtx);
            raw_message_waiting_to_be_encoded = std::vector<RawMessage>(_raw_message_waiting_to_be_encoded.begin(), _raw_message_waiting_to_be_encoded.end());
            _raw_message_waiting_to_be_encoded.clear();
        }

        for (auto raw_message : raw_message_waiting_to_be_encoded)
        {
            std::string serialized_json = raw_message.serialized_message;
            std::string identifier = raw_message.identifier;
            uint64_t timestamp = raw_message.timestamp;

            nlohmann::json unserialiazed_json;
            try
            {
                unserialiazed_json = nlohmann::json::parse(serialized_json);
            }
            catch (std::exception e)
            { // Parse error:
                std::cerr << "[MCAPWrapper] ERROR: failed to parse " << serialized_json << std::endl;
                return;
            }

            push_sample(identifier, unserialiazed_json, timestamp);
        }
    }

    void IWriter::write_raw_message(std::string const &identifier, std::string const &serialized_message, uint64_t timestamp)
    {
        std::lock_guard<std::mutex> lg(_raw_message_waiting_to_be_encoded_mtx);
        RawMessage raw_message_to_be_encode;
        raw_message_to_be_encode.identifier = identifier;
        raw_message_to_be_encode.serialized_message = serialized_message;
        raw_message_to_be_encode.timestamp = timestamp;
        _raw_message_waiting_to_be_encoded.push_back(raw_message_to_be_encode);
    }

    void IWriter::prepare_log()
    {
        std::vector<Log> log_waiting_to_be_encoded;
        {
            std::lock_guard<std::mutex> lg(_log_waiting_to_be_encoded_mtx);
            log_waiting_to_be_encoded = std::vector<Log>(_log_waiting_to_be_encoded.begin(), _log_waiting_to_be_encoded.end());
            _log_waiting_to_be_encoded.clear();
        }

        for (auto log_message : log_waiting_to_be_encoded)
        {
            std::string log_channel_name = log_message.identifier;
            uint64_t timestamp = log_message.timestamp;
            int log_level = log_message.log_level;
            std::string message = log_message.message;
            std::string name = log_message.name;
            std::string file = log_message.file;
            uint32_t line = log_message.line;

            nlohmann::json log_json;
            log_json["timestamp"] = nlohmann::json();
            log_json["timestamp"]["sec"] = (uint64_t)timestamp / (uint64_t)1e9;
            log_json["timestamp"]["nsec"] = (uint64_t)timestamp % (uint64_t)1e9;
            log_json["level"] = log_level;
            log_json["message"] = message;
            log_json["name"] = name;
            log_json["file"] = file;
            log_json["line"] = line;

            // Create schema if not present:
            if (!is_schema_present(log_channel_name))
            {
                std::cout << "Push schema" << std::endl;
                nlohmann::json schema_unserialized = nlohmann::json::parse(get_log_schema());
                create_schema(log_channel_name, schema_unserialized);
            }

            push_sample(log_channel_name, log_json, timestamp);
        }
    }

    void IWriter::write_log(std::string const &log_channel_name, uint64_t timestamp, int log_level, std::string const &message, std::string const &name, std::string const &file, uint32_t line)
    {
        std::lock_guard<std::mutex> lg(_log_waiting_to_be_encoded_mtx);
        Log log;
        log.identifier = log_channel_name;
        log.timestamp = timestamp;
        log.log_level = (int)log_level;
        log.message = message;
        log.name = name;
        log.file = file;
        log.line = line;
        _log_waiting_to_be_encoded.push_back(log);
    }
};
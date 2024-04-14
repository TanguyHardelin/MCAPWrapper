#include "MCAPWriter.h"
#include "internal/MCAPFileWriter.h"
#include "internal/FoxgloveSchema.hpp"
#include "internal/Base64.hpp"

#include <map>
#include <fstream>
#include <opencv2/imgcodecs.hpp>

namespace mcap_wrapper
{
    // Each file stream is stored into dictionnary for being called later.
    std::map<std::string, MCAPFileWriter> file_writers;

    bool open_file_connexion(std::string file_path, std::string open_file_connexion)
    {
        if (open_file_connexion == "")
            open_file_connexion = file_path;
        file_writers[open_file_connexion] = MCAPFileWriter();
        if (file_writers[open_file_connexion].open(file_path))
            return true;
        return false;
    }

    void close_file_connexion(std::string reference_name)
    {
        if (file_writers.count(reference_name))
            file_writers[reference_name].close();
    }

    unsigned get_number_of_connexion_presents_for_identifier(std::string identifier)
    {
        unsigned number_of_connexion_presents = 0;
        number_of_connexion_presents += file_writers.count(identifier);

        // TODO: Add network
        return number_of_connexion_presents;
    }

    bool is_file_identifier(std::string connexion_identifier)
    {
        return file_writers.count(connexion_identifier) > 0;
    }

    bool write_image_to_all(std::string identifier, cv::Mat image, uint64_t timestamp, std::string frame_id)
    {
        bool out = true;
        for (auto &kv : file_writers)
            out &= write_image_to(kv.first, identifier, image, timestamp, frame_id);
        return out;
    }

    bool write_image_to_all(std::vector<std::string> connexion_identifier, std::string identifier, cv::Mat image, uint64_t timestamp, std::string frame_id)
    {
        bool out = true;
        for (auto id : connexion_identifier)
            out &= write_image_to(id, identifier, image, timestamp, frame_id);
        return out;
    }

    bool write_image_to(std::string connexion_identifier, std::string identifier, cv::Mat image, uint64_t timestamp, std::string frame_id)
    {
        unsigned number_of_connexion_presents = get_number_of_connexion_presents_for_identifier(connexion_identifier);
        if (number_of_connexion_presents == 0)
            return false;

        ////////////////////
        //
        //  File connexion
        //
        //////////////////
        if (is_file_identifier(connexion_identifier))
        {
            // Create schema if not present:
            if (!file_writers[connexion_identifier].is_schema_present(identifier))
            {
                nlohmann::json schema_unserialized = nlohmann::json::parse(compressed_image_schema);
                file_writers[connexion_identifier].create_schema(identifier, schema_unserialized);
            }

            // Camera calibration infos
            // TODO: place it in function
            nlohmann::json camera_calibration_foxglove_schema = nlohmann::json::parse(camera_calibration_schema);
            file_writers[connexion_identifier].create_schema("CameraInfo", camera_calibration_foxglove_schema);
            nlohmann::json camera_calibration;
            camera_calibration["timestamp"] = nlohmann::json();
            camera_calibration["timestamp"]["sec"] = (uint64_t)timestamp / (uint64_t)1e9;
            camera_calibration["timestamp"]["nsec"] = (uint64_t)timestamp % (uint64_t)1e9;
            camera_calibration["frame_id"] = frame_id;
            camera_calibration["width"] = image.rows;
            camera_calibration["height"] = image.cols;
            camera_calibration["distortion_model"] = "rational_polynomial";
            camera_calibration["D"] = std::vector<float>({0, 0, 0, 0});
            camera_calibration["K"] = std::vector<float>({100, 0, 0, 0, 100, 0, 0, 0, 1});
            camera_calibration["R"] = std::vector<float>({1, 0, 0, 0, 1, 0, 0, 0, 1});
            camera_calibration["P"] = std::vector<float>({100, 0, 0, 0, 0, 100, 0, 0, 0, 0, 1, 0});
            write_JSON_to_all("CameraInfo", camera_calibration.dump(), timestamp);

            nlohmann::json image_sample;
            image_sample["timestamp"] = nlohmann::json();
            image_sample["timestamp"]["sec"] = timestamp / (uint64_t)1e9;
            image_sample["timestamp"]["nsec"] = timestamp % (uint64_t)1e9;
            image_sample["frame_id"] = frame_id;
            // Encode image in JPEG
            std::vector<int> compression_params;
            compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
            compression_params.push_back(95); // Adjust the quality (0-100), higher is better quality

            std::vector<uchar> encoding_buffer;
            cv::imencode(".jpg", image, encoding_buffer, compression_params);
            // Foxglove wait buffer data in base64 so we need to convert it
            image_sample["data"] = base64::to_base64(std::string(encoding_buffer.begin(), encoding_buffer.end()));
            image_sample["format"] = "jpeg";

            file_writers[connexion_identifier].push_sample(identifier, image_sample, timestamp);
        }
        ////////////////////
        //
        //  Network connexion
        //
        //////////////////
        // TODO: implement it !

        return true;
    }

    bool write_JSON_to_all(std::string identifier, std::string serialized_json, uint64_t timestamp)
    {
        bool out = true;
        for (auto &kv : file_writers)
            out &= write_JSON_to(kv.first, identifier, serialized_json, timestamp);
        return out;
    }

    bool write_JSON_to(std::vector<std::string> connexion_identifier, std::string identifier, std::string serialized_json, uint64_t timestamp)
    {
        bool out = true;
        for (auto id : connexion_identifier)
            out &= write_JSON_to(id, identifier, serialized_json, timestamp);
        return out;
    }

    bool write_JSON_to(std::string connexion_identifier, std::string identifier, std::string serialized_json, uint64_t timestamp)
    {
        unsigned number_of_connexion_presents = get_number_of_connexion_presents_for_identifier(connexion_identifier);
        if (number_of_connexion_presents == 0)
            return false;

        nlohmann::json unserialiazed_json;
        try
        {
            unserialiazed_json = nlohmann::json::parse(serialized_json);
        }
        catch (std::exception e)
        { // Parse error:
            std::cerr << "[MCAPWrapper] ERROR: failed to parse " << serialized_json << std::endl;
            return false;
        }

        ////////////////////
        //
        //  File connexion
        //
        //////////////////
        if (is_file_identifier(connexion_identifier))
        {
            file_writers[connexion_identifier].push_sample(identifier, unserialiazed_json, timestamp);
        }
        ////////////////////
        //
        //  Network connexion
        //
        //////////////////
        // TODO: implement it !

        return true;
    }

    bool add_frame_transform_to_all(std::string transform_name, uint64_t timestamp, std::string parent, std::string child, Eigen::Matrix4f pose)
    {
        bool out = true;
        for (auto &kv : file_writers)
            out &= add_frame_transform_to(kv.first, transform_name, timestamp, parent, child, pose);
        return out;
    }

    bool add_frame_transform_to(std::vector<std::string> connexion_identifier, std::string transform_name, uint64_t timestamp, std::string parent, std::string child, Eigen::Matrix4f pose)
    {
        bool out = true;
        for (auto id : connexion_identifier)
            out &= add_frame_transform_to(id, transform_name, timestamp, parent, child, pose);
        return out;
    }

    bool add_frame_transform_to(std::string connexion_identifier, std::string transform_name, uint64_t timestamp, std::string parent, std::string child, Eigen::Matrix4f pose)
    {
        unsigned number_of_connexion_presents = get_number_of_connexion_presents_for_identifier(connexion_identifier);
        if (number_of_connexion_presents == 0)
            return false;

        ////////////////////
        //
        //  File connexion
        //
        //////////////////
        if (is_file_identifier(connexion_identifier))
        {
            // Create schema if not present:
            if (!file_writers[connexion_identifier].is_schema_present(transform_name))
            {
                nlohmann::json schema_unserialized = nlohmann::json::parse(frame_transform_schema);
                file_writers[connexion_identifier].create_schema(transform_name, schema_unserialized);
            }

            // Create transform object
            nlohmann::json frame_tranform;
            frame_tranform["timestamp"] = nlohmann::json();
            frame_tranform["timestamp"]["sec"] = (uint64_t)timestamp / (uint64_t)1e9;
            frame_tranform["timestamp"]["nsec"] = (uint64_t)timestamp % (uint64_t)1e9;
            if (parent.size() > 1)
                frame_tranform["parent_frame_id"] = parent;
            if (child.size() > 1)
                frame_tranform["child_frame_id"] = child;
            nlohmann::json serialized_pose = Internal3DObject::pose_serializer(pose);
            frame_tranform["translation"] = serialized_pose["position"];
            frame_tranform["rotation"] = serialized_pose["orientation"];

            write_JSON_to(connexion_identifier, transform_name, frame_tranform.dump(), timestamp);
        }
        ////////////////////
        //
        //  Network connexion
        //
        //////////////////
        // TODO: implement it !

        return true;
    }

    void create_3D_object(std::string object_name, std::string frame_id, bool frame_locked)
    {
        for (auto &kv : file_writers)
        {
            kv.second.create_3D_object(object_name, frame_id, frame_locked);
        }
    }

    bool add_metadata_to_3d_object(std::string object_name, std::pair<std::string, std::string> metadata)
    {
        bool out = true;
        for (auto &kv : file_writers)
        {
            out &= kv.second.add_metadata_to_3d_object(object_name, metadata);
        }
        return out;
    }

    bool add_arrow_to_3d_object(std::string object_name,
                                Eigen::Matrix4f pose,
                                double shaft_length,
                                double shaft_diameter,
                                double head_length,
                                double head_diameter,
                                std::array<double, 4> color)
    {
        bool out = true;
        for (auto &kv : file_writers)
        {
            out &= kv.second.add_arrow_to_3d_object(object_name, pose, shaft_length, shaft_diameter, head_length, head_diameter, color);
        }
        return out;
    }

    bool add_cube_to_3d_object(std::string object_name,
                               Eigen::Matrix4f pose,
                               std::array<double, 3> size,
                               std::array<double, 4> color)
    {
        bool out = true;
        for (auto &kv : file_writers)
        {
            out &= kv.second.add_cube_to_3d_object(object_name, pose, size, color);
        }
        return out;
    }

    bool add_sphere_to_3d_object(std::string object_name,
                                 Eigen::Matrix4f pose,
                                 std::array<double, 3> size,
                                 std::array<double, 4> color)
    {
        bool out = true;
        for (auto &kv : file_writers)
        {
            out &= kv.second.add_sphere_to_3d_object(object_name, pose, size, color);
        }
        return out;
    }

    bool add_cylinder_to_3d_object(std::string object_name,
                                   Eigen::Matrix4f pose,
                                   double bottom_scale,
                                   double top_scale,
                                   std::array<double, 3> size,
                                   std::array<double, 4> color)
    {
        bool out = true;
        for (auto &kv : file_writers)
        {
            out &= kv.second.add_cylinder_to_3d_object(object_name, pose, bottom_scale, top_scale, size, color);
        }
        return out;
    }

    bool add_line_to_3d_object(std::string object_name,
                               Eigen::Matrix4f pose,
                               double thickness,
                               bool scale_invariant,
                               std::vector<Eigen::Vector3d> points,
                               std::array<double, 4> color,
                               std::vector<std::array<double, 4>> colors,
                               std::vector<uint32_t> indices)
    {
        bool out = true;
        for (auto &kv : file_writers)
        {
            out &= kv.second.add_line_to_3d_object(object_name, pose, thickness, scale_invariant, points, color, colors, indices);
        }
        return out;
    }

    bool add_triangle_to_3d_object(std::string object_name,
                                   Eigen::Matrix4f pose,
                                   std::vector<Eigen::Vector3d> points,
                                   std::array<double, 4> color,
                                   std::vector<std::array<double, 4>> colors,
                                   std::vector<uint32_t> indices)
    {
        bool out = true;
        for (auto &kv : file_writers)
        {
            out &= kv.second.add_triangle_to_3d_object(object_name, pose, points, color, colors, indices);
        }
        return out;
    }

    bool add_text_to_3d_object(std::string object_name,
                               Eigen::Matrix4f pose,
                               bool billboard,
                               double font_size,
                               bool scale_invariant,
                               std::array<double, 4> color,
                               std::string text)
    {
        bool out = true;
        for (auto &kv : file_writers)
        {
            out &= kv.second.add_text_to_3d_object(object_name, pose, billboard, font_size, scale_invariant, color, text);
        }
        return out;
    }

    bool write_3d_object_to_all(std::string object_name, uint64_t timestamp)
    {
        bool out = true;
        for (auto &kv : file_writers)
            out &= write_3d_object_to(kv.first, object_name, timestamp);
        return out;
    }

    bool write_3d_object_to_all(std::vector<std::string> connexion_identifier, std::string object_name, uint64_t timestamp)
    {
        bool out = true;
        for (auto id : connexion_identifier)
            out &= write_3d_object_to(id, object_name, timestamp);
        return out;
    }

    bool write_3d_object_to(std::string connexion_identifier, std::string object_name, uint64_t timestamp)
    {
        unsigned number_of_connexion_presents = get_number_of_connexion_presents_for_identifier(connexion_identifier);
        if (number_of_connexion_presents == 0)
            return false;

        ////////////////////
        //
        //  File connexion
        //
        //////////////////
        if (is_file_identifier(connexion_identifier))
        {
            // Create schema if not present:
            if (!file_writers[connexion_identifier].is_schema_present(object_name))
            {
                nlohmann::json schema_unserialized = nlohmann::json::parse(scene_update_schema);
                file_writers[connexion_identifier].create_schema(object_name, schema_unserialized);
            }
            file_writers[connexion_identifier].write_3d_object_to_all(object_name, timestamp);
        }
        ////////////////////
        //
        //  Network connexion
        //
        //////////////////
        // TODO: implement it !

        return true;
    }

    bool write_log_to_all(std::string log_channel_name, uint64_t timestamp, LOG_LEVEL log_level, std::string message, std::string name, std::string file, uint32_t line)
    {
        bool out = true;
        for (auto &kv : file_writers)
        {
            out &= write_log_to(kv.first, log_channel_name, timestamp, log_level, message, name, file, line);
        }
        return out;
    }

    bool write_log_to_all(std::vector<std::string> connexion_identifier, std::string log_channel_name, uint64_t timestamp, LOG_LEVEL log_level, std::string message, std::string name, std::string file, uint32_t line)
    {
        bool out = true;
        for (auto id : connexion_identifier)
        {
            out &= write_log_to(id, log_channel_name, timestamp, log_level, message, name, file, line);
        }
        return out;
    }

    bool write_log_to(std::string connexion_identifier, std::string log_channel_name, uint64_t timestamp, LOG_LEVEL log_level, std::string message, std::string name, std::string file, uint32_t line)
    {
        unsigned number_of_connexion_presents = get_number_of_connexion_presents_for_identifier(connexion_identifier);
        if (number_of_connexion_presents == 0)
            return false;

        nlohmann::json log_json;
        log_json["timestamp"] = nlohmann::json();
        log_json["timestamp"]["sec"] = (uint64_t)timestamp / (uint64_t)1e9;
        log_json["timestamp"]["nsec"] = (uint64_t)timestamp % (uint64_t)1e9;
        log_json["level"] = (int)log_level;
        log_json["message"] = message;
        log_json["name"] = name;
        log_json["file"] = file;
        log_json["line"] = line;

        ////////////////////
        //
        //  File connexion
        //
        //////////////////
        if (is_file_identifier(connexion_identifier))
        {
            for (auto &kv : file_writers)
            {
                // Create schema if not present:
                if (!file_writers[connexion_identifier].is_schema_present(log_channel_name))
                {
                    nlohmann::json schema_unserialized = nlohmann::json::parse(log_schema);
                    file_writers[connexion_identifier].create_schema(log_channel_name, schema_unserialized);
                }

                file_writers[connexion_identifier].push_sample(log_channel_name, log_json, timestamp);
            }
        }
        ////////////////////
        //
        //  Network connexion
        //
        //////////////////
        // TODO: implement it !

        return true;
    }

    bool add_position_to_all(std::string position_channel_name, uint64_t timestamp, Eigen::Matrix4f pose, std::string frame_id)
    {
        bool out = true;
        for (auto &kv : file_writers)
        {
            out &= add_position_to(kv.first, position_channel_name, timestamp, pose, frame_id);
        }
        return out;
    }

    bool add_position_to(std::vector<std::string> connexion_identifier, std::string position_channel_name, uint64_t timestamp, Eigen::Matrix4f pose, std::string frame_id)
    {
        bool out = true;
        for (auto id : connexion_identifier)
        {
            out &= add_position_to(id, position_channel_name, timestamp, pose, frame_id);
        }
        return out;
    }

    bool add_position_to(std::string connexion_identifier, std::string position_channel_name, uint64_t timestamp, Eigen::Matrix4f pose, std::string frame_id)
    {
        unsigned number_of_connexion_presents = get_number_of_connexion_presents_for_identifier(connexion_identifier);
        if (number_of_connexion_presents == 0)
            return false;

        ////////////////////
        //
        //  File connexion
        //
        //////////////////
        if (is_file_identifier(connexion_identifier))
        {
            // Create schema if not present:
            if (!file_writers[connexion_identifier].is_schema_present(position_channel_name))
            {
                nlohmann::json schema_unserialized = nlohmann::json::parse(poses_in_frame_schema);
                file_writers[connexion_identifier].create_schema(position_channel_name, schema_unserialized);
            }

            file_writers[connexion_identifier].add_position_to_all(position_channel_name, timestamp, pose, frame_id);
        }
        return true;
    }
};
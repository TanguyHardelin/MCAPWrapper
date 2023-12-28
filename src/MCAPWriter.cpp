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

    bool open_file(std::string file_path)
    {
        file_writers[file_path] = MCAPFileWriter();
        if (file_writers[file_path].open(file_path))
            return true;
        return false;
    }

    void close_file(std::string file_path)
    {
        if (file_writers.count(file_path))
            file_writers[file_path].close();
    }

    bool write_image(std::string identifier, cv::Mat image, uint64_t timestamp)
    {
        for (auto &kv : file_writers)
        {
            // Create schema if not present:
            if (!kv.second.is_schema_present(identifier))
            {
                nlohmann::json schema_unserialized = nlohmann::json::parse(compressed_image_schema);
                kv.second.create_schema(identifier, schema_unserialized);
            }

            nlohmann::json image_sample;
            image_sample["timestamp"] = nlohmann::json();
            image_sample["timestamp"]["sec"] = timestamp / (uint64_t)1e9;
            image_sample["timestamp"]["nsec"] = timestamp % (uint64_t)1e9;
            image_sample["frame_id"] = "";
            // Encode image in JPEG
            std::vector<int> compression_params;
            compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
            compression_params.push_back(95); // Adjust the quality (0-100), higher is better quality

            std::vector<uchar> encoding_buffer;
            cv::imencode(".jpg", image, encoding_buffer, compression_params);
            // Foxglove wait buffer data in base64 so we need to convert it
            image_sample["data"] = base64::to_base64(std::string(encoding_buffer.begin(), encoding_buffer.end()));
            image_sample["format"] = "jpeg";

            std::ofstream sample_file("test.txt");
            sample_file << base64::to_base64(std::string(encoding_buffer.begin(), encoding_buffer.end())) << std::endl;
            sample_file.close();

            kv.second.push_sample(identifier, image_sample, timestamp);
        }
        return false;
    }

    bool write_JSON(std::string identifier, std::string serialized_json, uint64_t timestamp)
    {
        nlohmann::json unserialiazed_json;
        try
        {
            unserialiazed_json = nlohmann::json::parse(serialized_json);
        }
        catch (std::exception e)
        { // Parse error:
            std::cerr << "ERROR: failed to parse " << serialized_json << std::endl;
            return false;
        }
        for (auto &kv : file_writers)
            kv.second.push_sample(identifier, unserialiazed_json, timestamp);
        return true;
    }

    void add_frame_transform(std::string transform_name, uint64_t timestamp, std::string parent, std::string child, Eigen::Matrix4f pose)
    {
        transform_name = "/tf/" + transform_name;
        for (auto &kv : file_writers)
        {
            // Create schema
            nlohmann::json schema_unserialized = nlohmann::json::parse(frame_transform_schema);
            kv.second.create_schema(transform_name, schema_unserialized);

            nlohmann::json frame_transforms;
            frame_transforms["transforms"] = std::vector<nlohmann::json>();

            // Create transform object
            nlohmann::json frame_tranform;
            frame_tranform["timestamp"] = nlohmann::json();
            frame_tranform["timestamp"]["sec"] = (uint64_t)timestamp / (uint64_t) 1e9;
            frame_tranform["timestamp"]["nsec"] = (uint64_t)timestamp % (uint64_t) 1e9;
            frame_tranform["parent_frame_id"] = parent;
            nlohmann::json serialized_pose = Internal3DObject::pose_serializer(pose);
            frame_tranform["translation"] = serialized_pose["translation"];
            frame_tranform["rotation"] = serialized_pose["rotation"];

            frame_transforms["transforms"].push_back(frame_tranform);
            write_JSON(transform_name, frame_transforms.dump(), timestamp);
        }
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

    bool write_3d_object(std::string object_name, uint64_t timestamp)
    {
        bool out = true;
        for (auto &kv : file_writers)
        {
            // Create schema if not present:
            if (!kv.second.is_schema_present(object_name))
            {
                nlohmann::json schema_unserialized = nlohmann::json::parse(scene_update_schema);
                kv.second.create_schema(object_name, schema_unserialized);
            }

            out &= kv.second.write_3d_object(object_name, timestamp);
        }
        return out;
    }

    void add_log(std::string log_channel_name, uint64_t timestamp, LOG_LEVEL log_level, std::string message, std::string name, std::string file, uint32_t line){
        nlohmann::json log_json;
        log_json["timestamp"] = nlohmann::json();
        log_json["timestamp"]["sec"] = (uint64_t) timestamp / (uint64_t) 1e9;
        log_json["timestamp"]["nsec"] = (uint64_t) timestamp % (uint64_t) 1e9;
        log_json["level"] = (int)log_level;
        log_json["message"] = message;
        log_json["name"] = name;
        log_json["file"] = file;
        log_json["line"] = line;

        for (auto &kv : file_writers)
        {
            // Create schema if not present:
            if (!kv.second.is_schema_present(log_channel_name))
            {
                nlohmann::json schema_unserialized = nlohmann::json::parse(log_schema);
                kv.second.create_schema(log_channel_name, schema_unserialized);
            }

            kv.second.push_sample(log_channel_name, log_json, timestamp);
        }
        
    }
};
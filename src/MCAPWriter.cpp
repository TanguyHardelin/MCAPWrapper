#include "MCAPWriter.h"
#include "internal/MCAPFileWriter.h"
#include "internal/MCAPWebSocketWriter.h"
#include "internal/FoxgloveSchema.hpp"
#include "internal/Base64.hpp"
#include "internal/IWriter.h"

#include <map>
#include <memory>
#include <fstream>
#include <opencv2/imgcodecs.hpp>

namespace mcap_wrapper
{
    // Each file stream is stored into dictionnary for being called later.
    std::map<std::string, std::shared_ptr<IWriter>> all_writers;

    bool open_file_connection(std::string const &file_path, std::string const &connection_name)
    {
        std::string real_connection_name = connection_name;
        if (real_connection_name == "")
            real_connection_name = file_path;
        auto file_writer = std::make_shared<MCAPFileWriter>();
        if (file_writer->open(file_path))
        {
            all_writers[real_connection_name] = file_writer;
            return true;
        }
        return false;
    }

    bool open_network_connection(std::string const &url, unsigned port, std::string const &reference_name, std::string const &server_name)
    {
        if (reference_name == "")
            return false;
        auto websocket_writer = std::make_shared<MCAPWebSocketWriter>();
        foxglove::ServerOptions server_options;
        websocket_writer->open(url, port, server_name, server_options);
        all_writers[reference_name] = websocket_writer;
        return true;
    }

    void close_file_connection(std::string const &reference_name)
    {
        if (all_writers.count(reference_name))
            all_writers[reference_name]->close();
    }

    void close_all_files()
    {
        for (auto &kv : all_writers)
            kv.second->close();
    }

    void close_all_network()
    {
        for (auto &kv : all_writers)
            kv.second->close();
    }

    void close_network_connection(std::string const &reference_name)
    {
        if (all_writers.count(reference_name))
            all_writers[reference_name]->close();
    }

    unsigned get_number_of_connection_presents_for_identifier(std::string const &identifier)
    {
        unsigned number_of_connection_presents = 0;
        number_of_connection_presents += all_writers.count(identifier);
        return number_of_connection_presents;
    }

    bool write_image_to_all(std::string const &identifier, cv::Mat const &image, uint64_t timestamp, std::string const &frame_id)
    {
        bool out = true;
        for (auto &kv : all_writers)
            out &= write_image_to(kv.first, identifier, image, timestamp, frame_id);
        return out;
    }

    bool write_image_to_all(std::vector<std::string> const &connection_identifier, std::string const &identifier, cv::Mat const &image, uint64_t timestamp, std::string const &frame_id)
    {
        bool out = true;
        for (auto id : connection_identifier)
            out &= write_image_to(id, identifier, image, timestamp, frame_id);
        return out;
    }

    bool write_image_to(std::string const &connection_identifier, std::string const &identifier, cv::Mat const &image, uint64_t timestamp, std::string const &frame_id)
    {
        unsigned number_of_connection_presents = get_number_of_connection_presents_for_identifier(connection_identifier);
        if (number_of_connection_presents == 0)
            return false;
        all_writers[connection_identifier]->write_image(identifier, image, timestamp, frame_id);
        return true;
    }

    bool set_connection_to_be_sync(std::string const &connection_name, bool sync)
    {
        unsigned number_of_connection_presents = get_number_of_connection_presents_for_identifier(connection_name);
        if (number_of_connection_presents == 0)
            return false;
        all_writers[connection_name]->set_sync(sync);
        return true;
    }

    bool write_camera_calibration_all(std::string const &camera_identifier,
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
        bool out = true;
        for (auto &kv : all_writers)
        {
            write_camera_calibration_to(kv.first, camera_identifier, timestamp, frame_id, image_width, image_height, distortion_model, D, K, R, P);
        }
        return out;
    }

    bool write_camera_calibration_to(std::vector<std::string> const &connection_identifier,
                                     std::string const &camera_identifier,
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
        bool out = true;
        for (auto id : connection_identifier)
        {
            write_camera_calibration_to(id, camera_identifier, timestamp, frame_id, image_width, image_height, distortion_model, D, K, R, P);
        }
        return out;
    }

    bool write_camera_calibration_to(std::string const &connection_identifier,
                                     std::string const &camera_identifier,
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
        unsigned number_of_connection_presents = get_number_of_connection_presents_for_identifier(connection_identifier);
        if (number_of_connection_presents == 0)
            return false;

        all_writers[connection_identifier]->write_camera_calibration(camera_identifier, timestamp, frame_id, image_width, image_height, distortion_model, D, K, R, P);

        return true;
    }

    bool write_JSON_to_all(std::string const &identifier, std::string const &serialized_json, uint64_t timestamp)
    {
        bool out = true;
        for (auto &kv : all_writers)
            out &= write_JSON_to(kv.first, identifier, serialized_json, timestamp);
        return out;
    }

    bool write_JSON_to(std::vector<std::string> const &connection_identifier, std::string const &identifier, std::string const &serialized_json, uint64_t timestamp)
    {
        bool out = true;
        for (auto id : connection_identifier)
            out &= write_JSON_to(id, identifier, serialized_json, timestamp);
        return out;
    }

    bool write_JSON_to(std::string const &connection_identifier, std::string const &identifier, std::string const &serialized_json, uint64_t timestamp)
    {
        unsigned number_of_connection_presents = get_number_of_connection_presents_for_identifier(connection_identifier);
        if (number_of_connection_presents == 0)
            return false;

        all_writers[connection_identifier]->write_raw_message(identifier, serialized_json, timestamp);

        return true;
    }

    bool add_frame_transform_to_all(std::string transform_name, uint64_t timestamp, std::string parent, std::string child, Eigen::Matrix4f pose)
    {
        bool out = true;
        for (auto &kv : all_writers)
            out &= add_frame_transform_to(kv.first, transform_name, timestamp, parent, child, pose);
        return out;
    }

    bool add_frame_transform_to(std::vector<std::string> connection_identifier, std::string transform_name, uint64_t timestamp, std::string parent, std::string child, Eigen::Matrix4f pose)
    {
        bool out = true;
        for (auto id : connection_identifier)
            out &= add_frame_transform_to(id, transform_name, timestamp, parent, child, pose);
        return out;
    }

    bool add_frame_transform_to(std::string connection_identifier, std::string transform_name, uint64_t timestamp, std::string parent, std::string child, Eigen::Matrix4f pose)
    {
        unsigned number_of_connection_presents = get_number_of_connection_presents_for_identifier(connection_identifier);
        if (number_of_connection_presents == 0)
            return false;

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

        ////////////////////
        //
        //  File connection
        //
        //////////////////
        // Create schema if not present:
        if (!all_writers[connection_identifier]->is_schema_present(transform_name))
        {
            nlohmann::json schema_unserialized = nlohmann::json::parse(get_frame_transform_schema());
            all_writers[connection_identifier]->create_schema(transform_name, schema_unserialized);
        }
        write_JSON_to(connection_identifier, transform_name, frame_tranform.dump(), timestamp);

        return true;
    }

    void create_3D_object(std::string object_name, std::string frame_id, bool frame_locked)
    {
        for (auto &kv : all_writers)
        {
            kv.second->create_3D_object(object_name, frame_id, frame_locked);
        }
    }

    bool add_metadata_to_3d_object(std::string object_name, std::pair<std::string, std::string> metadata)
    {
        bool out = true;
        for (auto &kv : all_writers)
        {
            out &= kv.second->add_metadata_to_3d_object(object_name, metadata);
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
        for (auto &kv : all_writers)
        {
            out &= kv.second->add_arrow_to_3d_object(object_name, pose, shaft_length, shaft_diameter, head_length, head_diameter, color);
        }
        return out;
    }

    bool add_cube_to_3d_object(std::string object_name,
                               Eigen::Matrix4f pose,
                               std::array<double, 3> size,
                               std::array<double, 4> color)
    {
        bool out = true;
        for (auto &kv : all_writers)
        {
            out &= kv.second->add_cube_to_3d_object(object_name, pose, size, color);
        }
        return out;
    }

    bool add_sphere_to_3d_object(std::string object_name,
                                 Eigen::Matrix4f pose,
                                 std::array<double, 3> size,
                                 std::array<double, 4> color)
    {
        bool out = true;
        for (auto &kv : all_writers)
        {
            out &= kv.second->add_sphere_to_3d_object(object_name, pose, size, color);
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
        for (auto &kv : all_writers)
        {
            out &= kv.second->add_cylinder_to_3d_object(object_name, pose, bottom_scale, top_scale, size, color);
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
        for (auto &kv : all_writers)
        {
            out &= kv.second->add_line_to_3d_object(object_name, pose, thickness, scale_invariant, points, color, colors, indices);
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
        for (auto &kv : all_writers)
        {
            out &= kv.second->add_triangle_to_3d_object(object_name, pose, points, color, colors, indices);
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
        for (auto &kv : all_writers)
        {
            out &= kv.second->add_text_to_3d_object(object_name, pose, billboard, font_size, scale_invariant, color, text);
        }
        return out;
    }

    bool write_3d_object_to_all(std::string object_name, uint64_t timestamp)
    {
        bool out = true;
        for (auto &kv : all_writers)
            out &= write_3d_object_to(kv.first, object_name, timestamp);
        return out;
    }

    bool write_3d_object_to_all(std::vector<std::string> connection_identifier, std::string object_name, uint64_t timestamp)
    {
        bool out = true;
        for (auto id : connection_identifier)
            out &= write_3d_object_to(id, object_name, timestamp);
        return out;
    }

    bool write_3d_object_to(std::string connection_identifier, std::string object_name, uint64_t timestamp)
    {
        unsigned number_of_connection_presents = get_number_of_connection_presents_for_identifier(connection_identifier);
        if (number_of_connection_presents == 0)
            return false;

        // Create schema if not present:
        if (!all_writers[connection_identifier]->is_schema_present(object_name))
        {
            nlohmann::json schema_unserialized = nlohmann::json::parse(get_scene_update_schema());
            all_writers[connection_identifier]->create_schema(object_name, schema_unserialized);
        }
        all_writers[connection_identifier]->write_3d_object_to_all(object_name, timestamp);

        return true;
    }

    bool write_log_to_all(std::string const& log_channel_name, uint64_t timestamp, LOG_LEVEL log_level, std::string const& message, std::string const& name, std::string const& file, uint32_t line)
    {
        bool out = true;
        for (auto &kv : all_writers)
        {
            out &= write_log_to(kv.first, log_channel_name, timestamp, log_level, message, name, file, line);
        }
        return out;
    }

    bool write_log_to_all(std::vector<std::string> const& connection_identifier, std::string const& log_channel_name, uint64_t timestamp, LOG_LEVEL log_level, std::string const& message, std::string const& name, std::string const& file, uint32_t line)
    {
        bool out = true;
        for (auto id : connection_identifier)
        {
            out &= write_log_to(id, log_channel_name, timestamp, log_level, message, name, file, line);
        }
        return out;
    }

    bool write_log_to(std::string const& connection_identifier, std::string const& log_channel_name, uint64_t timestamp, LOG_LEVEL log_level, std::string const& message, std::string const& name, std::string const& file, uint32_t line)
    {
        unsigned number_of_connection_presents = get_number_of_connection_presents_for_identifier(connection_identifier);
        if (number_of_connection_presents == 0)
            return false;
        all_writers[connection_identifier]->write_log(log_channel_name, timestamp, (int)log_level, message, name, file, line);

        return true;
    }

    bool add_position_to_all(std::string position_channel_name, uint64_t timestamp, Eigen::Matrix4f pose, std::string frame_id)
    {
        bool out = true;
        for (auto &kv : all_writers)
        {
            out &= add_position_to(kv.first, position_channel_name, timestamp, pose, frame_id);
        }
        return out;
    }

    bool add_position_to(std::vector<std::string> connection_identifier, std::string position_channel_name, uint64_t timestamp, Eigen::Matrix4f pose, std::string frame_id)
    {
        bool out = true;
        for (auto id : connection_identifier)
        {
            out &= add_position_to(id, position_channel_name, timestamp, pose, frame_id);
        }
        return out;
    }

    bool add_position_to(std::string connection_identifier, std::string position_channel_name, uint64_t timestamp, Eigen::Matrix4f pose, std::string frame_id)
    {
        unsigned number_of_connection_presents = get_number_of_connection_presents_for_identifier(connection_identifier);
        if (number_of_connection_presents == 0)
            return false;

        // Create schema if not present:
        if (!all_writers[connection_identifier]->is_schema_present(position_channel_name))
        {
            nlohmann::json schema_unserialized = nlohmann::json::parse(get_poses_in_frame_schema());
            all_writers[connection_identifier]->create_schema(position_channel_name, schema_unserialized);
        }

        all_writers[connection_identifier]->add_position_to_all(position_channel_name, timestamp, pose, frame_id);

        return true;
    }
};
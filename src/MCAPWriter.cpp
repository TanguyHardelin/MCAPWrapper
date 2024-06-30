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

    bool write_log_to_all(std::string const &log_channel_name, uint64_t timestamp, LOG_LEVEL log_level, std::string const &message, std::string const &name, std::string const &file, uint32_t line)
    {
        bool out = true;
        for (auto &kv : all_writers)
        {
            out &= write_log_to(kv.first, log_channel_name, timestamp, log_level, message, name, file, line);
        }
        return out;
    }

    bool write_log_to_all(std::vector<std::string> const &connection_identifier, std::string const &log_channel_name, uint64_t timestamp, LOG_LEVEL log_level, std::string const &message, std::string const &name, std::string const &file, uint32_t line)
    {
        bool out = true;
        for (auto id : connection_identifier)
        {
            out &= write_log_to(id, log_channel_name, timestamp, log_level, message, name, file, line);
        }
        return out;
    }

    bool write_log_to(std::string const &connection_identifier, std::string const &log_channel_name, uint64_t timestamp, LOG_LEVEL log_level, std::string const &message, std::string const &name, std::string const &file, uint32_t line)
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

    void add_image_annotation_to_all(std::vector<CircleAnnotation> const &circle_annotations, std::string const& channel_name, std::vector<PointsAnnotation> const &points_annotations, std::vector<TextAnnotation> const &text_annotations, uint64_t timestamp)
    {
        for (auto &kv : all_writers)
        {
           add_image_annotation_to(kv.first, channel_name,  circle_annotations, points_annotations, text_annotations, timestamp);
        }
    }
    void add_image_annotation_to(std::vector<std::string> const &connection_identifier, std::string const& channel_name, std::vector<CircleAnnotation> const &circle_annotations, std::vector<PointsAnnotation> const &points_annotations, std::vector<TextAnnotation> const &text_annotations, uint64_t timestamp)
    {
        for (auto id : connection_identifier)
        {
           add_image_annotation_to(id, channel_name,  circle_annotations, points_annotations, text_annotations, timestamp);
        }
    }
    void add_image_annotation_to(std::string const &connection_identifier, std::string const& channel_name, std::vector<CircleAnnotation> const &circle_annotations, std::vector<PointsAnnotation> const &points_annotations, std::vector<TextAnnotation> const &text_annotations, uint64_t timestamp)
    {
        unsigned number_of_connection_presents = get_number_of_connection_presents_for_identifier(connection_identifier);
        if (number_of_connection_presents == 0)
            return ;

        // Create schema if not present:
        if (!all_writers[connection_identifier]->is_schema_present(channel_name))
        {
            nlohmann::json schema_unserialized = nlohmann::json::parse(get_image_annotation_schema());
            all_writers[connection_identifier]->create_schema(channel_name, schema_unserialized);
        }

        nlohmann::json image_annotation;
        image_annotation["circles"] = std::vector<nlohmann::json>();
        for(auto circle : circle_annotations)
        {
            nlohmann::json circle_json;
            circle_json["timestamp"] = nlohmann::json();
            circle_json["timestamp"]["sec"] = (uint64_t)circle.timestamp / (uint64_t)1e9;
            circle_json["timestamp"]["nsec"] = (uint64_t)circle.timestamp % (uint64_t)1e9;
            circle_json["position"] = nlohmann::json();
            circle_json["position"]["x"] = circle.position[0];
            circle_json["position"]["y"] = circle.position[1];
            circle_json["diameter"] = circle.diameter;
            circle_json["thickness"] = circle.thickness;
            circle_json["fill_color"]["r"] = circle.fill_color[0];
            circle_json["fill_color"]["g"] = circle.fill_color[1];
            circle_json["fill_color"]["b"] = circle.fill_color[2];
            circle_json["fill_color"]["a"] = circle.fill_color[3];
            circle_json["outline_color"] = nlohmann::json();
            circle_json["outline_color"]["r"] = circle.outline_color[0];
            circle_json["outline_color"]["g"] = circle.outline_color[1];
            circle_json["outline_color"]["b"] = circle.outline_color[2];
            circle_json["outline_color"]["a"] = circle.outline_color[3];
            image_annotation["circles"].push_back(circle_json);
        }
        image_annotation["points"] = std::vector<nlohmann::json>();
        for(auto point: points_annotations)
        {
            nlohmann::json point_json;
            point_json["timestamp"] = nlohmann::json();
            point_json["timestamp"]["sec"] = (uint64_t)point.timestamp / (uint64_t)1e9;
            point_json["timestamp"]["nsec"] = (uint64_t)point.timestamp % (uint64_t)1e9;
            point_json["type"] = (int)point.type;
            for(auto point_position: point.points)
            {
                nlohmann::json position_json;
                position_json["x"] = point_position[0];
                position_json["y"] = point_position[1];
                point_json["positions"].push_back(position_json);
            }
            point_json["outline_color"] = nlohmann::json();
            point_json["outline_color"]["r"] = point.outline_color[0];
            point_json["outline_color"]["g"] = point.outline_color[1];
            point_json["outline_color"]["b"] = point.outline_color[2];
            point_json["outline_color"]["a"] = point.outline_color[3];
            point_json["outline_colors"] = std::vector<nlohmann::json>();
            for(auto outline_color: point.outline_colors)
            {
                nlohmann::json outline_color_json;
                outline_color_json["r"] = outline_color[0];
                outline_color_json["g"] = outline_color[1];
                outline_color_json["b"] = outline_color[2];
                outline_color_json["a"] = outline_color[3];
                point_json["outline_colors"].push_back(outline_color_json);
            }
            point_json["fill_color"]["r"] = point.fill_color[0];
            point_json["fill_color"]["g"] = point.fill_color[1];
            point_json["fill_color"]["b"] = point.fill_color[2];
            point_json["fill_color"]["a"] = point.fill_color[3];
            point_json["thickness"] = point.thickness;
            image_annotation["points"].push_back(point_json);
        }
        image_annotation["texts"] = std::vector<nlohmann::json>();
        for(auto text: text_annotations)
        {
            nlohmann::json text_json;
            text_json["timestamp"] = nlohmann::json();
            text_json["timestamp"]["sec"] = (uint64_t)text.timestamp / (uint64_t)1e9;
            text_json["timestamp"]["nsec"] = (uint64_t)text.timestamp % (uint64_t)1e9;
            text_json["position"] = nlohmann::json();
            text_json["position"]["x"] = text.position[0];
            text_json["position"]["y"] = text.position[1];
            text_json["text"] = text.text;
            text_json["font_size"] = text.font_size;
            text_json["text_color"] = nlohmann::json();
            text_json["text_color"]["r"] = text.text_color[0];
            text_json["text_color"]["g"] = text.text_color[1];
            text_json["text_color"]["b"] = text.text_color[2];
            text_json["text_color"]["a"] = text.text_color[3];
            text_json["background_color"] = nlohmann::json();
            text_json["background_color"]["r"] = text.background_color[0];
            text_json["background_color"]["g"] = text.background_color[1];
            text_json["background_color"]["b"] = text.background_color[2];
            text_json["background_color"]["a"] = text.background_color[3];
            image_annotation["texts"].push_back(text_json);
        }
        write_JSON_to(connection_identifier, channel_name, image_annotation.dump(), timestamp);
    }
};
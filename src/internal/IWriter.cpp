#include "internal/IWriter.h"

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

    void IWriter::set_sync(bool sync){
        is_write_sync = sync;
    }

    void IWriter::create_3D_object(std::string object_name, std::string frame_id, bool frame_locked)
    {
        _all_3d_object[object_name] = Internal3DObject (object_name, frame_id, frame_locked);
    }

    bool IWriter::add_metadata_to_3d_object(std::string object_name, std::pair<std::string, std::string> metadata)
    {
        if(_all_3d_object.count(object_name) == 0)
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
        if(_all_3d_object.count(object_name) == 0)
            return false;
        return _all_3d_object[object_name].add_arrow(pose, shaft_length, shaft_diameter, head_length, head_diameter, color);
    }

    bool IWriter::add_cube_to_3d_object(std::string object_name,
                                               Eigen::Matrix4f pose,
                                               std::array<double, 3> size,
                                               std::array<double, 4> color)
    {
        if(_all_3d_object.count(object_name) == 0)
            return false;
        return _all_3d_object[object_name].add_cube(pose, size, color);
    }

    bool IWriter::add_sphere_to_3d_object(std::string object_name,
                                                 Eigen::Matrix4f pose,
                                                 std::array<double, 3> size,
                                                 std::array<double, 4> color)
    {
        if(_all_3d_object.count(object_name) == 0)
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
        if(_all_3d_object.count(object_name) == 0)
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
        if(_all_3d_object.count(object_name) == 0)
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
        if(_all_3d_object.count(object_name) == 0)
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
        if(_all_3d_object.count(object_name) == 0)
            return false;
        return _all_3d_object[object_name].add_text(pose, billboard, font_size, scale_invariant, color, text);
    }
    
    bool IWriter::write_3d_object_to_all(std::string object_name, uint64_t timestamp)
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

    bool IWriter::add_position_to_all(std::string position_channel_name, uint64_t timestamp, Eigen::Matrix4f pose, std::string frame_id){
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



};
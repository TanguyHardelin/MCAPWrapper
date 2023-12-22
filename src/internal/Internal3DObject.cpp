#include "Internal3DObject.h"

unsigned long long object_id = 0;

Internal3DObject::Internal3DObject(std::string frame_id, bool frame_locked)
{
    _object_definition = nlohmann::json();
    _object_definition["frame_id"] = frame_id;
    _object_definition["id"] = std::to_string(object_id++);
    _object_definition["lifetime"] = 0;
    _object_definition["frame_locked"] = frame_locked;
    _object_definition["metadata"] = std::vector<nlohmann::json>();
    _object_definition["arrows"] = std::vector<nlohmann::json>();
    _object_definition["cubes"] = std::vector<nlohmann::json>();
    _object_definition["spheres"] = std::vector<nlohmann::json>();
    _object_definition["cylinders"] = std::vector<nlohmann::json>();
    _object_definition["lines"] = std::vector<nlohmann::json>();
    _object_definition["triangles"] = std::vector<nlohmann::json>();
    _object_definition["texts"] = std::vector<nlohmann::json>();
    _object_definition["models"] = std::vector<nlohmann::json>();
}

void Internal3DObject::set_timestamp(uint64_t timestamp)
{
    _object_definition["timestamp"] = nlohmann::json();
    _object_definition["timestamp"]["sec"] = (uint64_t)timestamp / (uint64_t)1e9;
    _object_definition["timestamp"]["nsec"] = (uint64_t)timestamp % (uint64_t)1e9;
}

bool Internal3DObject::add_metadata(std::pair<std::string, std::string> metadata)
{
    nlohmann::json metadata;
    metadata["key"] = metadata.first;
    metadata["value"] = metadata.second;
    _object_definition["metadata"].push_back(metadata);
    return true;
}

bool Internal3DObject::add_arrow(Eigen::Matrix4f pose, double shaft_length, double shaft_diameter, double head_length, double head_diameter, std::array<double, 4> color)
{
    nlohmann::json arrow;
    arrow["pose"] = pose_serializer(pose);
    arrow["shaft_length"] = shaft_length;
    arrow["shaft_diameter"] = shaft_diameter;
    arrow["head_length"] = head_length;
    arrow["head_diameter"] = head_diameter;
    arrow["color"] = color;
    _object_definition["arrows"].push_back(arrow);
    return true;
}

bool Internal3DObject::add_cube(Eigen::Matrix4f pose, std::array<double, 3> size, std::array<double, 4> color)
{
    nlohmann::json cube;
    cube["pose"] = pose_serializer(pose);
    cube["size"] = vector3_serializer(size);
    cube["color"] = color;
    _object_definition["cubes"].push_back(cube);
    return true;
}

bool Internal3DObject::add_sphere(Eigen::Matrix4f pose, std::array<double, 3> size, std::array<double, 4> color)
{
    nlohmann::json sphere;
    sphere["pose"] = pose_serializer(pose);
    sphere["size"] = vector3_serializer(size);
    sphere["color"] = color_serializer(color);
    _object_definition["spheres"].push_back(sphere);
    return true;
}

bool Internal3DObject::add_cylinder(Eigen::Matrix4f pose, double bottom_scale, double top_scale, std::array<double, 3> size, std::array<double, 4> color)
{
    nlohmann::json cylinder;
    cylinder["pose"] = pose_serializer(pose);
    cylinder["bottom_scale"] = bottom_scale;
    cylinder["top_scale"] = top_scale;
    cylinder["size"] = vector3_serializer(size);
    cylinder["color"] = color_serializer(color);
    _object_definition["cylinders"].push_back(cylinder);
    return true;
}

bool Internal3DObject::add_line(Eigen::Matrix4f pose, double thickness, bool scale_invariant, std::vector<Eigen::Vector3d> points, std::array<double, 4> color, std::vector<std::array<double, 4>> colors, std::vector<uint32_t> indices)
{
    nlohmann::json line;
    line["pose"] = pose_serializer(pose);
    line["thickness"] = thickness;
    line["scale_invariant"] = scale_invariant;
    line["points"] = std::vector<nlohmann::json>();
    for(unsigned i=0; i<points.size(); i++){
        line["points"].push_back(vector3_serializer(points[i]));
    }
    line["color"] = color_serializer(color);
    line["colors"] = std::vector<nlohmann::json>();
    for(unsigned i=0; i<colors.size(); i++){
        line["colors"].push_back(color_serializer(colors[i]));
    }
    line["indices"] = indices;
    _object_definition["lines"].push_back(line);
    return true;
}

bool Internal3DObject::add_triangle(Eigen::Matrix4f pose, std::vector<Eigen::Vector3d> points, std::array<double, 4> color, std::vector<std::array<double, 4>> colors, std::vector<uint32_t> indices)
{
    nlohmann::json triangle;
    triangle["pose"] = pose_serializer(pose);
    triangle["points"] = std::vector<nlohmann::json>();
    for(unsigned i=0; i<points.size(); i++){
        triangle["points"].push_back(vector3_serializer(points[i]));
    }
    triangle["color"] = color_serializer(color);
    triangle["colors"] = std::vector<nlohmann::json>();
    for(unsigned i=0; i<colors.size(); i++){
        triangle["colors"].push_back(color_serializer(colors[i]));
    }
    triangle["indices"] = indices;
    _object_definition["triangles"].push_back(triangle);
    return true;
}

bool Internal3DObject::add_text(Eigen::Matrix4f pose, bool billboard, double font_size, bool scale_invariant, std::array<double, 4> color, std::string text_value)
{
    nlohmann::json text;
    text["pose"] = pose_serializer(pose);
    text["billboard"] = billboard;
    text["font_size"] = font_size;
    text["scale_invariant"] = scale_invariant;
    text["color"] = color_serializer(color);
    text["colors"] = std::vector<nlohmann::json>();
    text["text"] = text_value;
    _object_definition["texts"].push_back(text);
    return true;
}

std::string Internal3DObject::serialize(){
    return _object_definition.dump();
}

nlohmann::json Internal3DObject::pose_serializer(Eigen::Matrix4f pose)
{
    nlohmann::json out;
    out["position"] = nlohmann::json();
    out["position"]["x"] = pose(0, 3);
    out["position"]["y"] = pose(1, 3);
    out["position"]["z"] = pose(2, 3);
    Eigen::Quaternionf orientation(pose.block<3, 3>(0, 0));
    out["orientation"] = nlohmann::json();
    out["orientation"]["x"] = orientation[0];
    out["orientation"]["y"] = orientation[1];
    out["orientation"]["z"] = orientation[2];
    out["orientation"]["w"] = orientation[3];
    return out;
}

nlohmann::json Internal3DObject::vector3_serializer(std::array<double, 3> vector3)
{
    nlohmann::json out;
    out["x"] = vector3[0];
    out["y"] = vector3[1];
    out["z"] = vector3[2];
    return out;
}

nlohmann::json Internal3DObject::vector3_serializer(Eigen::Vector3d vector3)
{
    nlohmann::json out;
    out["x"] = vector3[0];
    out["y"] = vector3[1];
    out["z"] = vector3[2];
    return out;
}

nlohmann::json Internal3DObject::color_serializer(std::array<double, 4> color)
{
    nlohmann::json out;
    out["r"] = color[0];
    out["g"] = color[1];
    out["b"] = color[2];
    out["a"] = color[3];
    return out;
}
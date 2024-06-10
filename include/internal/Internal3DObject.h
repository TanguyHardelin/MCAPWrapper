#ifndef __INTERNAL_3D_OBJECT_H__
#define __INTERNAL_3D_OBJECT_H__

#include <string>
#include <iostream>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include "json.hpp"

class Internal3DObject
{
public:
    Internal3DObject();
    Internal3DObject(std::string object_name, std::string frame_id, bool frame_locked);
    void set_timestamp(uint64_t timestamp);
    bool add_metadata(std::pair<std::string, std::string> metadata);
    bool add_arrow(Eigen::Matrix4f pose,
                   double shaft_length,
                   double shaft_diameter,
                   double head_length,
                   double head_diameter,
                   std::array<double, 4> color = {0, 0, 0, 1});
    bool add_cube(Eigen::Matrix4f pose,
                  std::array<double, 3> size,
                  std::array<double, 4> color = {0, 0, 0, 1});
    bool add_sphere(Eigen::Matrix4f pose,
                    std::array<double, 3> size,
                    std::array<double, 4> color = {0, 0, 0, 1});
    bool add_cylinder(Eigen::Matrix4f pose,
                      double bottom_scale,
                      double top_scale,
                      std::array<double, 3> size,
                      std::array<double, 4> color = {0, 0, 0, 1});
    bool add_line(Eigen::Matrix4f pose,
                  double thickness,
                  bool scale_invariant,
                  std::vector<Eigen::Vector3d> points,
                  std::array<double, 4> color,
                  std::vector<std::array<double, 4>> colors,
                  std::vector<uint32_t> indices);
    bool add_triangle(Eigen::Matrix4f pose,
                      std::vector<Eigen::Vector3d> points,
                      std::array<double, 4> color,
                      std::vector<std::array<double, 4>> colors,
                      std::vector<uint32_t> indices);
    bool add_text(Eigen::Matrix4f pose,
                  bool billboard,
                  double font_size,
                  bool scale_invariant,
                  std::array<double, 4> color,
                  std::string text);
    nlohmann::json get_description();
    std::string get_id();

    static nlohmann::json pose_serializer(Eigen::Matrix4f pose);
    static nlohmann::json vector3_serializer(std::array<double, 3> vector3);
    static nlohmann::json vector3_serializer(Eigen::Vector3d vector3);
    static nlohmann::json color_serializer(std::array<double, 4> color);

protected:
    

    nlohmann::json _object_definition;
    std::string _object_name;
};

#endif
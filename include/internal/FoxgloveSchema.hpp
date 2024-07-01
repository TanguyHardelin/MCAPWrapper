#ifndef FOXGLOVE_SCHEMA_HPP
#define FOXGLOVE_SCHEMA_HPP

#include <string>
#include "internal/json.hpp"

std::string get_poses_in_frame_schema();
std::string get_camera_calibration_schema();
std::string get_frame_transform_schema();
std::string get_scene_update_schema();
std::string get_log_schema();
std::string get_compressed_image_schema();
std::string get_image_annotation_schema();


#endif
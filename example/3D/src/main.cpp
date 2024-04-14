#include <iostream>
#include <chrono>
#include <thread>
#include "MCAPWriter.h"

int main(int argc, char **argv){
    // Open MCAP writer:
    mcap_wrapper::open_file_connexion("simple.mcap");

    // Add transform:
    mcap_wrapper::add_frame_transform_to_all("root", std::chrono::system_clock::now().time_since_epoch().count(), "root", "", Eigen::Matrix4f::Identity());

    // Write data into file:
    for(unsigned i=0; i<20; i++){
        // Create cube:
        Eigen::Matrix4f cube_position = Eigen::Matrix4f::Identity();
        cube_position(0,3) = i;
        mcap_wrapper::create_3D_object("cube");
        mcap_wrapper::add_cube_to_3d_object("cube", cube_position, {1, 1, 1}, {255.0,0,0,255.0});
        mcap_wrapper::write_3d_object_to_all("cube", std::chrono::system_clock::now().time_since_epoch().count());
        // Create sphere:
        Eigen::Matrix4f sphere_position = Eigen::Matrix4f::Identity();
        sphere_position(1,3) = i;
        mcap_wrapper::create_3D_object("sphere");
        mcap_wrapper::add_sphere_to_3d_object("sphere", sphere_position, {1, 1, 1}, {255.0,255.0,0,255.0});
        mcap_wrapper::add_metadata_to_3d_object("sphere", {"metadata_key", "sample metada"});
        mcap_wrapper::write_3d_object_to_all("sphere", std::chrono::system_clock::now().time_since_epoch().count());
        // Create arrow:
        Eigen::Matrix4f arrow_position = Eigen::Matrix4f::Identity();
        arrow_position(1,3) = i * 2;
        mcap_wrapper::create_3D_object("arrow");
        mcap_wrapper::add_arrow_to_3d_object("arrow", arrow_position, 1, 0.1, 0.2,0.15, {0,0,0,1});
        mcap_wrapper::add_metadata_to_3d_object("arrow", {"metadata_key", "sample metada"});
        mcap_wrapper::write_3d_object_to_all("arrow", std::chrono::system_clock::now().time_since_epoch().count());
        // Create cylinder:
        Eigen::Matrix4f cylinder_position = Eigen::Matrix4f::Identity();
        cylinder_position(0,3) = i * 2;
        mcap_wrapper::create_3D_object("cylinder");
        mcap_wrapper::add_cylinder_to_3d_object("cylinder", cylinder_position, 1, 0.5, {1,1,1}, {0,0,0,1});
        mcap_wrapper::add_metadata_to_3d_object("cylinder", {"metadata_key", "sample metada"});
        mcap_wrapper::write_3d_object_to_all("cylinder", std::chrono::system_clock::now().time_since_epoch().count());
        // Create triangle:
        Eigen::Matrix4f triangle_position = Eigen::Matrix4f::Identity();
        triangle_position(2,3) = i ;
        mcap_wrapper::create_3D_object("triangle");
        std::vector<Eigen::Vector3d> triangle_points;
        triangle_points.push_back(Eigen::Vector3d(0,0,0));
        triangle_points.push_back(Eigen::Vector3d(1,0,0));
        triangle_points.push_back(Eigen::Vector3d(0,1,0));
        triangle_points.push_back(Eigen::Vector3d(1,1,0));
        triangle_points.push_back(Eigen::Vector3d(0.5,0.5,1));
        mcap_wrapper::add_triangle_to_3d_object("triangle", triangle_position,triangle_points, {0,0,0,1}, {}, {});
        mcap_wrapper::add_metadata_to_3d_object("triangle", {"metadata_key", "sample metada"});
        mcap_wrapper::write_3d_object_to_all("triangle", std::chrono::system_clock::now().time_since_epoch().count());

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    // Close file
    mcap_wrapper::close_file_connexion("simple.mcap");

    return 0;
}

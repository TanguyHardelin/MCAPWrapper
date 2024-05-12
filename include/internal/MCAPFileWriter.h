#ifndef MCAP_FILE_WRITER_HPP
#define MCAP_FILE_WRITER_HPP

#include <string>
#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <map>
#include <Eigen/Core>
#include "mcap/writer.hpp"
#include "json.hpp"
#include "Internal3DObject.h"
#include "utils.hpp"

namespace mcap_wrapper
{
    class MCAPFileWriter
    {
    public:
        // Constructor / desctructor
        MCAPFileWriter();
        ~MCAPFileWriter();
        /**
         * @brief Open MCAP file in write mode. Will create a dedicated write thread
         *
         * @param file_name path to file to write
         * @return true Open succeed
         * @return false Open failed
         */
        bool open(std::string file_name);
        /**
         * @brief Close file and stop writing thread
         *
         * @return true Close suceed
         * @return false Close failed
         */
        bool close();
        /**
         * @brief Return true if file is openned
         *
         * @return true File is already open
         * @return false File is closed
         */
        bool is_open();
        /**
         * @brief Return true if one schema is already present in writer for dedicated channel. A schema is what will describe the data for foxglove studio.
         *
         * @param channel_name Name of the channel to check
         * @return true One schema is already present
         * @return false No schema present
         */
        bool is_schema_present(std::string channel_name);
        /**
         * @brief Create a schema by infering data type contained into data sample
         *
         * @param channel_name Channel name of data
         * @param sample Sample of data
         */
        void infer_schema(std::string channel_name, nlohmann::json sample);
        /**
         * @brief Create a schema for data corresponding to `channel_name`
         *
         * @param channel_name Channel name of data
         * @param schema Schema of data
         */
        void create_schema(std::string channel_name, nlohmann::json schema);
        /**
         * @brief Push sample into file.
         *
         * @param channel_name Channel to which data will be pushed.
         * @param sample Sample of data
         * @param timestamp Timestamp of data
         */
        void push_sample(std::string channel_name, nlohmann::json sample, uint64_t timestamp);
        /**
         * @brief Create a 3D object that can own some primitives like triangle, rectangle, etc...
         *
         * @param object_name name of the 3D object
         * @param frame_id [optionnal] corresponding frame ID
         * @param frame_locked [optionnal] Is object should keep it position or move with the specified frame
         */
        void create_3D_object(std::string object_name, std::string frame_id = "", bool frame_locked = false);
        /**
         * @brief Add metadata to 3D object
         *
         * @param object_name Name of associated 3D object
         * @param metadata metadata that will be added to the object
         * @return true The primitive was added
         * @return false The primitive was not added (error occur)
         */
        bool add_metadata_to_3d_object(std::string object_name, std::pair<std::string, std::string> metadata);
        /**
         * @brief Add arrow to 3D object
         *
         * @param object_name Name of associated 3D object
         * @param pose Position of the arrow in the space
         * @param shaft_length Length of the arrow's shaft
         * @param shaft_diameter Diameter of the arrow's shaft
         * @param head_length Length of the arrow's head
         * @param head_diameter Diameter of the arrow's head
         * @param color Color of the object
         * @return true The primitive was added
         * @return false The primitive was not added (error occur)
         */
        bool add_arrow_to_3d_object(std::string object_name,
                                    Eigen::Matrix4f pose,
                                    double shaft_length,
                                    double shaft_diameter,
                                    double head_length,
                                    double head_diameter,
                                    std::array<double, 4> color = {0, 0, 0, 1});
        /**
         * @brief Add cube to 3D object
         *
         * @param object_name Name of associated 3D object
         * @param pose Position of the cube in the space
         * @param size Size of the cube in the space
         * @param color Color of the cube in the space
         * @return true The primitive was added
         * @return false The primitive was not added (error occur)
         */
        bool add_cube_to_3d_object(std::string object_name,
                                   Eigen::Matrix4f pose,
                                   std::array<double, 3> size,
                                   std::array<double, 4> color = {0, 0, 0, 1});
        /**
         * @brief Add sphere to 3D object
         *
         * @param object_name Name of associated 3D object
         * @param pose Position of the sphere in the space
         * @param size Size of the sphere in the space
         * @param color Color of the sphere in the space
         * @return true The primitive was added
         * @return false The primitive was not added (error occur)
         */
        bool add_sphere_to_3d_object(std::string object_name,
                                     Eigen::Matrix4f pose,
                                     std::array<double, 3> size,
                                     std::array<double, 4> color = {0, 0, 0, 1});
        /**
         * @brief Add cylinder to the 3D object
         *
         * @param object_name Name of associated 3D object
         * @param pose Position of the cylinder in the space
         * @param bottom_scale Scale of the bottom of cylinder
         * @param top_scale Scale of the top of cylinder
         * @param size Size of cylinder in the space
         * @param color Color of the cylinder in the space
         * @return true The primitive was added
         * @return false The primitive was not added (error occur)
         */
        bool add_cylinder_to_3d_object(std::string object_name,
                                       Eigen::Matrix4f pose,
                                       double bottom_scale,
                                       double top_scale,
                                       std::array<double, 3> size,
                                       std::array<double, 4> color = {0, 0, 0, 1});
        /**
         * @brief Add line to 3D object
         *
         * @param object_name Name of associated 3D object
         * @param pose Position of the cylinder in the space
         * @param thickness Thickness of the line
         * @param scale_invariant Is the thickness must be specified in pixel and be fixed (true) or being scale relative to distance to the camera (false)
         * @param points Points constituting the line
         * @param color Color of the whole line
         * @param colors Per point color. Erase the `color` definition. The size of `colors` must correspond to the size of `points`
         * @param indices Indices into the points and colors attribute arrays, which can be used to avoid duplicating attribute data.
         * @return true The primitive was added
         * @return false The primitive was not added (error occur)
         */
        bool add_line_to_3d_object(std::string object_name,
                                   Eigen::Matrix4f pose,
                                   double thickness,
                                   bool scale_invariant,
                                   std::vector<Eigen::Vector3d> points,
                                   std::array<double, 4> color,
                                   std::vector<std::array<double, 4>> colors,
                                   std::vector<uint32_t> indices);
        /**
         * @brief Add triangle to 3D object
         *
         * @param object_name Name of associated 3D object
         * @param pose Position of the triangle in the space
         * @param points Vertices to use for triangles, interpreted as a list of triples (0-1-2, 3-4-5, ...)
         * @param color Color of the whole triangle
         * @param colors Per vertex color. Erase the `color` definition. The size of `colors` must correspond to the size of `points`
         * @param indices Indices into the points and colors attribute arrays, which can be used to avoid duplicating attribute data.
         * @return true The primitive was added
         * @return false The primitive was not added (error occur)
         */
        bool add_triangle_to_3d_object(std::string object_name,
                                       Eigen::Matrix4f pose,
                                       std::vector<Eigen::Vector3d> points,
                                       std::array<double, 4> color,
                                       std::vector<std::array<double, 4>> colors,
                                       std::vector<uint32_t> indices);
        /**
         * @brief Add text to 3D object
         *
         * @param object_name Name of associated 3D object
         * @param pose Position of the text in the space
         * @param billboard Whether the text should respect pose.orientation (false) or always face the camera (true)
         * @param font_size Font size (height of one line of text)
         * @param scale_invariant Indicates whether font_size is a fixed size in screen pixels (true), or specified in world coordinates and scales with distance from the camera (false)
         * @param color Color of the text
         * @param text Text
         * @return true The primitive was added
         * @return false The primitive was not added (error occur)
         */
        bool add_text_to_3d_object(std::string object_name,
                                   Eigen::Matrix4f pose,
                                   bool billboard,
                                   double font_size,
                                   bool scale_invariant,
                                   std::array<double, 4> color,
                                   std::string text);
        /**
         * @brief Write 3D object into the file
         *
         * @param object_name Name of the object to push
         * @param timestamp Timestamp in which the object will be pushed
         * @return true Everything does fines.
         * @return false Everything does wrong.
         */
        bool write_3d_object_to_all(std::string object_name, uint64_t timestamp);
        /**
         * @brief Add position that could be vizualized into 3D. Position could be linked to frame thanks to the `frame_id` parameter.
         *
         * @param position_channel_name Name of position
         * @param timestamp Timestamp of pose
         * @param pose Pose in 3D space
         * @param frame_id Frame of reference for pose position and orientation
         * @return true  Everything does fines.
         * @return false Everything does wrong.
         */
        bool add_position_to_all(std::string position_channel_name, uint64_t timestamp, Eigen::Matrix4f pose, std::string frame_id);

        // Deffine operator= for std::mutex and std::conditionnal variable
        MCAPFileWriter &operator=(const MCAPFileWriter &object);

        /**
         * @brief Set the sync mode. In sync mode every write must wait it end to return
         * 
         * @param sync Is suposed to be sync
         */
        void set_sync(bool sync);

    protected:
        void run(); // Function used for storing data into file

        // Attributes:
        mcap::McapWriter _file_writer;                                      // File writer object
        std::mutex _file_writer_mtx;                                        // Mutex of `_file_writer`
        std::map<std::string, mcap::Channel> _all_channels;                 // All channels schema
        std::queue<mcap::Message> _data_queue;                              // Data FIFO (used by write thread to get data)
        std::mutex _data_queue_mtx;                                         // Mutex of `_data_queue`
        std::thread *_writing_thread;                                       // Writing thread
        bool _continue_writing;                                             // Variable used for indicating to the writing thread if write must continue;
        std::condition_variable _write_notifier;                            // Used for signaling new data to write for writing thread
        std::map<std::string, std::string> _defined_schema;                 // Is usefull for keeping trace of defined schema
        std::map<std::string, Internal3DObject> _all_3d_object;             // Definition of all 3D objects.
        std::map<std::string, std::vector<Eigen::Matrix4f>> _all_positions; // Keep track of all positions for a dedicated channel
        bool is_write_sync = false;                                         // Attribute that is used for knowing if write should be sync
        std::mutex write_is_being_process;                                  // Mutex that is grab during the whole write process
        std::condition_variable write_finished_adviser;                     // Conditional variable that is notified when whole data were wrote
    };

};

#endif
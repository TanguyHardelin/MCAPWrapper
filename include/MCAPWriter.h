#ifndef MCAP_WRITER_HPP
#define MCAP_WRITER_HPP

#include <string>
#include <Eigen/Core>
#include <opencv2/core.hpp>

namespace mcap_wrapper
{
    /**
     * @brief Create file for writting MCAP in it. The path must be valid.
     *
     * @param file_path path to the file to write
     * @return true could create file
     * @return false could not create file
     */
    bool open_file(std::string file_path);
    /**
     * @brief Close file corresponding to the `file_path`
     *
     * @param file_path path to the file to close
     */
    void close_file(std::string file_path);
    /**
     * @brief Close all files opened
     *
     */
    void close_all_files();
    /**
     * @brief Write image into MCAP. This function is thread safe.
     *
     * @param identifier identifier to where write the input image
     * @param image cv::Mat representing the image
     * @param timestamp image's timestamp
     * @return true could write image
     * @return false could not write image
     */
    bool write_image(std::string identifier, cv::Mat image, uint64_t timestamp);
    /**
     * @brief Write JSON into MCAP. Each JSON data "type" must have a property called "__foxglove_name__" in order to be interpretable by
     * Foxglove studio as valid data. This function is thread safe.
     *
     * @param identifier identifier to where write the JSON
     * @param serialized_json JSON serialized into string
     * @param timestamp timestamp of JSON
     * @return true could write JSON
     * @return false could not write JSON
     */
    bool write_JSON(std::string identifier, std::string serialized_json, uint64_t timestamp);
    /**
     * @brief Add frame transform that could be used for 3D and image
     * 
     * @param transform_name Name of the transform
     * @param timestamp Timestamp of transform
     * @param parent Name of the parent frame
     * @param child Name of the child frame
     * @param pose Transform position
     */
    void add_frame_transform(std::string transform_name, uint64_t timestamp, std::string parent, std::string child, Eigen::Matrix4f pose);
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
    bool write_3d_object(std::string object_name, uint64_t timestamp);

    /**
     * @brief Used by add_log. Specify the level of log
     * 
     */
    enum class LOG_LEVEL{
        UNKNOWN = 0, 
        DEBUG = 1, 
        INFO = 2,
        WARNING = 3,
        ERROR = 4,
        FATAL = 5
    };
    /**
     * @brief Add log 
     * 
     * @param log_channel_name Name of channel that must be used for pushing logs
     * @param timestamp Timestamp of log message
     * @param log_level Log level
     * @param message Log message
     * @param name Process or node name
     * @param file Filename
     * @param line Line number in the file
     */
    void add_log(std::string log_channel_name, uint64_t timestamp, LOG_LEVEL log_level, std::string message, std::string name, std::string file, uint32_t line);

};

#endif
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
     * @param reference_name name refered to connection for future usage. This will be equal to `file_path` if empty
     * @return true could create file
     * @return false could not create file
     */
    bool open_file_connection(std::string const &file_path, std::string const &reference_name = "");
    /**
     * @brief Create a network connection that could be used with foxglove studio.
     *
     * @param url url where server should be
     * @param port port where server should start
     * @param reference_name name refered to connection for future usage.
     * @param server_name name of server. Will be displayed into foxglove-studio
     * @return true could create server
     * @return false could not create server
     */
    bool open_network_connection(std::string const &url, unsigned port, std::string const &reference_name, std::string const &server_name = "MCAP wrapper");
    /**
     * @brief Close file corresponding to the `file_path`
     *
     * @param reference_name path or reference to the file to close
     */
    void close_file_connection(std::string const &reference_name);
    /**
     * @brief Close network corresponding to `reference_name`
     *
     * @param reference_name server reference name
     */
    void close_network_connection(std::string const &reference_name);
    /**
     * @brief Close all files opened
     *
     */
    void close_all_files();
    /**
     * @brief Close all network connection opened
     *
     */
    void close_all_network();
    /**
     * @brief Set connection referenced with `connection_name` to be sync. Sync connection wait the end of it write for return
     *
     * @param connection_name connection to be sync
     * @param sync should the connection must be sync
     * @return true connection found
     * @return false connection not found
     */
    bool set_connection_to_be_sync(std::string const &connection_name, bool sync);
    /**
     * @brief Write image into all MCAP connection. connection can be files or live foxglove-studio.
     *
     * @param identifier identifier to where write the input image
     * @param image cv::Mat representing the image
     * @param timestamp image's timestamp
     * @return true could write image
     * @return false could not write image
     */
    bool write_image_to_all(std::string const &identifier, cv::Mat const &image, uint64_t timestamp, std::string const &frame_id = "");
    /**
     * @brief Write image into to specifics connections. connection muste be refered by it names.
     *
     * @param connection_identifier list of connection names where data must be write.
     * @param identifier identifier to where write the input image
     * @param image cv::Mat representing the image
     * @param timestamp image's timestamp
     * @return true could write image
     * @return false could not write image
     */
    bool write_image_to(std::vector<std::string> const &connection_identifier, std::string const &identifier, cv::Mat const &image, uint64_t timestamp, std::string const &frame_id = "");
    /**
     * @brief Write image into to specifics connections. connection muste be refered by it names.
     *
     * @param connection_identifier connection name where data must be write.
     * @param identifier identifier to where write the input image
     * @param image cv::Mat representing the image
     * @param timestamp image's timestamp
     * @return true could write image
     * @return false could not write image
     */
    bool write_image_to(std::string const &connection_identifier, std::string const &identifier, cv::Mat const &image, uint64_t timestamp, std::string const &frame_id = "");

    /**
     * @brief Write camera calibration. It is usefull for plotting image in 3D view and debug stereo programs.
     *
     * @param camera_identifier channel name used in MCAP for this data
     * @param timestamp Timestamp of calibration data
     * @param frame_id Frame of reference for the camera. The origin of the frame is the optical center of the camera. +x points to the right in the image, +y points down, and +z points into the plane of the image.
     * @param image_width Image width
     * @param image_height Image height
     * @param distortion_model Name of distortion model. Supported values: `plumb_bob` and `rational_polynomial`
     * @param D Distortion parameters
     * @param K Intrinsic camera matrix (3x3 row-major matrix)
     * @param R Rectification matrix (stereo cameras only, 3x3 row-major matrix)
     * @param P Projection/camera matrix (3x4 row-major matrix)
     * @return true could write calibration
     * @return false could not write calibration
     */
    bool write_camera_calibration_all(std::string const &camera_identifier,
                                      uint64_t timestamp,
                                      std::string const &frame_id,
                                      unsigned image_width,
                                      unsigned image_height,
                                      std::string const &distortion_model,
                                      std::array<double, 5> const &D,
                                      std::array<double, 9> const &K,
                                      std::array<double, 9> const &R,
                                      std::array<double, 12> const &P);
    /**
     * @brief Write camera calibration. It is usefull for plotting image in 3D view and debug stereo programs.
     *
     * @param connection_identifier list of connection names where data must be write.
     * @param camera_identifier channel name used in MCAP for this data
     * @param timestamp Timestamp of calibration data
     * @param frame_id Frame of reference for the camera. The origin of the frame is the optical center of the camera. +x points to the right in the image, +y points down, and +z points into the plane of the image.
     * @param image_width Image width
     * @param image_height Image height
     * @param distortion_model Name of distortion model. Supported values: `plumb_bob` and `rational_polynomial`
     * @param D Distortion parameters
     * @param K Intrinsic camera matrix (3x3 row-major matrix)
     * @param R Rectification matrix (stereo cameras only, 3x3 row-major matrix)
     * @param P Projection/camera matrix (3x4 row-major matrix)
     * @return true could write calibration
     * @return false could not write calibration
     */
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
                                     std::array<double, 12> const &P);
    /**
     * @brief Write camera calibration. It is usefull for plotting image in 3D view and debug stereo programs.
     *
     * @param connection_identifier connection name where data must be write.
     * @param camera_identifier channel name used in MCAP for this data
     * @param timestamp Timestamp of calibration data
     * @param frame_id Frame of reference for the camera. The origin of the frame is the optical center of the camera. +x points to the right in the image, +y points down, and +z points into the plane of the image.
     * @param image_width Image width
     * @param image_height Image height
     * @param distortion_model Name of distortion model. Supported values: `plumb_bob` and `rational_polynomial`
     * @param D Distortion parameters
     * @param K Intrinsic camera matrix (3x3 row-major matrix)
     * @param R Rectification matrix (stereo cameras only, 3x3 row-major matrix)
     * @param P Projection/camera matrix (3x4 row-major matrix)
     * @return true could write calibration
     * @return false could not write calibration
     */
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
                                     std::array<double, 12> const &P);

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
    bool write_JSON_to_all(std::string const &identifier, std::string const &serialized_json, uint64_t timestamp);
    /**
     * @brief Write JSON into MCAP. Each JSON data "type" must have a property called "__foxglove_name__" in order to be interpretable by
     * Foxglove studio as valid data. This function is thread safe.
     *
     * @param connection_identifier list of connection names where data must be write.
     * @param identifier identifier to where write the JSON
     * @param serialized_json JSON serialized into string
     * @param timestamp timestamp of JSON
     * @return true could write JSON
     * @return false could not write JSON
     */
    bool write_JSON_to(std::vector<std::string> const &connection_identifier, std::string const &identifier, std::string const &serialized_json, uint64_t timestamp);
    /**
     * @brief Write JSON into MCAP. Each JSON data "type" must have a property called "__foxglove_name__" in order to be interpretable by
     * Foxglove studio as valid data. This function is thread safe.
     *
     * @param connection_identifier connection name where data must be write.
     * @param identifier identifier to where write the JSON
     * @param serialized_json JSON serialized into string
     * @param timestamp timestamp of JSON
     * @return true could write JSON
     * @return false could not write JSON
     */
    bool write_JSON_to(std::string const &connection_identifier, std::string const &identifier, std::string const &serialized_json, uint64_t timestamp);

    /**
     * @brief Add frame transform that could be used for 3D and image
     *
     * @param transform_name Name of the transform
     * @param timestamp Timestamp of transform
     * @param parent Name of the parent frame
     * @param child Name of the child frame
     * @param pose Transform position
     */
    bool add_frame_transform_to_all(std::string transform_name, uint64_t timestamp, std::string parent, std::string child, Eigen::Matrix4f pose);
    /**
     * @brief Add frame transform that could be used for 3D and image
     *
     * @param connection_identifier list of connection names where data must be write.
     * @param transform_name Name of the transform
     * @param timestamp Timestamp of transform
     * @param parent Name of the parent frame
     * @param child Name of the child frame
     * @param pose Transform position
     */
    bool add_frame_transform_to(std::vector<std::string> connection_identifier, std::string transform_name, uint64_t timestamp, std::string parent, std::string child, Eigen::Matrix4f pose);
    /**
     * @brief Add frame transform that could be used for 3D and image
     *
     * @param connection_identifier connection name where data must be write.
     * @param transform_name Name of the transform
     * @param timestamp Timestamp of transform
     * @param parent Name of the parent frame
     * @param child Name of the child frame
     * @param pose Transform position
     */
    bool add_frame_transform_to(std::string connection_identifier, std::string transform_name, uint64_t timestamp, std::string parent, std::string child, Eigen::Matrix4f pose);

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
     * @param scale_invariant Is the thickness must be specified in pixel and be fixed (true) orbeing scale relative to distance to the camera (false)
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
     * @brief Write 3D object into the file
     *
     * @param connection_identifier list of connection names where data must be write.
     * @param object_name Name of the object to push
     * @param timestamp Timestamp in which the object will be pushed
     * @return true Everything does fines.
     * @return false Everything does wrong.
     */
    bool write_3d_object_to(std::vector<std::string> connection_identifier, std::string object_name, uint64_t timestamp);
    /**
     * @brief Write 3D object into the file
     *
     * @param connection_identifier connection name where data must be write.
     * @param object_name Name of the object to push
     * @param timestamp Timestamp in which the object will be pushed
     * @return true Everything does fines.
     * @return false Everything does wrong.
     */
    bool write_3d_object_to(std::string connection_identifier, std::string object_name, uint64_t timestamp);

    /**
     * @brief Used by write_log_to_all. Specify the level of log
     *
     */
    enum class LOG_LEVEL
    {
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
    bool write_log_to_all(std::string const &log_channel_name, uint64_t timestamp, LOG_LEVEL log_level, std::string const &message, std::string const &name, std::string const &file, uint32_t line);
    /**
     * @brief Add log
     *
     * @param connection_identifier list of connection names where data must be write.
     * @param log_channel_name Name of channel that must be used for pushing logs
     * @param timestamp Timestamp of log message
     * @param log_level Log level
     * @param message Log message
     * @param name Process or node name
     * @param file Filename
     * @param line Line number in the file
     */
    bool write_log_to(std::vector<std::string> const &connection_identifier, std::string const &log_channel_name, uint64_t timestamp, LOG_LEVEL log_level, std::string const &message, std::string const &name, std::string const &file, uint32_t line);
    /**
     * @brief Add log
     *
     * @param connection_identifier connection name where data must be write.
     * @param log_channel_name Name of channel that must be used for pushing logs
     * @param timestamp Timestamp of log message
     * @param log_level Log level
     * @param message Log message
     * @param name Process or node name
     * @param file Filename
     * @param line Line number in the file
     */
    bool write_log_to(std::string const &connection_identifier, std::string const &log_channel_name, uint64_t timestamp, LOG_LEVEL log_level, std::string const &message, std::string const &name, std::string const &file, uint32_t line);

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
    bool add_position_to_all(std::string position_channel_name, uint64_t timestamp, Eigen::Matrix4f pose, std::string frame_id = "");
    /**
     * @brief Add position that could be vizualized into 3D. Position could be linked to frame thanks to the `frame_id` parameter.
     *
     * @param connection_identifier list of connection names where data must be write.
     * @param position_channel_name Name of position
     * @param timestamp Timestamp of pose
     * @param pose Pose in 3D space
     * @param frame_id Frame of reference for pose position and orientation
     * @return true  Everything does fines.
     * @return false Everything does wrong.
     */
    bool add_position_to(std::vector<std::string> connection_identifier, std::string position_channel_name, uint64_t timestamp, Eigen::Matrix4f pose, std::string frame_id = "");
    /**
     * @brief Add position that could be vizualized into 3D. Position could be linked to frame thanks to the `frame_id` parameter.
     *
     * @param connection_identifier connection name where data must be write.
     * @param timestamp Timestamp of pose
     * @param pose Pose in 3D space
     * @param frame_id Frame of reference for pose position and orientation
     * @return true  Everything does fines.
     * @return false Everything does wrong.
     */
    bool add_position_to(std::string connection_identifier, std::string position_channel_name, uint64_t timestamp, Eigen::Matrix4f pose, std::string frame_id = "");

    typedef struct CircleAnnotation{
        uint64_t timestamp;
        std::array<int, 2> position;
        double diameter;
        double thickness;
        std::array<double, 4> fill_color;
        std::array<double, 4> outline_color;
    } CircleAnnotation;

    enum class PointAnnotationType{
        UNKNOWN = 0,
        POITNS = 1,
        LINE_LOOP = 2,
        LINE_STRIP = 3,
        LINE_LIST = 4
    };
    typedef struct PointsAnnotation{
        uint64_t timestamp;
        PointAnnotationType type;
        std::vector<std::array<int, 2>> points;
        std::array<double, 4> outline_color;
        std::vector<std::array<double, 4>> outline_colors;
        std::array<double, 4> fill_color;
        double thickness;
    } PointsAnnotation;

    typedef struct TextAnnotation{
        uint64_t timestamp;
        std::array<int, 2> position;
        std::string text;
        double font_size;
        std::array<double, 4> text_color;
        std::array<double, 4> background_color;
    } TextAnnotation;
    
    void add_image_annotation_to_all(std::vector<CircleAnnotation> const &circle_annotations, std::string const& channel_name, std::vector<PointsAnnotation> const &points_annotations, std::vector<TextAnnotation> const &text_annotations, uint64_t timestamp);
    void add_image_annotation_to(std::vector<std::string> const &connection_identifier, std::string const& channel_name, std::vector<CircleAnnotation> const &circle_annotations, std::vector<PointsAnnotation> const &points_annotations, std::vector<TextAnnotation> const &text_annotations, uint64_t timestamp);
    void add_image_annotation_to(std::string const &connection_identifier, std::string const& channel_name, std::vector<CircleAnnotation> const &circle_annotations, std::vector<PointsAnnotation> const &points_annotations, std::vector<TextAnnotation> const &text_annotations, uint64_t timestamp);

};

#endif
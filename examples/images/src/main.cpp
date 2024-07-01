#include <iostream>
#include <chrono>
#include <thread>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include "MCAPWriter.h"

int main(int argc, char **argv){
    // Open MCAP writer:
    mcap_wrapper::open_file_connection("simple.mcap");
    mcap_wrapper::open_network_connection("0.0.0.0", 8765, "simple_server");

    // Open cv::Mat image
    std::string image_path = RESSOURCE_PATH; // `RESSOURCE_PATH` is defined in cmake
    image_path += "/sample_image.jpg";
    cv::Mat image = cv::imread(image_path.c_str());

    // Write data into file:
    for(unsigned i=0; ; i++){
        cv::Mat image_to_save = image.clone();

        // Simple image manipulation
        if(i%2 == 0)
            cv::cvtColor(image_to_save.clone(), image_to_save, cv::COLOR_BGRA2GRAY);

        uint64_t current_timestamp = std::chrono::system_clock::now().time_since_epoch().count();

        // Add anotation to image
        std::vector<mcap_wrapper::CircleAnnotation> circles_annotations;
        mcap_wrapper::CircleAnnotation circle_annotation;
        circle_annotation.timestamp = current_timestamp;
        circle_annotation.position = {rand()%image_to_save.cols, rand() % image_to_save.rows};
        circle_annotation.diameter = 5.0;
        circle_annotation.thickness = 1.0;
        circle_annotation.fill_color = {255,0,0,255};
        circle_annotation.outline_color = {255,255,255,255};
        circles_annotations.push_back(circle_annotation);
        circle_annotation.position = {rand()%image_to_save.cols, rand() % image_to_save.rows};
        circles_annotations.push_back(circle_annotation);
        circle_annotation.position = {rand()%image_to_save.cols, rand() % image_to_save.rows};
        circles_annotations.push_back(circle_annotation);

        std::vector<mcap_wrapper::PointsAnnotation> points_annotations;
        mcap_wrapper::PointsAnnotation point_annotation;
        point_annotation.timestamp = current_timestamp;
        point_annotation.type = mcap_wrapper::PointAnnotationType::POINTS;
        point_annotation.points = std::vector<std::array<int, 2>>();
        point_annotation.points.push_back({100,100});
        point_annotation.points.push_back({200,100});
        point_annotation.points.push_back({100,300});
        point_annotation.thickness = 10.0;
        point_annotation.outline_color = {255,255,255,255};
        point_annotation.fill_color = {0, 255, 0, 255};
        points_annotations.push_back(point_annotation);

        point_annotation.type = mcap_wrapper::PointAnnotationType::LINE_LIST;
        point_annotation.points = std::vector<std::array<int, 2>>();
        point_annotation.points.push_back({200,100});
        point_annotation.points.push_back({220,300});
        point_annotation.points.push_back({200,120});
        point_annotation.points.push_back({200,200});
        point_annotation.outline_color = {255,255,255,255};
        point_annotation.fill_color = {0, 0, 255, 255};
        point_annotation.outline_colors = std::vector<std::array<double, 4>>();
        point_annotation.outline_colors.push_back({255,255,255,255});
        point_annotation.outline_colors.push_back({255,255,255,255});
        point_annotation.outline_colors.push_back({255,255,255,255});
        points_annotations.push_back(point_annotation);

        point_annotation.type = mcap_wrapper::PointAnnotationType::LINE_LOOP;
        point_annotation.points = std::vector<std::array<int, 2>>();
        point_annotation.points.push_back({10,100});
        point_annotation.points.push_back({100,100});
        point_annotation.points.push_back({100,200});
        point_annotation.outline_color = {255,255,255,255};
        point_annotation.fill_color = {0, 255, 255, 255};
        point_annotation.outline_colors = std::vector<std::array<double, 4>>();
        point_annotation.outline_colors.push_back({255,255,255,255});
        point_annotation.outline_colors.push_back({255,255,255,255});
        point_annotation.outline_colors.push_back({255,255,255,255});
        points_annotations.push_back(point_annotation);

        point_annotation.type = mcap_wrapper::PointAnnotationType::LINE_STRIP;
        point_annotation.points = std::vector<std::array<int, 2>>();
        point_annotation.points.push_back({200,300});
        point_annotation.points.push_back({100,100});
        point_annotation.points.push_back({100,200});
        point_annotation.outline_color = {255,255,255,255};
        point_annotation.fill_color = {255, 0, 255, 255};
        point_annotation.outline_colors = std::vector<std::array<double, 4>>();
        point_annotation.outline_colors.push_back({255,255,255,255});
        point_annotation.outline_colors.push_back({255,255,255,255});
        point_annotation.outline_colors.push_back({255,255,255,255});
        points_annotations.push_back(point_annotation);

        std::vector<mcap_wrapper::TextAnnotation> texts_annotations;
        mcap_wrapper::TextAnnotation text_annotation;
        text_annotation.timestamp = current_timestamp;
        text_annotation.position = {150, 150};
        text_annotation.text = "sample text";
        text_annotation.font_size = 1.0;
        text_annotation.text_color = {0,0,0,255};
        text_annotation.background_color = {255,0,0,255};
        texts_annotations.push_back(text_annotation);

        mcap_wrapper::add_image_annotation_to_all("simple_image_annotations", circles_annotations, points_annotations, texts_annotations, current_timestamp);

        
        // Save image:
        mcap_wrapper::write_image_to_all("simple_image", image_to_save, current_timestamp);

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    // Close file
    mcap_wrapper::close_file_connection("simple.mcap");

    return 0;
}

#pragma once
#include "opencv2/core.hpp"
#include <habitat_cv.h>

struct Camera_array{
    Camera_array(const std::string &, const habitat_cv::Camera_order &);
    void capture();
    void reset();
    std::vector<cv::Mat> clones();
    const habitat_cv::Camera_order &camera_order;
    std::vector<cv::Mat> images;
    ~Camera_array();
    unsigned int camera_index(cv::Point);
private:
    std::string config_file;
    void open ();
    void close ();
    unsigned int frame_size;
};
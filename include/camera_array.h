#pragma once
#include "opencv2/core.hpp"
#include <habitat_cv.h>

struct Camera_array{
    Camera_array(const std::string &, unsigned int);
    void capture();
    void reset();
    std::vector<cv::Mat> clones();
    unsigned int camera_count;
    habitat_cv::Images images;
    ~Camera_array();
private:
    std::string config_file;
    void open();
    void close();
    unsigned int frame_size;
};
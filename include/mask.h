#pragma once
#include <opencv2/opencv.hpp>

struct Mask{
    Mask (cv::Size);
    cv::Mat &apply(const cv::Mat &);
    cv::Mat mask;
    cv::Mat masked;
};
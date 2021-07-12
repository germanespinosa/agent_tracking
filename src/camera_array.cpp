#include <camera_array.h>
#include <iostream>
#include <future>

#include "xcliball.h"

using namespace std;
using namespace habitat_cv;

Camera_array::Camera_array(const std::string &config_file_path, const habitat_cv::Camera_order &camera_order) :
camera_order(camera_order) , config_file(config_file_path){
    open();
}

Camera_array::~Camera_array() {
    close();
}

void Camera_array::capture() {
    vector<std::future<int>> futures;
    for (unsigned int c=0; c < camera_order.count(); c++) {
        int grabber_bit_map = 1 << c; // frame grabber identifier is 4 bits with a 1 on the device number.
        pxd_readuchar(grabber_bit_map, 1, 0, 0, -1, -1, images[c].data, frame_size, "Grey");
//        futures.push_back(async(std::launch::async, pxd_readuchar, grabber_bit_map, 1, 0, 0, -1, -1, images[c].data, frame_size, "Grey"));
    }
//    for (auto &future:futures) {
//        future.wait();
//    }
}

std::vector<cv::Mat> Camera_array::clones() {
    std::vector<cv::Mat> res;
    for (auto &image:images){
        res.push_back(image.clone());
    }
    return res;
}

void Camera_array::reset() {
    close();
    open();
}

void Camera_array::open() {
    if (!file_exists(config_file)) {
        cerr << "configuration file " << config_file << " not found" << endl;
        exit(1);
    }
    pxd_PIXCIopen("", "", config_file.c_str());
    if (pxd_goLive(15, 1)){
        cerr << "Failed to initialize frame grabbers" << endl;
        exit(1);
    }
    cv::Size size = {pxd_imageXdim(), pxd_imageYdim()};
    for (unsigned int c=0; c < camera_order.count(); c++){
        images.emplace_back(size.height,size.width,CV_8UC1);
    }
    frame_size = size.width * size.height;
}

void Camera_array::close() {
    pxd_PIXCIclose();
}


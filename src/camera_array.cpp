#include <agent_tracking/camera_array.h>
#include <iostream>
#include <future>

#if XLIBALL_PRESENT
#include "xcliball.h"
#endif

using namespace std;
using namespace habitat_cv;

namespace agent_tracking {

    Camera_array::Camera_array(const std::string &config_file_path, unsigned int camera_count) :
            camera_count(camera_count), config_file(config_file_path) {
        open();
    }

    Camera_array::~Camera_array() {
        close();
    }

    void Camera_array::capture() {
#if XLIBALL_PRESENT
        vector<std::future<int>> futures;
        for (unsigned int c = 0; c < camera_count; c++) {
            int grabber_bit_map = 1 << c; // frame grabber identifier is 4 bits with a 1 on the device number.
            pxd_readuchar(grabber_bit_map, 1, 0, 0, -1, -1, images[c].data, frame_size, "Grey");
        }
#endif
    }

    void Camera_array::reset() {
        close();
        open();
    }

    void Camera_array::open() {
#if XLIBALL_PRESENT
        pxd_PIXCIopen("", "", config_file.c_str());
        if (pxd_goLive(15, 1)) {
            cerr << "Failed to initialize frame grabbers" << endl;
            exit(1);
        }
        cv::Size size = {pxd_imageXdim(), pxd_imageYdim()};
        for (unsigned int c = 0; c < camera_count; c++) {
            images.emplace_back(size.height, size.width, Image::Type::gray);
        }
        frame_size = size.width * size.height;
#endif
    }

    void Camera_array::close() {
#if XLIBALL_PRESENT
        pxd_PIXCIclose();
#endif
    }
}


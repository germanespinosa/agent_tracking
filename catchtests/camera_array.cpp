#include <catch.h>
#include <camera_array.h>
#include <fstream>
#include <habitat_cv.h>

using namespace std;
using namespace habitat_cv;
using namespace cell_world;

TEST_CASE("bare capture"){
    Camera_order co;
    co.load("../config/camera_order.config");
    Camera_array ca("../config/EPIX_lights_on.fmt",co);
    ca.capture();
    for (unsigned int i =0; i < ca.images.size();i++){
        stringstream ss;
        ss << "camera_" << i << ".png";
        cv::imwrite(ss.str(),ca.images[i]);
    }
}

TEST_CASE("composite"){
    Camera_order co;
    co.load("../config/camera_order.config");
    Camera_array ca("../config/EPIX_lights_on.fmt",co);
    Cameras_associations cas;
    cas.load("../config/associations.config");
    Composite comp(cv::Size(980,800),co,cas);
    ca.capture();
    cv::imwrite("composite.png", comp.get_composite(ca.images));

}
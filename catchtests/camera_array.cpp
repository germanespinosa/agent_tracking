#include <catch.h>
#include <fstream>
#include <habitat_cv.h>
#include <agent_tracking/layouts.h>

using namespace std;
using namespace habitat_cv;
using namespace cell_world;

//TEST_CASE("bare capture"){
//    Camera_order co;
//    co.load("../config/camera_order.config");
//    Camera_array ca("../config/EPIX_lights_on.fmt",co);
//    ca.capture();
//    for (unsigned int i =0; i < ca.images.size();i++){
//        stringstream ss;
//        ss << "camera_" << i << ".png";
//        cv::imwrite(ss.str(),ca.images[i]);
//    }
//}

//TEST_CASE("composite"){
//    Camera_order co;
//    co.load("../config/camera_order.config");
//    Camera_array ca("../config/EPIX_lights_on.fmt",co);
//    Cameras_associations cas;
//    cas.load("../config/associations.config");
//    Composite comp(cv::Size(980,800),co,cas);
//    ca.capture();
//    cv::imwrite("composite.png", comp.get_composite(ca.images));
//
//}

//string get_time(double time_stamp){
//    ostringstream oss;
//    unsigned int hour = (unsigned int)time_stamp / 60 / 60;
//    unsigned int minute = (unsigned int)(time_stamp / 60) % 60;
//    unsigned int second =(unsigned int)(time_stamp) % 60;
//    unsigned int millisecond = (unsigned int)(time_stamp * 1000) % 1000;
//    oss << setfill('0') << setw(2) << hour << ":"<<setw(2)<< minute<<":"<<setw(2)<<second<<"."<< setw(3)<< millisecond;
//    return oss.str();
//}

//
//TEST_CASE("get_time"){
//    CHECK(get_time (100.430)=="00:01:40.430");
//}



TEST_CASE("Layouts"){
    string bg_path = "/maze/agent_tracking/backgrounds/experiment/";
    auto images = Images::read(bg_path,{"camera_0.png","camera_1.png","camera_2.png","camera_3.png"});
    auto composite = Image::read(bg_path,"composite.png");
    Main_layout main_layout("FPP1","ROBOT",1);

    auto main_frame = main_layout.get_frame(composite.to_rgb(), 5);
    cv::imshow("main layout", main_frame);
    cv::waitKey();
}
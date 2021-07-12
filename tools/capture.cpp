#include <camera_array.h>
#include <habitat_cv.h>
#include <chrono>
#include <thread>

using namespace habitat_cv;
using namespace cell_world;
using namespace std;

int main(int argc, char **argv){
    if (argc==1){
        cerr << "missing camera configuration parameter." << endl;
        exit(1);
    }
    string cam_config(argv[1]);
    Camera_order co;
    co.load("../config/camera_order.config");
    Camera_array ca("../config/EPIX_" + cam_config + ".fmt",co);
    this_thread::sleep_for(std::chrono::milliseconds(250));
    Cameras_associations cas;
    cas.load("../config/associations.config");
    Composite Pcomp(cv::Size(980,862),co,cas);
    string path = "/maze/agent_tracking/backgrounds/" + cam_config;
    if (argc>2){
        path = argv[2];
    } else {
        cout << "saving images to " << path << endl;
    }
    vector<string> files {"camera_0.png","camera_1.png","camera_2.png","camera_3.png"};
    ca.capture();
    write_images(ca.images,path,files);
    Pcomp.get_composite(ca.images);
    cv::imwrite(path + "/composite.png", Pcomp.composite);
}
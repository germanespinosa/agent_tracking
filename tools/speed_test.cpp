#include <habitat_cv.h>
#include <camera_array.h>
#include <input_handler.h>
#include <frame_rate.h>
#include <future>
#include "opencv2/video.hpp"
#include <thread>


using namespace habitat_cv;
using namespace cell_world;
using namespace std;
using namespace json_cpp;


void show(vector<cv::Mat> &images){
    for (auto &im:images){
        cv::imshow("im",im);
        cv::waitKey(1);
    }
}

int main(int argc, char **argv){
    if (argc==1){
        cerr << "missing camera configuration parameter." << endl;
        exit(1);
    }
    string cam_config(argv[1]);
    Frame_rate fr;
    Camera_order co = Json_from_file<Camera_order>("/maze/agent_tracking/config/camera_order.config");
    Camera_array ca("/maze/agent_tracking/config/EPIX_" + cam_config + ".fmt",co);

    cout << "Camera speed test:" << endl;
    for (int i=0;i<1000;i++) {
        fr.new_frame();
        ca.capture();
//        auto dest_future = std::async(std::launch::async, show ,std::ref(ca.images));
        thread t (show ,std::ref(ca.images) );
        t.detach();
        cout << "\r" << i/10 << "%   " << flush;
    }
    cout << "\r100%" << endl;
    cout << "Results : " << fr.filtered_fps << endl;
}
#include <input_handler.h>
#include <json_cpp.h>
#include <iostream>
#include <fstream>
#include <habitat_cv.h>

using namespace std;
using namespace json_cpp;
using namespace cell_world;
using namespace habitat_cv;


struct Frame_detection : Json_object{
    Json_object_members(
    Add_member(frame);
    Add_member(time_stamp);
    Add_member(theta);
    Add_member(detection_coordinates);
    )
    unsigned int frame;
    double time_stamp;
    double theta;
    Detection_coordinates detection_coordinates;
};


int main(){
    Input_handler input;
    ofstream f;
    f.open("/maze/controller/logs/agent_logs.txt");
    while (true) {
        if (input.ready){
            auto s = input.get_string();
            Frame_detection fd;
            s >> fd;
            if (fd.detection_coordinates.detection_location.profile.agent_name != "robot") continue;
            auto x = fd.detection_coordinates.detection_location.location.x;
            auto y = fd.detection_coordinates.detection_location.location.y;
            auto theta = fd.theta;

            //your code
            //TODO: normalize x and y to each other (y is longer for hexagon)
            if ( s == "exit"){
                break;
            } else{
                f << s << endl << flush;
            }
        }
    }
    f.close();
    exit(0);
}
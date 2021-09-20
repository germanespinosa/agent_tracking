#include <input_handler.h>
#include <json_cpp.h>
#include <iostream>
#include <fstream>
#include <habitat_cv.h>

#include "../include/ptcp.h"
#include "../include/controller.h"

using namespace std;
using namespace json_cpp;
using namespace cell_world;
using namespace habitat_cv;

int BASE_SPEED = 70;
int PROPORTIONAL_SPEED = 10;
int DERIV_SPEED = -5;

//TODO: test these error margins
int TRANS_ERROR_MARGIN = 0.02;
int ROT_ERROR_MARGIN = 0.02;


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


//TODO: is this the proper way to do this function
float sgn(float val) {
    if (val  < __FLT_MIN__) {
        return 0;
    } else {
        return val / abs(val);
    }
}

//translational movement to x value
motor_data traverse_to(location dest, location robot_loc, motor_data &prev_motors) {
    motor_data m;
    float speed;
    //float deriv = 0;

    float dist = dest.x - robot_loc.x;
    float direction = sgn(dist); //NOTE: is this the correct direction?

    if (abs(dist) < TRANS_ERROR_MARGIN) {
        speed = 0;
    } else {
        speed = BASE_SPEED + (PROPORTIONAL_SPEED * dist); //+ (DERIV_SPEED * deriv);
    }

    m.axis1 = -1 * direction * speed;
    m.axis2 = direction * speed;

    m.axis1 = direction * speed;
    m.axis2 = direction * speed;
    return m;
}

//rotational movement to angle value (radians)
motor_data rotate_to(location dest, location robot_loc, motor_data &prev_motors) {
    motor_data m;
    float speed;
    //float deriv = 0;

    float dist = dest.theta - robot_loc.theta;
    float direction = sgn(dist);


    if (abs(dist) < ROT_ERROR_MARGIN) {
        speed = 0;
    } else {
        speed = BASE_SPEED + (PROPORTIONAL_SPEED * dist); //+ (DERIV_SPEED * deriv);
    }

    m.axis1 = -1 * direction * speed;
    m.axis2 = direction * speed;
    return m;
}

int main(){

    location robot_loc;
    //TODO: configure and set mouse location too
    //location mouse_loc;

    motor_data motors;
    motor_data prev_motors;
    prev_motors.axis1 = 0;
    prev_motors.axis2 = 0;

    //Connect a TCP socket to the predator
    int connected = tcp_connect();
    if (connected < 0) {
        cout << "Error connecting over TCP. Terminating program.";
        return -1;
    }
    cout << "Remote Predator connected over TCP\n";

    //Setup input handler
    Input_handler input;

    //Setup output filestream for reporting
    //TODO: report errors to this filestream
    ofstream f;
    f.open("/maze/controller/logs/agent_logs.txt");


    //Configure example simple target destination
    location dest;
    dest.x = 0.9;
    dest.y = 0.5;
    dest.theta = 0;

    while (true) {
        if (input.ready){
            auto s = input.get_string();
            Frame_detection fd;
            s >> fd;
            if (fd.detection_coordinates.detection_location.profile.agent_name != "robot") continue;
            robot_loc.x = fd.detection_coordinates.detection_location.location.x;
            robot_loc.y = fd.detection_coordinates.detection_location.location.y;
            robot_loc.theta = fd.theta;

            //TODO: normalize x and y to each other? (y is longer for hexagon)


            cout << "pos: " << robot_loc.x << ", " << robot_loc.y << ", " << robot_loc.theta << endl;
            motors = rotate_to(dest, robot_loc, prev_motors);
            cout << "motors: " << motors.axis1 << ", " << motors.axis2 << endl;

            int ssent = send_values(motors);
            if (ssent < 0) {
                printf("Failure sending a value. Terminating program");
                return -1;
            }

            prev_motors = motors;
        }
    }
    f.close();
    exit(0);
}

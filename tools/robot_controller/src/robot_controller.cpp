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

int LOW_SPEED = 70;

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


//translational movement to x value
motor_data traverse_to(location dest, location robot_loc) {
    motor_data m;

    //TODO: decide on a proper and tested error value
    if (abs(dest.x - robot_loc.x) < TRANS_ERROR_MARGIN) {
        m.axis1 = 0;
        m.axis2 = 0;
    } else {
        float sign = (robot_loc.x - dest.x) / abs((robot_loc.x - dest.x));

        //TODO: change the robot code so that the sign does not have to be reversed here ever!!
        m.axis1 = sign * speed;
        m.axis2 = sign * speed;
    }
    return m;
}

//rotational movement to angle value (radians)
motor_data rotate_to(location dest, location robot_loc) {
    motor_data m;

    //TODO: decide on a proper and tested error value
    if (abs(dest.theta - robot_loc.theta) < ROT_ERROR_MARGIN) {
        m.axis1 = 0;
        m.axis2 = 0;
    } else {
        float sign = (robot_loc.theta - dest.theta) / abs((robot_loc.theta - dest.theta));

        //TODO: change the robot code so that the sign has to be reversed here!!
        m.axis1 = -1 * sign * speed;
        m.axis2 = sign * speed;
    }
    return m;
}

int main(){

    location robot_loc;
    //TODO: configure and set mouse location too
    //location mouse_loc;

    motor_data motors;

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
            //TODO: use this to compute the next PWM value to send based on the current location to send
            //set_pwm_for_dest(mouse_loc, robot_loc, motors);



            cout << "pos: " << robot_loc.x << ", " << robot_loc.y << ", " << robot_loc.theta << endl;
            motors = rotate_to(dest, robot_loc);
            cout << "motors: " << motors.axis1 << ", " << motors.axis2 << endl;

            int ssent = send_values(motors);
            if (ssent < 0) {
                printf("Failure sending a value. Terminating program");
                return -1;
            }
        }
    }
    f.close();
    exit(0);
}

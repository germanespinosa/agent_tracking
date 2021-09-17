#include "../include/controller.h"
#include <stdio.h>

#include <iostream>
#include <math.h>

using namespace std;

//TODO: - normalize these values to PWMs more precisely
int normalize(int raw_js_value) {
    /*
    int32_t max = 32767;
    int32_t min = -32767;
    int32_t range = (max - min) + 1;
    // normalize between -512 and 511
    int32_t normalized = raw_js_value * 1024 / range;
    */
    int normalized = raw_js_value / 32;
    return normalized; //for now, makes everything a positive number so padding can be used properly
}

int velocity (int pwm) {
    return 0;
}

//convert a translational distance for the robot into a pwm and time value, pass by reference
void for_distance (int dist, int *pwm, int *seconds) {
    
    return;
    
}

//when standing still
//convert a desired turn angle in radians to pwms for both motors in opposite directions for a given amount of time
//pass by reference
void for_rotation (int angle, int *pwm) {
    return;
}

//when moving
void for_rotation_in_motion (int angle, int cur_pwm0, int cur_pwm1) {
    return;
}

void set_pwm_for_dest(struct location dest, struct location rob_loc, struct motor_data md) {
    //rotate until the angle is correct for the location
    bool turn_right = true;
    double angle_to_dest = atan2 ( (dest.y - rob_loc.y), (dest.x, rob_loc.x) ); //if input is degrees, multiply input by (pi/180)
    double curr_angle = rob_loc.theta - 180;

    if (curr_angle > angle_to_dest + THETA_ERROR) {
        //turn counter clockwise
        md.axis1 = -1 * ROTATE_PWM;
        md.axis2 =  1 * ROTATE_PWM;

    } else if (curr_angle < angle_to_dest - THETA_ERROR) {
        //turn clockwise
        md.axis1 =  1 * ROTATE_PWM;
        md.axis2 = -1 * ROTATE_PWM;

    } else {
        //rotation is acceptable move forward

        if (dist_between_loc(rob_loc, dest) > DIST_ERROR) {
            //TODO: slow down the PWMs here until it reaches the destination
            //move forward
            md.axis1 = FORWARD_PWM;
            md.axis2 = FORWARD_PWM;
        } 
    }
}

//quadratic formula bitch
int dist_between_loc(struct location loc1, struct location loc2) {
    return sqrt(pow(loc2.x - loc1.x, 2) + pow(loc2.y - loc1.y, 2));
}
#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <stdint.h> //allows int32 

#define THETA_ERROR 0.01
#define DIST_ERROR 0.01

#define FORWARD_PWM 200
#define ROTATE_PWM 100

struct motor_data {
    int32_t axis1;
    int32_t axis2;
    bool light_trigger;
    bool stimulus_trigger;

};

struct location {
    float x;
    float y;
    float theta;
};

int normalize(int raw_js_value);

void run_instruction();

int velocity (int pwm);

//convert a translational distance for the robot into a pwm and time value
//pass by reference
void for_distance (int dist, int *pwm, int *seconds);

//when standing stil
//convert a desired turn angle in radians to pwms for both motors in opposite directions for a given amount of time
//pass by reference
void for_rotation (int angle, int *pwm, int *seconds);

//when moving
int for_rotation_in_motion (int angle, int cur_pwm0, int cur_pwm1, int total_time);

void set_pwm_for_dest(struct location dest, struct location rob_loc, struct motor_data md);

int dist_between_loc(struct location loc1, struct location loc2);

#endif //CONTROLLER_H

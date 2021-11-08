#pragma once
#include <thread>
#include <camera_array.h>
#include <easy_tcp.h>
#include <message.h>

struct Agent_tracking {
    static void set_camera_file(const std::string &);
    static void set_background_path(const std::string &);
    static void new_episode(const New_episode_message &);
    static void end_episode();
    static int register_consumer(easy_tcp::Service &);
    static void deregister_consumer(int);
    static void tracking_process ();
    static void reset_cameras();
    static void initialize_background();
    static void update_puff();
};
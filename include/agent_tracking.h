#pragma once
#include <agent_tracking/message.h>
#include <agent_tracking/service.h>
#include <agent_tracking/background.h>
#include <agent_tracking/camera_array.h>
#include <agent_tracking/client.h>
#include <agent_tracking/frame_rate.h>
#include <agent_tracking/layouts.h>
#include <agent_tracking/time_stamp.h>

namespace agent_tracking {
    void set_camera_file(const std::string &);
    void set_background_path(const std::string &);
    void set_occlusions(const std::string &);
    void new_episode(const New_episode_message &);
    void end_episode();
    void tracking_process();
    void reset_cameras();
    void initialize_background();
    void update_puff();
}
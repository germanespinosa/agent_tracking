#pragma once
#include <agent_tracking/message.h>

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
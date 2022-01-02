#include <agent_tracking/service.h>

namespace agent_tracking{
    struct Cv_service : Service {
        //experiment
        void new_experiment(const std::string &) override;
        void new_episode(New_episode_message) override;
        void end_episode() override;
        //camera
        void update_background() override;
        void reset_cameras() override;
        void update_puff() override;
        //visualization
        void show_occlusions(const std::string &) override;
        void hide_occlusions() override;
    };
}
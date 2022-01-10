#include <agent_tracking/service.h>

namespace agent_tracking{
    struct Cv_service : Service {
        //experiment
        bool new_experiment(const std::string &) override;
        bool new_episode(New_episode_message) override;
        bool end_episode() override;
        bool update_puff() override;
    };
}
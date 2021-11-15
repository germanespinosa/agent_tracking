#pragma once
#include <cell_world.h>

namespace agent_tracking {
    struct Client : cell_world::Message_client {
        Client();
        bool connect();
        bool connect(const std::string &ip);
        bool connect(const std::string &ip, int port);
        bool new_episode( const std::string &experiment, int episode, const std::string &subject, const std::string &occlusions, const std::string &destination_folder);
        bool end_episode();
        bool register_consumer();
        bool unregister_consumer();
        bool reset_cameras();
        bool update_background();
        bool update_puff();
        cell_world::Message wait_for_result(const std::string &operation, double time_out = 5.0);
        int consumer_id = -1;
        bool episode_in_progress;
    };
}

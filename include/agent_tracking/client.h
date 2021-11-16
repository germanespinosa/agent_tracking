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
        bool register_consumer(auto &callback);
        bool register_consumer(bool automatic = false);
        bool unregister_consumer();
        virtual void new_step(cell_world::Step);
        bool reset_cameras();
        bool update_background();
        bool update_puff();
        cell_world::Message wait_for_result(const std::string &operation, double time_out = 5.0);
        std::atomic<bool> registered_consumer;
        bool episode_in_progress;
        std::thread consumer;
    };
}

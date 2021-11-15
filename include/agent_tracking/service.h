#pragma once
#include <cell_world.h>
#include <agent_tracking.h>

namespace agent_tracking {
    struct Service : cell_world::Message_router {
        Routes(
                Add_route("new_episode", new_episode, New_episode_message);
                Add_route("end_episode", end_episode);
                Add_route("update_background", update_background);
                Add_route("register_consumer", register_consumer);
                Add_route("reset_cameras", reset_cameras);
                Add_route("unregister_consumer", unregister_consumer);
                Add_route("update_puff", update_puff);
                Add_route("set_occlusions", set_occlusions, std::string);
        )

        void unrouted_message(const cell_world::Message &) override;

        // routes
        void new_episode(New_episode_message);
        void end_episode();
        void update_background();
        void register_consumer();
        void reset_cameras();
        void unregister_consumer();
        void update_puff();
        void set_occlusions(const std::string &);
        void on_connect() override;
        void on_disconnect() override;
        static int get_port();
        int consumer_id = -1;
        static void send_update(const cell_world::Step &);
    private:
        void remove_consumer();
    };
}